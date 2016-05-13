//
//  GameObject.cpp
//  bRenderer_ios
//
//  Created by dev_lab on 03/05/16.
//
//

#include "GameObject.h"
#include <math.h>


GameObject::GameObject(vmml::Vector3f scaling, vmml::Vector3f translation, vmml::Vector3f rotation, float angle, ObjectType type){
    modelMatrix = vmml::create_translation(translation) * vmml::create_scaling(scaling)* vmml::create_rotation(angle, rotation);
    this->type = type;
}

GameObject::GameObject(vmml::Matrix4f modelMatrix, vmml::AABBf aabb, ObjectType type){
    this->modelMatrix = modelMatrix;
    this->aabb = aabb;
    this->type = type;
}

void init(ObjectManagerPtr ptr){
}

bool GameObject::collidesWith(GameObject obj){
    vmml::Matrix4f bbMin = vmml::create_translation(aabb.getMin()) * modelMatrix;
    vmml::Matrix4f bbMax = vmml::create_translation(aabb.getMax()) * modelMatrix;
    
    float max_x = bbMax.x();
    float max_y = bbMax.y();
    float max_z = bbMax.z();
    float min_x = bbMin.x();
    float min_y = bbMin.y();
    float min_z = bbMin.z();
    
    bbMin = vmml::create_translation(obj.aabb.getMin()) * obj.modelMatrix;
    bbMax = vmml::create_translation(obj.aabb.getMax()) * obj.modelMatrix;
    
    float omax_x = bbMax.x();
    float omax_y = bbMax.y();
    float omax_z = bbMax.z();
    float omin_x = bbMin.x();
    float omin_y = bbMin.y();
    float omin_z = bbMin.z();
    
    float max[] = {max_x, max_y,max_z};
    float min[] = {min_x, min_y,min_z};
    float omax[] = {omax_x, omax_y,omax_z};
    float omin[] = {omin_x, omin_y,omin_z};
    
    
    for(int i = 0; i < 3; i++){
        if(min[i] > omax[i] || omin[i] > max[i]){
            return false;
        }
    }
    return true;
}

ObjectType GameObject::getType(){
    return type;
}

