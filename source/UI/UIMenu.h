#pragma once
#include "UIObject.h"
#include <vector>
#include "../Engine/Renderer.h"

class Renderer;  // Forward declare to avoid issues
class UIObject;

//ui is split up into ui menus wich have their own buttons and stuff
class UIMenu{
    public:
        UIMenu();
        ~UIMenu();
        void draw(Renderer& renderer);
        UIObject& addObject();
        std::vector<UIObject> &getAllObjects();

    private:
        std::vector<UIObject> uiObjects;
};