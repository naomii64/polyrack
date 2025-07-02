#include <vector>
#include <string>

//will store stuff like models, texture ids, ect for each module
struct ModuleData
{
    std::string name="[MODULE NAME]";
    std::string description="[MODULE DESCRIPTION]";
    std::vector<std::string> tags;

};

class ModuleManager{
    public:
        static ModuleData& addModule();
        static std::vector<ModuleData> modules;
};
