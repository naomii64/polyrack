#pragma once
#include "UIMenu.h"
#include "../Engine/Renderer.h"
#include <vector>

class Renderer;  // Forward declare to avoid issues
class UIObject;  
class UIMenu;

class UIManager {
public:
    static void init(Renderer& renderer);
    static void drawAll(Renderer& renderer);
    static void updateMousePos(Renderer &renderer, Vec2 position);
    static void click(const juce::MouseEvent& event);

    //ui groups
    static int newUIGroupID();
    static void assignGroup(UIObject &obj, int groupID);
    static void hideGroup(int groupID);
    static void showGroup(int groupID);
    static int currentUIGroup;
    static std::vector<std::vector<UIObject*>> groups;

    static int moduleSelectionGroup;

    static UIMenu mainUI;
    static UIObject* hoveredObject;
};