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
    std::vector<std::unique_ptr<Component>> components;

    void draw(Renderer& renderer){
        for (std::unique_ptr<Component>& component : components) {
            component->draw(renderer);
        }
    }
    //call after moduleData is set to create the instances
    void init() {
        std::cout << "copying over components..." << std::endl;
        for (const auto& component : moduleData->components) {
            std::unique_ptr<Component> cloned = component->clone();
            cloned->initInstance();
            components.push_back(std::move(cloned));
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