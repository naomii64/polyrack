#pragma once
#include "UIManager.h"
#include "../Engine/Scene.h"

UIMenu UIManager::mainUI;
UIObject* UIManager::hoveredObject=nullptr;
int UIManager::currentUIGroup=0;
std::vector<std::vector<UIObject*>> UIManager::groups;
int UIManager::moduleSelectionGroup=0;

void UIManager::init(Renderer& renderer){
    std::cout << "initializing ui..." <<    std::endl;
    moduleSelectionGroup = newUIGroupID();

    //add a test button to module selection menu
    UIObject& test = mainUI.addObject();
    assignGroup(test,moduleSelectionGroup);

    //100 pixels
    const int padding=25;
    const int moduleButtonHeight=100;

    test.x.units=float(padding);
    test.y.units=float(padding);

    test.width.percentage=1.0f;
    test.width.units=float(-2*padding);
    test.height.percentage=1.0f;
    test.height.units=float(-2*padding);

    //that was the bg, now go through and add all the buttons
    int i = 0;
    for(ModuleData& moduleData : ModuleManager::modules){
        UIObject& moduleButton = mainUI.addObject();
        assignGroup(moduleButton,moduleSelectionGroup);
        
        moduleButton.enableHoverAndClick=true;    

        moduleButton.width.percentage=1.0f;
        moduleButton.width.units=float(-4*padding);
        
        moduleButton.height.units=moduleButtonHeight;
        moduleButton.y.units=(float(i)*moduleButtonHeight)+float(2*padding);
        moduleButton.x.units=float(padding*2);
        moduleButton.style.hovered.backgroundColor=Vec4(0.5f,0.5f,0.5f,1.0f);
        //increment i to keep track of the current module
        moduleButton.setText(renderer,moduleData.name);

        moduleButton.onClick = [&moduleData] {
            Scene::addModuleFromData(moduleData);        
            UIManager::hideGroup(UIManager::moduleSelectionGroup);    
        };

        i++;
    }
}
void UIManager::drawAll(Renderer& renderer) {
    mainUI.draw(renderer);
}
//position is from -1 to 1
void UIManager::updateMousePos(Renderer& renderer,Vec2 position){
    if (hoveredObject != nullptr) {
        hoveredObject->isHovered = false;
        hoveredObject = nullptr;
    }
    for(UIObject& object : mainUI.getAllObjects()){
        //only check the hitbox of ui objects that allow hovering
        if(!object.enableHoverAndClick){continue;}

        Vec4 boundingRect=object.getBoundingRect(renderer);
        Vec2 relativeMousePos=position-boundingRect.xy();
        if(relativeMousePos.x<boundingRect.z&&relativeMousePos.y<boundingRect.w&&relativeMousePos.x>0&&relativeMousePos.y>0){
            hoveredObject=&object;
        }
    }
    if(hoveredObject==nullptr){return;}
    hoveredObject->isHovered=true;
}
int UIManager::newUIGroupID(){
    return currentUIGroup++;
}
void UIManager::assignGroup(UIObject& obj, int groupID){
    if (groups.size() <= groupID) {
        groups.resize(groupID + 1); // Ensure index is valid
    }
    groups[groupID].push_back(&obj);
}
void UIManager::hideGroup(int groupID){
    if (groupID < 0 || groupID >= groups.size()) {
        return; // Group doesn't exist
    }
    for(UIObject* obj : groups[groupID]){
        obj->isVisible=false;
    }
}
void UIManager::showGroup(int groupID){
    if (groupID < 0 || groupID >= groups.size()) {
        return; // Group doesn't exist
    }
    for(UIObject* obj : groups[groupID]){
        obj->isVisible=true;
    }
}
void UIManager::click(const juce::MouseEvent& event){
    if(hoveredObject==nullptr){return;}
    if(!hoveredObject->isVisible){return;}
    if (hoveredObject->onClick) {
        hoveredObject->onClick();
    }
}