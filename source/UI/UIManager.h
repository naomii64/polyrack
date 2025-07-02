#pragma once
#include "UIMenu.h"
#include "../Engine/Renderer.h"

class Renderer;  // Forward declare to avoid issues
class UIObject;  
class UIMenu;

class UIManager {
public:
    static void init(Renderer& renderer);
    static void drawAll(Renderer& renderer);
    static void updateMousePos(Renderer &renderer, Vec2 position);
    static UIMenu mainUI;
    static UIObject* hoveredObject;
};