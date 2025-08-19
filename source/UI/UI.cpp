#include "UI.h"
#include "../Engine/Engine.h"
#include "../Engine/EngineAssets.h"
#include "../Engine/TextManager.h"
#include "../Engine/ModuleManager.h"

std::vector<UIObject> UIManager::objects;
UIStyleSet UIManager::style;
int UIManager::hoveredObject=-1;
int UIManager::heldObject=-1;

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

    if (drawBackground) Engine::renderer->drawBorderRect(
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
void UIObject::updateText()
{
    std::vector<Vertex> geometry = TextManager::textMesh(textContent,fontSize);
    textModel.createGeometry(Engine::renderer->openGLContext,geometry);
    textChangeRequested = false;
}
void UIObject::setText(std::string text)
{
    textContent = text;
    textChangeRequested = true;
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

//probably change these two to be more abstract so i can have them either horizontal or vertical
void UIObject::layout_listVertical()
{

    const float listX = screenPositionPixels.x+padding;
    const float listWidth = screenSizePixels.x-(padding*2.0f);
    
    float verticalOffset=screenPositionPixels.y+padding;
    
    for(int& childID : children){
        UIObject& child = UIManager::objects[childID];
        if(!child.visible)continue;
        Vec2& position = child.screenPositionPixels;
        Vec2& size = child.screenSizePixels;

        position.x=listX;
        position.y=verticalOffset;        
        
        size.x=listWidth;
        size.y=child.relativeSize.y;

        verticalOffset+=size.y;
        child.callDraw();
    }
}
void UIObject::layout_splitHorizontal()
{
    const float padding2 = padding*2.0f;

    const float innerRegionWidth = screenSizePixels.x-padding2;
    const float innerRegionHeight = screenSizePixels.y-padding2;

    int visibleChildren=0;
    for(int& childID : children){
        UIObject& child = UIManager::objects[childID];
        visibleChildren+=child.visible;
    }

    const float childWidth = innerRegionWidth/float(visibleChildren);

    const float innerRegionY = screenPositionPixels.y+padding;
    float horizontalOffset = screenPositionPixels.x+padding;

    for(int& childID : children){
        UIObject& child = UIManager::objects[childID];
        if(!child.visible)continue;
        Vec2& position = child.screenPositionPixels;
        Vec2& size = child.screenSizePixels;

        position.y=innerRegionY;
        position.x=horizontalOffset;
        
        size.y=innerRegionHeight;
        size.x=childWidth;
        
        horizontalOffset+=size.x;
        child.callDraw();
    }
}

void UIObject::callDraw()
{
    if(!visible) return;

    if(textChangeRequested) updateText();

    if(drawSelf) onDraw();

    //this could probably be made better, maybe using child classes of a "layout" class
    switch(layout){
        case NONE:layout_none();                        break;
        case LIST_VERTICAL:layout_listVertical();       break;
        case SPLIT_HORIZONTAL:layout_splitHorizontal(); break;
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
    obj.interactable = true;
}

void UIManager::styleUI()
{
    style.normal.font = EngineAssets::tFont;
    style.normal.background = EngineAssets::tBorder;

    style.hovered.font = EngineAssets::tFontHovered;
    style.hovered.background = EngineAssets::tBorderHovered;
}
void UIManager::checkHover(UIObject& obj){
    if(!obj.visible) return;
    Vec2& mousePos = Engine::mousePosition;
    bool hovered = Vec2::pointWithinRect(mousePos,obj.screenPositionPixels,obj.screenPositionPixels+obj.screenSizePixels);
    if(hovered) hoveredObject = obj.ID;
    for(int& objID : obj.children){
        UIObject& child = getObj(objID);
        checkHover(child);
    }
}
void UIManager::hover(Vec2 mousePos)
{
    int previousHoveredObject=hoveredObject;
    if(hoveredObject!=-1){
        UIObject& hoveredObj = getObj(hoveredObject);
        hoveredObj.hovered=false;
        hoveredObject=-1;
    }
    checkHover(UI::mainUI);
    if(hoveredObject!=-1){
        UIObject& hoveredObj = getObj(hoveredObject);
        if(hoveredObj.interactable) hoveredObj.hovered=true;
        const bool newlyHovered = hoveredObject!=previousHoveredObject;
        if(newlyHovered){
            if(hoveredObj.mouseOver) hoveredObj.mouseOver();
        }
    }
}
void UIManager::mouseDown(){
    if(hoveredObject!=-1){
        UIObject& obj = getObj(hoveredObject);
        if(obj.mouseDown) obj.mouseDown();
        heldObject=hoveredObject;
    }
}
void UIManager::mouseUp(){
    if(hoveredObject!=-1){
        UIObject& obj = getObj(hoveredObject);
        if(obj.mouseUp) obj.mouseUp();
        if(heldObject==hoveredObject){
            if(obj.onClick) obj.onClick();
        }
    }
    heldObject=-1;
}

UIObject UI::mainUI = UIObject();
int UI::moduleMenu=0;
int UI::moduleList=0;
int UI::moduleDesc=0;
int UI::addModuleButton=0;
int UI::moduleDescription::name=0;
int UI::moduleDescription::desc=0;
int UI::moduleDescription::creator=0;
void UI::loadModuleDescription(int moduleID){
    ModuleData& module = ModuleManager::modules[moduleID];
    UIObject& nameDisplay = UIManager::getObj(UI::moduleDescription::name);
    UIObject& descDisplay = UIManager::getObj(UI::moduleDescription::desc);    
    UIObject& creatorDisplay = UIManager::getObj(UI::moduleDescription::creator);    

    nameDisplay.setText(module.name);
    creatorDisplay.setText("by "+module.creator);
    descDisplay.setText(module.description);
}

void UI::initUI(){
    //dont want to draw the base ui thing
    mainUI.drawSelf=false;

    //idk why it takes so many reserves but like prevent the resizing idfk
    UIManager::objects.reserve(15);

    //create ui stuff, will eventually make functions for this
    UIObject& moduleMenuUI = UIManager::createChild(UI::mainUI,UIObject());
    UIObject& moduleListUI = UIManager::createChild(moduleMenuUI,UIObject());
    UIObject& moduleDescriptionUI = UIManager::createChild(moduleMenuUI,UIObject());
    moduleMenuUI.visible = false;

    moduleMenu = moduleMenuUI.ID;
    moduleList = moduleListUI.ID;
    moduleDesc = moduleDescriptionUI.ID;

    moduleMenuUI.layout=SPLIT_HORIZONTAL;
    moduleListUI.layout=LIST_VERTICAL;
    moduleDescriptionUI.layout=LIST_VERTICAL;

    //generate the description area
    constexpr float unit = 20.0f;

    UIObject& moduleDescName = UIManager::createChild(moduleDescriptionUI,UIObject());
    UIObject& moduleDescCreator = UIManager::createChild(moduleDescriptionUI,UIObject());
    UIObject& moduleDescDesc = UIManager::createChild(moduleDescriptionUI,UIObject());
    
    moduleDescName.drawBackground = false;
    moduleDescCreator.drawBackground = false;
    

    moduleDescName.fontSize=unit*1.5f;
    
    moduleDescName.relativeSize.y=unit*3.0f;
    moduleDescDesc.relativeSize.y=unit*2.0f;
    moduleDescCreator.relativeSize.y=unit*1.5f;

    moduleDescDesc.setText("module description goes here.");
    moduleDescCreator.setText("module creator");
    moduleDescName.setText("module name");
    moduleDescription::name = moduleDescName.ID;
    moduleDescription::desc = moduleDescDesc.ID;
    moduleDescription::creator = moduleDescCreator.ID;
    

    //create the button for opening and closing the add module menu
    UIObject& openModuleMenuButton = UIManager::createChild(UI::mainUI,UIObject());
    addModuleButton=openModuleMenuButton.ID;
    openModuleMenuButton.relativeSize={80.0f,40.0f};
    UIManager::makeInteractable(openModuleMenuButton);
    openModuleMenuButton.onClick=[]{
        UIObject& addModuleMenu = UIManager::getObj(UI::moduleMenu);
        addModuleMenu.visible = !addModuleMenu.visible;
    };

    openModuleMenuButton.setText("add");

}
void UI::onResize(int width,int height){
    if(mainUI.children.size()<=0) return;
    
    constexpr float padding = 10.0f;
    constexpr float padding2=padding*2.0f;

    UIObject& moduleMenuUI = UIManager::getObj(moduleMenu);
    moduleMenuUI.relativePosition = {padding,padding};
    moduleMenuUI.relativeSize = {float(width)-padding2,float(height)-padding2};

    float screenRatio = float(width)/float(height);
    bool showModuleDescription = screenRatio > 0.75;
    UIObject& moduleDescriptionUI = UIManager::getObj(moduleDesc);
    moduleDescriptionUI.visible=showModuleDescription;

    UIObject& openModuleMenuButton = UIManager::getObj(addModuleButton);
    openModuleMenuButton.relativePosition.x=float(width/2.0f)-(openModuleMenuButton.relativeSize.x/2.0f);
    openModuleMenuButton.relativePosition.y=float(height)-openModuleMenuButton.relativeSize.y;
}
void UI::createModuleList(){
    constexpr float listButtonSize = 40.0f;

    UIManager::objects.reserve(ModuleManager::modules.size());      //prevent the objects from resizing and losing reference
    UIObject& moduleListUI = UIManager::getObj(moduleList);
    //probably need to rewrite this later
    int i=0;
    for(ModuleData& module : ModuleManager::modules){
        //REPLACE THIS WITH A BUTTON LATER
        UIObject& newOBJ = UIManager::createChild(moduleListUI,UIObject());
        newOBJ.relativeSize.y = listButtonSize;
        newOBJ.setText(module.name);
        int moduleID=i;
        newOBJ.mouseOver = [moduleID] {
             loadModuleDescription(moduleID);
        };
        newOBJ.onClick = [moduleID] {
            UIObject& moduleMenuUI = UIManager::getObj(moduleMenu);
            moduleMenuUI.visible = false;
            ModuleData* modulePtr = &ModuleManager::modules[moduleID];
            ModuleManager::loadModule(modulePtr);
        };
        UIManager::makeInteractable(newOBJ);
        i++;
    }
}