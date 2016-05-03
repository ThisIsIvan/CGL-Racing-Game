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
    int hitCircleDistance = 0;
    
    GameObject(vmml::Matrix4f, float, vmml::AABBf);
    bool collidesWith(GameObject);
    vmml::AABBf aabb;
};
#endif