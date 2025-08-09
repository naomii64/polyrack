#pragma once
#include "ModuleManager.h"
#include <iostream>
#include "Engine.h"

std::vector<ModuleData> ModuleManager::modules;
ModuleData& ModuleManager::addModule()
{
    std::cout << "adding module to data list..." << std::endl;
    return modules.emplace_back();
}

void ModuleManager::loadModule(ModuleData *moduleData)
{
    std::cout<<"loading module...\n";
    //start by creating the object
    auto& moduleObject = static_cast<OBJ_Module&>(*Engine::objects.emplace_back(std::make_unique<OBJ_Module>()));
    Engine::scene->children.push_back(&moduleObject);
    moduleObject.createFrom(moduleData);
}
