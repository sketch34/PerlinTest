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
    static float s_fLastEmissionTime = 0.f;
    static int s_iNumToEmit = 10;
    if(!freeList.empty() && s_fLastEmissionTime + s_fEmissionInterval < ofGetElapsedTimef())
    {
        s_fLastEmissionTime = ofGetElapsedTimef();
        int count = s_iNumToEmit;
        while(count--)
        {
            list<Particle*>::iterator pIter = freeList.begin();
            Particle* pNew = *pIter;
            activeList.splice(activeList.begin(), freeList, pIter);     // Move a particle from the free to the active list.
        }
    }
    
    // Sim particles.
    static float s_fNoiseInfluence = .1f;
    static float s_fNoiseScale = 2.f;
    static float s_fNoiseRate = .2f;
    const float cfNoiseEvolve = ofGetElapsedTimef() * s_fNoiseRate;

    
     list<Particle*>::iterator pIter;
     for(pIter = activeList.begin(); pIter != activeList.end(); ++pIter)       // Loop through all active particles.
     {
        Particle& p = **pIter;
        
        const float noise = s_fNoiseInfluence * ((ofNoise(p.pos.x * s_fNoiseScale + cfNoiseEvolve,
                                                          p.pos.y * s_fNoiseScale + cfNoiseEvolve,
                                                          p.pos.z * s_fNoiseScale + cfNoiseEvolve) * 2.f) - 1.f);

        const float wind = .05f;
        /*
        // Optional to noise the wind as well. Don't worry about it, save some CPU.
        static float s_fWindScale = 1.f;
        static float s_fWindRate = 0.1f;
        static float s_fWindBias = 0.1f;    // How much the wind tends left to right.
        const float wind = (ofNoise(p.pos.x * s_fWindScale + cfNoiseEvolve,
                                    p.pos.y * s_fWindScale + cfNoiseEvolve,
                                    p.pos.z * s_fWindScale + cfNoiseEvolve)
                                    - s_fWindBias) * s_fWindRate;
        */
        
        /*
         * Compute a simple physics update.
         */
        ofVec3f vForce((noise + wind), noise, noise);
        ofVec3f vAirResist = p.vel * -15.f;    // Crucial step, add air resistance, which is just a negative force proportional to velocity.
        p.acc = (vForce + vAirResist) * a_fTimeDelta;
        p.vel += p.acc;
        p.pos += p.vel;
        p.lifetime += a_fTimeDelta;
        
        //if(p.lifetime > 8.f)      // Only allow particles to live for this long.
        //{
        //    initParticle(p);
        //    freeList.splice(freeList.begin(), activeList, pIter);     // Move a particle from the free to the active list.
        //}
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
