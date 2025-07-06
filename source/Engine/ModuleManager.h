#pragma once
#include <vector>
#include <string>
#include "Components.h"
#include "Includes/Model.h"
#include "Includes/Animation.h"
//predefine
class Component;
//will store stuff like models, texture ids, ect for each module
struct ModuleData
{
    std::string name="[MODULE NAME]";
    std::string description="[MODULE DESCRIPTION]";
    std::vector<std::string> tags;
    std::vector<std::unique_ptr<Component>> components;
    std::vector<Model> models;
    std::vector<Animation> animations;
};

class ModuleManager{
    public:
        static ModuleData& addModule();
        static std::vector<ModuleData> modules;
};
