//
//  Car.cpp
//  bRenderer_ios
//
//  Created by dev_lab on 04/05/16.
//
//

#include "Car.h"
#include "GameObject.h"
#include "ObjectType.h"
#include <math.h>

Car::Car(vmml::Vector3f scaling, vmml::Vector3f translation, vmml::Vector3f rotation, float angle) :
GameObject(scaling, translation, rotation, angle, ObjectType::NORMAL){
    speed = 0.f;
    boost = 5;
}

bool Car::handleCollision(GameObject obj){
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
            case ObjectType::BOARD:
                speed = 0.f;
                break;
            default:
                break;
        }
        
        return true;
    }
    return false;
}

bool Car::move(vmml::Matrix4f rotationY){
    bool checkPointPassed = false;
    if(speed != 0){
        vmml::Vector3f planeChange=vmml::Vector3f(0.f, -1.f, speed/100.f);
    
        vmml::Matrix4f planeMotion=vmml::create_translation(planeChange);
        modelMatrix *= planeMotion;
    
        vmml::Matrix4f rotationMatrix = rotationY;
        modelMatrix *= rotationMatrix;
    
        for(int i = 0; i < collidables.size(); i++){
            if(handleCollision(collidables.at(i)) && collidables.at(i).getType() == ObjectType::CHECKPOINT){
                checkPointPassed = true;
            }
            
        }
    }
    
    return checkPointPassed;
}

void Car::accelerate(){
    speed = speed + std::max(2., 2*std::log(speed/10.)/2.);
    speed = std::min((float)MAX_SPEED, speed);
    boost += 1;
}

void Car::decelerate(){
    speed = speed * .99;
    boost += 1;
}

void Car::brake(){
    speed = std::max(0., speed-3.22);
    boost += 1;
}

bool Car::activateBoost(){
    if(boost > 10){
        boost -= 10;
        speed = speed + std::max(2., 10*std::log(speed/10.));
        speed = std::min(1.1f * MAX_SPEED, speed);
        return true;
    }
    else{
        decelerate();
        return false;
    }
}

void Car::addCollidable(GameObject obj){
    collidables.push_back(obj);
}

void Car::clearCollidables(){
    collidables.clear();
}

void Car::reset(){
    speed = 0.0f;
    boost = 5;
    modelMatrix = vmml::create_translation(vmml::Vector3f(1.0f, 0.0f, 1.f));
}