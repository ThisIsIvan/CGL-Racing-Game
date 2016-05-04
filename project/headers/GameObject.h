//
//  object.h
//  bRenderer_ios
//
//  Created by dev_lab on 03/05/16.
//
//
#include "bRenderer.h"

#ifndef bRenderer_ios_object_h
#define bRenderer_ios_object_h
class GameObject{
public:
    vmml::Matrix4f modelMatrix;
    vmml::AABBf aabb;
    
    GameObject(vmml::Vector3f, vmml::Vector3f, vmml::Vector3f, float);
    GameObject(vmml::Matrix4f, vmml::AABBf);
    void init(ObjectManagerPtr);
    bool collidesWith(GameObject);
};
#endif