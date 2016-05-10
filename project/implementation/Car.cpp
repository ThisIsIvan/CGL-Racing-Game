//
//  Car.cpp
//  bRenderer_ios
//
//  Created by dev_lab on 04/05/16.
//
//

#include "Car.h"
#include "GameObject.h"
#include <math.h>

Car::Car(vmml::Vector3f scaling, vmml::Vector3f translation, vmml::Vector3f rotation, float angle) :
GameObject(scaling, translation, rotation, angle){
    speed = 0.f;
}

void Car::handleCollision(GameObject obj){
    if(GameObject::collidesWith(obj)){
        modelMatrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, -5.f)) * modelMatrix;
        speed = 0.f;
    }
}

void Car::move(vmml::Matrix4f rotationY){
    
    vmml::Vector3f planeChange=vmml::Vector3f(0.f,0.f,speed/50*10.f);
    
    vmml::Matrix4f planeMotion=vmml::create_translation(planeChange);
    modelMatrix = planeMotion * modelMatrix;
    
    vmml::Matrix4f rotationMatrix = rotationY;
    modelMatrix = rotationMatrix * modelMatrix;
    
    for(int i = 0; i < collidables.size(); i++){
        handleCollision(collidables.at(i));
    }
}

void Car::accelerate(){
    speed = speed + std::max(2., 2*std::log(speed/10.));
}

void Car::decelerate(){
    speed = speed * .975;
}

void Car::brake(){
    speed = std::max(0., speed - 10.);
}

void Car::addCollidable(GameObject obj){
    collidables.push_back(obj);
}