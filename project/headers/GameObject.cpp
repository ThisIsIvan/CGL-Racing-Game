//
//  GameObject.cpp
//  bRenderer_ios
//
//  Created by dev_lab on 03/05/16.
//
//

#include "GameObject.h"
#include <math.h>

GameObject::GameObject(vmml::Matrix4f modelMatrix, float hitCircleDistance, vmml::AABBf aabb){
    this->modelMatrix = modelMatrix;
    this->hitCircleDistance = hitCircleDistance;
    this->aabb = aabb;
}

bool GameObject::collidesWith(GameObject obj){
    float distance = sqrtf(powf(modelMatrix.x() - obj.modelMatrix.x(), 2.0f) + powf(modelMatrix.y() - obj.modelMatrix.y(), 2.0f) + powf(modelMatrix.z() - obj.modelMatrix.z(), 2.0f));
    bool collides = distance <= (hitCircleDistance);
    
    float max_x = aabb.getMax().x() + modelMatrix.x();
    float max_y = aabb.getMax().y() + modelMatrix.y();
    float max_z = aabb.getMax().z() + modelMatrix.z();
    float min_x = aabb.getMin().x() + modelMatrix.x();
    float min_y = aabb.getMin().y() + modelMatrix.y();
    float min_z = aabb.getMin().z() + modelMatrix.z();
    
    float omax_x = obj.aabb.getMax().x() + obj.modelMatrix.x();
    float omax_y = obj.aabb.getMax().y() + obj.modelMatrix.y();
    float omax_z = obj.aabb.getMax().z() + obj.modelMatrix.z();
    float omin_x = obj.aabb.getMin().x() + obj.modelMatrix.x();
    float omin_y = obj.aabb.getMin().y() + obj.modelMatrix.y();
    float omin_z = obj.aabb.getMin().z() + obj.modelMatrix.z();
    
    float max[] = {max_x, max_y,max_z};
    float min[] = {min_x, min_y,min_z};
    float omax[] = {omax_x, omax_y,omax_z};
    float omin[] = {omin_x, omin_y,omin_z};
    
    
    for(int i = 0; i < 3; i++){
        if(min[i] > omax[i]|| omin[i] > max[i]){
            return false;
        }
    }
    return true;
}

