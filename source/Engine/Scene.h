#pragma once
#include "ModuleManager.h"
#include <iostream>
#include "Includes/DataTypes.h"
#include "Renderer.h"
#include <vector>

//predefines
class Renderer;

struct ModuleInstance{
    Vec3 position;
    const ModuleData* moduleData;
    void draw(Renderer& renderer){
        // draw the instance of the module
        for(const std::unique_ptr<Component>& component : moduleData->components){
            component->draw(renderer);
        }
    }
};

//static class
class Scene{
    public:
        static void addModuleFromData(const ModuleData& data);
        static void draw(Renderer& renderer);
        static std::vector<ModuleInstance> instances;
};