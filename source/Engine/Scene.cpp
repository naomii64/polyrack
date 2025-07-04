#pragma once
#include "Scene.h"
#include <stdio.h>

//vars
std::vector<ModuleInstance> Scene::instances;
//functions
void Scene::addModuleFromData(const ModuleData &data)
{
    ModuleInstance& instance = instances.emplace_back();
    instance.moduleData=&data;
}
void Scene::draw(Renderer& renderer){
    for(ModuleInstance& instance : instances){
        instance.draw(renderer);
    }
}