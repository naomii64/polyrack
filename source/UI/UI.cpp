#include "UI.h"
#include "../Engine/Engine.h"
#include "../Engine/EngineAssets.h"
#include "../Engine/TextManager.h"

std::vector<UIObject> UIManager::objects;
std::vector<int> UIManager::interactables;
UIStyleSet UIManager::style;
int UIManager::hoveredObject=-1;

void UIObject::onDraw(){
    UIStyle* stylePtr;
    //select a style from the styleset
    if(hovered){
        stylePtr=&UIManager::style.hovered;
    }else{
        stylePtr=&UIManager::style.normal;
    }

    Vec2& position = screenPositionPixels;
    Vec2& size = screenSizePixels;

    constexpr float borderWidth = 10.0f;

    Engine::renderer->drawBorderRect(
        position.x,position.y,
        size.x,size.y,
        borderWidth,
        stylePtr->background
    );

    Vec3 textDrawLocation(0.0f);
    Vec3 textDrawScale(1.0f);
    Vec3 textDrawRotation(0.0f);
    
    textDrawLocation.x=screenPositionPixels.x+padding;
    textDrawLocation.y=screenPositionPixels.y-padding+screenSizePixels.y;
    
    Engine::renderer->drawModelAt(
        textModel,
        textDrawLocation,
        textDrawRotation,
        textDrawScale,
        stylePtr->font
    );
}
void UIObject::setText(std::string &text)
{
    constexpr float fontSize = 20.0f;
    const std::vector<Vertex> geometry = TextManager::textMesh(text,fontSize);
    textModel.createGeometry(Engine::renderer->openGLContext,geometry);
}
void UIObject::layout_none()
{
    for(int& childID : children){
        UIObject& child = UIManager::objects[childID];
        child.screenPositionPixels=child.relativePosition;
        child.screenSizePixels=child.relativeSize;
    
        child.callDraw();
    }
}
void UIObject::layout_listVertical()
{
    float verticalOffset=screenPositionPixels.y+padding;
    
    const float listX = screenPositionPixels.x+padding;
    const float listWidth = screenSizePixels.x-(padding*2.0f);
    for(int& childID : children){
        UIObject& child = UIManager::objects[childID];
        Vec2 size(0.0f);
        Vec2 position(0.0f);
        
        size.x=listWidth;
        size.y=child.relativeSize.y;
        position.x=listX;
        position.y=verticalOffset;        

        child.screenPositionPixels=position;
        child.screenSizePixels=size;

        verticalOffset+=size.y;
        child.callDraw();
    }
}
void UIObject::callDraw()
{

    if(drawSelf) onDraw();

    //this could probably be made better, maybe using child classes of a "layout" class
    switch(layout){
        case NONE:layout_none();                    break;
        case LIST_VERTICAL:layout_listVertical();   break;
    }
}

UIObject& UIManager::createChild(UIObject& parent, UIObject&& child)
{
    int childID = objects.size();
    parent.children.push_back(childID);

    // Move the child directly into the vector
    UIObject& obj = UIManager::objects.emplace_back(std::move(child));

    obj.ID = childID;
    return obj;
}

UIObject& UIManager::getObj(int ID)
{
    return objects[ID];
}

void UIManager::makeInteractable(UIObject &obj)
{
    interactables.push_back(obj.ID);
}

void UIManager::styleUI()
{
    style.normal.font = EngineAssets::tFont;
    style.normal.background = EngineAssets::tBorder;

    style.hovered.font = EngineAssets::tFontHovered;
    style.hovered.background = EngineAssets::tBorderHovered;
}

//assumes objects are already ordered from back to front
void UIManager::hover(Vec2 mousePos)
{
    //unhover the hovered object
    if(hoveredObject!=-1){
        UIObject& hoveredObj = getObj(hoveredObject);
        hoveredObj.hovered=false;
        hoveredObject=-1;
    }
    //scan for a new one
    int newHoveredObject = -1;

    for(int& objID : interactables){
        UIObject& obj = getObj(objID);
        bool hovered = Vec2::pointWithinRect(mousePos,obj.screenPositionPixels,obj.screenPositionPixels+obj.screenSizePixels);
        if(hovered) newHoveredObject=obj.ID;
    }

    hoveredObject=newHoveredObject;
    if(hoveredObject!=-1){
        UIObject& hoveredObj = getObj(hoveredObject);
        hoveredObj.hovered=true;
    }
}
