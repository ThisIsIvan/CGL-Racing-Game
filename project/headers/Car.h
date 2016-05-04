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
    
    Car(vmml::Vector3f, vmml::Vector3f, vmml::Vector3f, float);
    void handleCollision(GameObject);
    void move(vmml::Matrix4f);
};

#endif
