//
//  Particles.cpp
//  PerlinTest
//
//  Created by Jarrod Smith on 19/09/13.
//
//

#include "Particles.h"

Particles::Particles()
{
    for(int i = 0; i < MAX_PARTICLES; ++i)
    {
        initParticle(particles[i]);
        freeList.push_back(&particles[i]);      // Add all particles to free list.
    }
}

void Particles::update(float a_fTimeDelta)
{
    // Add particles to simulation.
    static float s_fEmissionInterval = 0.01;     // How often to emit a particle.
    static float s_fLastEmissionTime = 0.f;
    if(!freeList.empty() && s_fLastEmissionTime + s_fEmissionInterval < ofGetElapsedTimef())
    {
        s_fLastEmissionTime = ofGetElapsedTimef();
        list<Particle*>::iterator pIter = freeList.begin();
        Particle* pNew = *pIter;
        activeList.splice(activeList.begin(), freeList, pIter);     // Move a particle from the free to the active list.
    }
    
    // Sim particles.
    const float cfNoiseEvolve = ofGetElapsedTimef();
    static float s_fNoiseRate = 0.1f;
    static float s_fWindScale = 0.1f;
    static float s_fNoiseScale = 5.f;
    
     list<Particle*>::iterator pIter;
     for(pIter = activeList.begin(); pIter != activeList.end(); ++pIter)       // Loop through all active particles.
     {
        const float ft = s_fNoiseRate * a_fTimeDelta;
        Particle& p = **pIter;
        
        const float nx = (ofNoise(p.pos.x * s_fNoiseScale + cfNoiseEvolve) * 2.f) - 1.f;    // Make noise in range [-1, 1].
        const float ny = (ofNoise(p.pos.y * s_fNoiseScale + cfNoiseEvolve) * 2.f) - 1.f;
        const float nz = (ofNoise(p.pos.z * s_fNoiseScale + cfNoiseEvolve) * 2.f) - 1.f;
        
        const float wind = ofNoise(p.pos.x * s_fWindScale, p.pos.y * s_fWindScale);         // 2D is fine for wind. Probably 1D would be fine as well.
        
        // Compute a simple physics update.
        ofVec3f force((nx + wind), ny, nz);
        p.acc = force * ft;
        p.vel += p.acc;
        p.pos += p.vel;
        
        if(p.pos.x > ofGetWidth())      // Gone off edge of screen, recycle.
        {
            initParticle(p);
            freeList.splice(freeList.begin(), activeList, pIter);     // Move a particle from the free to the active list.
        }
    }
}

void Particles::draw()
{
    list<Particle*>::iterator pIter;
    for(pIter = activeList.begin(); pIter != activeList.end(); ++pIter)       // Loop through all active particles.
    {
        Particle& p = **pIter;
        ofCircle(p.pos.x * ofGetWidth(), p.pos.y * ofGetHeight(), 2.f);
    }
}
