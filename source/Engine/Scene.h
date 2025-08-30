#pragma once
#include "ModuleManager.h"
#include <iostream>
#include "Includes/DataTypes.h"
#include "Renderer.h"
#include <vector>

//predefines
class Renderer;

//static class
class Scene{
    public:
        //static void addModuleFromData(const ModuleData& data);
        //static void draw(Renderer& renderer);
        static void applyCameraSettings(Renderer& renderer);
        //static std::vector<ModuleInstance> instances;
        //camera settings
        static float zoom;
        static float realZoom;
        static Vec2f panVelocity;
        static Vec2f panPosition;
        static float maxPanVelocity;
        static float panAcceleartion;

        //will probably move later so mouse pos is always in one place
        static Vec2f mousePos;
};