//
//  Particles.h
//  PerlinTest
//
//  Created by Jarrod Smith on 19/09/13.
//
//

#ifndef __PerlinTest__Particles__
#define __PerlinTest__Particles__

#include <iostream>
#include <ofMain.h>
#include <ofVec3f.h>

#define MAX_PARTICLES 5000

struct Particle
{
    ofVec3f pos;
};

class Particles
{
public:
    Particles();
    void update(float a_fTimeDelta);
    void draw();
    
private:
    void initParticle(Particle& p);
    
public:
    Particle particles[MAX_PARTICLES];
    std::list<Particle*> freeList;          // Available (inactive) particles.
    std::list<Particle*> activeList;        // Active particles.
};

inline void Particles::initParticle(Particle& p)
{
    // Just init in a random rect to the left of the screen for this test.
    p.pos.x = ofRandom(.2f) + .1f;
    p.pos.y = ofRandom(-.3f, .3f) + 0.5;
    p.pos.z = 0.f;
}

#endif /* defined(__PerlinTest__Particles__) */
