#pragma once
#include "ModuleManager.h"
#include <iostream>

std::vector<ModuleData> ModuleManager::modules;
ModuleData& ModuleManager::addModule()
{
    std::cout << "adding module to data list..." << std::endl;
    return modules.emplace_back();
}
