#pragma once
#include "UIMenu.h"

UIMenu::UIMenu(){}
UIMenu::~UIMenu(){}
void UIMenu::draw(Renderer& renderer){
    for(UIObject& object : uiObjects){
        object.draw(renderer);
    }
}
UIObject& UIMenu::addObject(){
    std::cout << "UI OBJECT ADDED!" << std::endl;
    return uiObjects.emplace_back();
}

std::vector<UIObject>&  UIMenu::getAllObjects(){
    return uiObjects;
}