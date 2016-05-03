//
//  GameObject.cpp
//  bRenderer_ios
//
//  Created by dev_lab on 03/05/16.
//
//

#include "GameObject.h"
#include <math.h>

GameObject::GameObject(vmml::Matrix4f modelMatrix, float hitCircleDistance){
    this->modelMatrix = modelMatrix;
    this->hitCircleDistance = hitCircleDistance;
}

bool GameObject::collidesWith(GameObject obj){
    float distance = sqrtf(powf(modelMatrix.x() - obj.modelMatrix.x(), 2.0f) + powf(modelMatrix.y() - obj.modelMatrix.y(), 2.0f) + powf(modelMatrix.z() - obj.modelMatrix.z(), 2.0f));
    bool collides = distance <= (hitCircleDistance);
    return collides;
}
