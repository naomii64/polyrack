#pragma once
#include "Scene.h"
#include <stdio.h>
#include "Includes/CustomMath.h"
#include <algorithm>

//vars
//std::vector<ModuleInstance> Scene::instances;
//camera settings
float Scene::zoom=10.0f;
float Scene::realZoom=Scene::zoom;
Vec2 Scene::panPosition;
Vec2 Scene::panVelocity;

//constants
float Scene::maxPanVelocity=1.0f;
float Scene::panAcceleartion=0.01f;

Vec2 Scene::mousePos;

//functions
// void Scene::addModuleFromData(const ModuleData &data)
// {
//     ModuleInstance& instance = instances.emplace_back();
//     instance.moduleData=&data;
//     instance.init();
// }
// void Scene::draw(Renderer& renderer){
//     for(ModuleInstance& instance : instances){
//         instance.draw(renderer);
//     }
// }

//runs every frame before drawing
void Scene::applyCameraSettings(Renderer& renderer){
    Vec3 cameraPos;
    Vec3 cameraRot;

    //zoom
    realZoom=Math::lerp(realZoom,zoom,0.5f);    
    cameraPos.z=realZoom;

    //pan
    Vec2 input;
    panVelocity*=0.9f;
    if(juce::KeyPress::isKeyCurrentlyDown('w')){input.y++;}
    if(juce::KeyPress::isKeyCurrentlyDown('s')){input.y--;}
    if(juce::KeyPress::isKeyCurrentlyDown('d')){input.x++;}
    if(juce::KeyPress::isKeyCurrentlyDown('a')){input.x--;}
    panVelocity+=input*panAcceleartion;
    panVelocity.y=std::clamp(panVelocity.y,-maxPanVelocity,maxPanVelocity);
    panVelocity.x=std::clamp(panVelocity.x,-maxPanVelocity,maxPanVelocity);
    panPosition+=panVelocity;
    cameraPos.x=panPosition.x;
    cameraPos.y=panPosition.y;

    //rotational pan (didnt like how it felt)
    /*
    //degrees
    const float lookTurnAmount = 2.5f;
    Vec2 normalizedMousePos=((mousePos/renderer.screenSize)-Vec2(0.5f))*2.0f;

    cameraRot.y=-normalizedMousePos.x*lookTurnAmount*(juce::MathConstants<float>::pi/180.0f);
    cameraRot.x=-normalizedMousePos.y*lookTurnAmount*(juce::MathConstants<float>::pi/180.0f);
    */
    renderer.setCameraPosition(cameraPos,cameraRot);
}