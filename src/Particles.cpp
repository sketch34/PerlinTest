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
    static float s_fEmissionInterval = 0.01;     // How often to emit a particle group.
    static float s_fEmissionPulseRate = 2.5f;
    static float s_fLastEmissionTime = 0.f;
    static int s_iNumToEmit = 15;
    if(!freeList.empty() && s_fLastEmissionTime + s_fEmissionInterval < ofGetElapsedTimef())
    {
        s_fLastEmissionTime = ofGetElapsedTimef();
        int count = (sin(ofGetElapsedTimef() * s_fEmissionPulseRate) + 1.f) * .5f * (float)s_iNumToEmit;
        while(count-- && !freeList.empty())
        {
            list<Particle*>::iterator pIter = freeList.begin();
            Particle* pNew = *pIter;
            activeList.splice(activeList.begin(), freeList, pIter);     // Move a particle from the free to the active list.
        }
    }
    
    // Sim particles.
    static float s_fOverallSpeed = 1.2f;
    static float s_fNoiseInfluence = .017f;
    static float s_fNoiseScale = 3.f;
    static float s_fNoiseRate = .4f;
    static float s_fMaxParticleLife = 2.f;
    const float cfNoiseEvolve = ofGetElapsedTimef() * s_fNoiseRate;
    //static float s_fNoise2Scale = 5.5f;
    
    list<Particle*>::iterator pIter;
    for(pIter = activeList.begin(); pIter != activeList.end();)       // Loop through all active particles.
    {
        Particle& p = **pIter;

        float noise = s_fNoiseInfluence * ((ofNoise(p.pos.x * s_fNoiseScale + cfNoiseEvolve,
                                                    p.pos.y * s_fNoiseScale,
                                                    p.pos.z * s_fNoiseScale) - .5f) * 2.f);   // Center noise around the range [-1, 1].
        
        const float wind = .004f;

        /*
         * Compute a simple physics update.
         */
        ofVec3f vForce((noise + wind), noise, noise);
        ofVec3f vAirResist = p.vel * -2.f;    // Crucial step, add air resistance, which is just a negative force proportional to velocity.
        p.acc = (vForce + vAirResist) * a_fTimeDelta * s_fOverallSpeed;
        p.vel += p.acc;
        p.pos += p.vel;
        p.lifetime += a_fTimeDelta;

        if(p.lifetime > s_fMaxParticleLife)      // Only allow particles to live for this long.
        {
            list<Particle*>::iterator toRemove = pIter;
            ++pIter;
            freeList.push_back(&p);
            activeList.erase(toRemove);
            initParticle(p);
        }
        else
            ++pIter;
    }
}

void Particles::draw()
{
    static char s_buf[128];
    sprintf(s_buf, "Particle Count: %d", (int)activeList.size());
    ofDrawBitmapString(s_buf, 20, 20);
    
    list<Particle*>::iterator pIter;
    for(pIter = activeList.begin(); pIter != activeList.end(); ++pIter)       // Loop through all active particles.
    {
        Particle& p = **pIter;
        ofCircle(p.pos.x * ofGetWidth(), p.pos.y * ofGetHeight(), 1.5f);
    }
}
