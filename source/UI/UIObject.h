#pragma once
#include "includes/UISettings.h"
#include "includes/UIStyle.h"

#include "../Engine/Renderer.h"
#include "../Engine/TextManager.h"

#include <string>
//predefine to avoid issues
struct UIsettingScalar;
class Renderer;

class UIObject{
    public:
        UIObject();
        ~UIObject();
        //settings
        UISettingScalar x;
        UISettingScalar y;
        UISettingScalar width;
        UISettingScalar height;
        //drawing
        void draw(Renderer& renderer);
        Vec4 getBoundingRect(Renderer &renderer);
        void setText(Renderer &renderer, std::string textString);
        std::function<void()> onClick;

        bool isHovered = false;
        bool isVisible = false;

        UIObjectStyle style=UIObjectStyle();

        bool enableHoverAndClick=false;
    private:
        Model textModel=Model();
        bool hasText = false;
};