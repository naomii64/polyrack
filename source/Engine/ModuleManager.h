#pragma once
#include <vector>
#include <string>
#include "Includes/Model.h"
#include "Includes/Animation.h"
//predefine
class Component;
//will store stuff like models, texture ids, ect for each module
struct ModuleData
{
    std::string name="[MODULE NAME]";
    std::string description="[MODULE DESCRIPTION]";
    std::string creator="[CREATOR OF MODULE]";
    std::vector<std::string> tags;
    juce::Array<juce::var> layout;
    int width;
    int height;
    std::vector<Model> models;
    std::vector<Animation> animations;
};

class ModuleManager{
    public:
        static ModuleData& addModule();
        static void loadModule(ModuleData* moduleData);
        static std::vector<ModuleData> modules;
};
