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
GameObject(scaling, translation, rotation, angle, ObjectType::NORMAL){
    speed = 0.f;
    boost = 5;
}

void Car::handleCollision(GameObject obj){
    if(GameObject::collidesWith(obj)){
        switch(obj.getType()){
            case ObjectType::FLOOR:
                modelMatrix = vmml::create_translation(vmml::Vector3f(0.0f, 1.f, 0.f)) * modelMatrix;
                break;
            case ObjectType::NORMAL:
                modelMatrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, -.5f)) * modelMatrix;
                speed = 0.f;
                break;
            case ObjectType::CHECKPOINT:
                
                break;
            default:
                break;
        }
    }
}

void Car::move(vmml::Matrix4f rotationY){
    if(speed > 0){
        vmml::Vector3f planeChange=vmml::Vector3f(0.f, -1.f, speed/50*1.f);
    
        vmml::Matrix4f planeMotion=vmml::create_translation(planeChange);
        modelMatrix *= planeMotion;
    
        vmml::Matrix4f rotationMatrix = rotationY;
        modelMatrix *= rotationMatrix;
    
        for(int i = 0; i < collidables.size(); i++){
            handleCollision(collidables.at(i));
        }
    }
}

void Car::accelerate(){
    speed = speed + std::max(2., 2*std::log(speed/10.));
    boost += 1;
}

void Car::decelerate(){
    speed = speed * .975;
    boost += 1;
}

void Car::brake(){
    speed = std::max(0., speed - 10.);
    boost += 1;
}

void Car::activateBoost(){
    if(boost > 10){
        boost -= 10;
        speed = speed + std::max(2., 10*std::log(speed/10.));
    }
}

void Car::addCollidable(GameObject obj){
    collidables.push_back(obj);
}

void Car::clearCollidables(){
    collidables.clear();
}