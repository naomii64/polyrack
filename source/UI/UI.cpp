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

    Vec2f& position = screenPositionPixels;
    Vec2f& size = screenSizePixels;

    constexpr float borderWidth = 10.0f;

    if (drawBackground) Engine::renderer->drawBorderRect(
        position.x,position.y,
        size.x,size.y,
        borderWidth,
        stylePtr->background
    );

    Vec3f textDrawLocation(0.0f);
    Vec3f textDrawScale(1.0f);
    Vec3f textDrawRotation(0.0f);
    
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
    
    float childrenSize=0.0f;

    for(int& childID : children){
        UIObject& child = UIManager::objects[childID];
        if(!child.visible)continue;
        Vec2f& position = child.screenPositionPixels;
        Vec2f& size = child.screenSizePixels;

        position.x=listX;
        position.y=verticalOffset;        
        
        size.x=listWidth;
        size.y=child.relativeSize.y;

        verticalOffset+=size.y;
        
        if(resizeToFitContent) childrenSize+=size.y;
        child.callDraw();
    }
    if(resizeToFitContent) relativeSize.y=childrenSize+(padding*2.0f);
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
        Vec2f& position = child.screenPositionPixels;
        Vec2f& size = child.screenSizePixels;

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

    if(onResizeCallback) onResizeCallback();

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
    Vec2f& mousePos = Engine::mousePosition;
    bool hovered = Vec2f::pointWithinRect(mousePos,obj.screenPositionPixels,obj.screenPositionPixels+obj.screenSizePixels);
    if(hovered) hoveredObject = obj.ID;
    for(int& objID : obj.children){
        UIObject& child = getObj(objID);
        checkHover(child);
    }
}
void UIManager::hover(Vec2f mousePos)
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
void UIManager::reserveObjects(int count){
    objects.reserve(objects.size()+count);
}

UIObject UI::mainUI = UIObject();
int UI::moduleMenu=0;
int UI::moduleList=0;
int UI::moduleDesc=0;
int UI::addModuleButton=0;
int UI::moduleDescription::name=0;
int UI::moduleDescription::desc=0;
int UI::moduleDescription::creator=0;
int UI::contextMenus::cable=0;
int UI::contextMenus::modules=0;
OBJ_Module* UI::currentModuleContext=nullptr;

void UI::loadModuleDescription(int moduleID){
    ModuleData& module = ModuleManager::modules[moduleID];
    UIObject& nameDisplay = UIManager::getObj(UI::moduleDescription::name);
    UIObject& descDisplay = UIManager::getObj(UI::moduleDescription::desc);    
    UIObject& creatorDisplay = UIManager::getObj(UI::moduleDescription::creator);    

    nameDisplay.setText(module.name);
    creatorDisplay.setText("by "+module.creator);
    descDisplay.setText(module.description);
}

void UI::createOpenModuleMenuButton()
{    
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
void UI::createModuleMenuLayout()
{
    //create ui stuff, will eventually make functions for this
    UIObject& moduleMenuUI = UIManager::createChild(UI::mainUI,UIObject());
    UIObject& moduleListUI = UIManager::createChild(moduleMenuUI,UIObject());
    UIObject& moduleDescriptionUI = UIManager::createChild(moduleMenuUI,UIObject());
    
    UIObject& moduleDescName = UIManager::createChild(moduleDescriptionUI,UIObject());
    UIObject& moduleDescCreator = UIManager::createChild(moduleDescriptionUI,UIObject());
    UIObject& moduleDescDesc = UIManager::createChild(moduleDescriptionUI,UIObject());

    moduleMenuUI.visible = false;

    moduleMenu = moduleMenuUI.ID;
    moduleList = moduleListUI.ID;
    moduleDesc = moduleDescriptionUI.ID;

    moduleMenuUI.layout=SPLIT_HORIZONTAL;
    moduleListUI.layout=LIST_VERTICAL;
    moduleDescriptionUI.layout=LIST_VERTICAL;

    //generate the description area
    constexpr float unit = 20.0f;
    
    moduleDescName.drawBackground = false;
    moduleDescCreator.drawBackground = false;
    

    moduleDescName.fontSize=unit*1.5f;
    
    moduleDescName.relativeSize.y=unit*3.0f;
    moduleDescDesc.relativeSize.y=unit*2.0f;
    moduleDescCreator.relativeSize.y=unit*1.5f;

    moduleDescription::name = moduleDescName.ID;
    moduleDescription::desc = moduleDescDesc.ID;
    moduleDescription::creator = moduleDescCreator.ID;
}

void UI::initModuleMenu()
{
    UIManager::reserveObjects(7);
    createModuleMenuLayout();       //6 objects
    createOpenModuleMenuButton();   //1 object
}
UIObject &UI::createContextMenuBase(int &IDstorageLocation,std::string menuName)
{
    //needs to reserve 2, probably reserve some ahead of time anyway
    UIManager::reserveObjects(2);

    UIObject& contextMenuObject = UIManager::createChild(UI::mainUI,UIObject());
    IDstorageLocation=contextMenuObject.ID;
    contextMenuObject.layout=LIST_VERTICAL;
    contextMenuObject.resizeToFitContent=true;
    contextMenuObject.relativeSize.x=UI::contextMenuWidth;
    
    UIObject& contextMenuLabel = UIManager::createChild(contextMenuObject,UIObject());
    contextMenuLabel.relativeSize.y=UI::contextMenuButtonHeight;
    contextMenuLabel.setText(menuName+":");
    contextMenuLabel.drawBackground = false;

    return contextMenuObject;
}
void UI::initContextMenus(){
    UIManager::reserveObjects(15);

    UIObject& moduleContextMenu = UI::createContextMenuBase(contextMenus::modules,"Module");
    UIObject& moduleDeleteButton = UIManager::createChild(moduleContextMenu,UIObject());
    moduleDeleteButton.relativeSize.y=UI::contextMenuButtonHeight;
    moduleDeleteButton.setText("Delete");
    UIManager::makeInteractable(moduleDeleteButton);
    moduleDeleteButton.onClick=[]{
        if(!UI::currentModuleContext) return;
        OBJ_Module& module = *UI::currentModuleContext;
        module.deleteObj();
        UI::closeContextMenu(UI::contextMenus::modules);
        UI::currentModuleContext=nullptr;
    };

    moduleContextMenu.visible = false;
}
void UI::openContextMenu(int menuObjectID){
    UIObject& contextMenu = UIManager::getObj(menuObjectID);
    contextMenu.visible = true;
    contextMenu.relativePosition = Engine::mousePosition;
}
void UI::closeContextMenu(int menuObjectID){
    UIObject& contextMenu = UIManager::getObj(menuObjectID);
    contextMenu.visible = false;
}

void UI::openModuleContext(OBJ_Module *moduleObject)
{
    UI::openContextMenu(UI::contextMenus::modules);
    UI::currentModuleContext=moduleObject;
}

void UI::initUI(){
    mainUI.drawSelf=false;
    initModuleMenu();
    initContextMenus();
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

    UIManager::reserveObjects(ModuleManager::modules.size());

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