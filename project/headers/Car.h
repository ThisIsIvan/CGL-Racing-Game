//
//  Car.h
//  bRenderer_ios
//
//  Created by dev_lab on 04/05/16.
//
//

#ifndef bRenderer_ios_Car_h
#define bRenderer_ios_Car_h

#include "GameObject.h"

class Car : public GameObject{
public:
    float speed;
    int boost;
    const int MAX_BOOST = 300;
    const int MAX_SPEED = 120;
    
    Car(vmml::Vector3f, vmml::Vector3f, vmml::Vector3f, float);
    bool move(vmml::Matrix4f);
    void accelerate();
    void decelerate();
    void brake();
    bool activateBoost();
    void addCollidable(GameObject);
    void clearCollidables();
    void reset();

private:
    std::vector<GameObject> collidables;
    
    bool handleCollision(GameObject);
};

#endif
