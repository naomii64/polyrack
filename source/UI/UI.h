#pragma once
#include "../Engine/Includes/DataTypes.h"
#include <vector>
#include "../Engine/Includes/Model.h"

/*how to arrange the objects within another object*/
enum UILayout {
    NONE, //objects are not positioned relative to the screen
    LIST_VERTICAL, //objects are arranged from top to bottom
    SPLIT_HORIZONTAL    //arranges objects horizontally making sure theyre all the same width
};
class UIManager;
/*The base ui object class that gets extended*/
class UIObject{
    public:
        void callDraw();            /*positions and draws the children of the object*/
        void onDraw();              /*draws the object itself*/
        std::function<void()> onResizeCallback;

        void setText(std::string text);

        Vec2f relativePosition;      /*position relative to parent container (only really used in freeform container)*/
        Vec2f relativeSize;          /*size relative to parent container (only really used in freeform container)*/

        bool drawSelf=true;         /*if false, the object wont draw itself but will still draw children*/
        bool drawBackground=true;   /*if false, the background wont be drawn and only the text*/
        bool visible = true;

        bool resizeToFitContent=false;    /*changes the size of the size of the container to fit its content, mostly for lists*/

        bool interactable = false;  /*will it change color on hover*/

        float fontSize=20.0f;       /*how big the font of this object is, only updates when setText is run*/
        float padding=10.0f;

        std::string textContent;
        bool textChangeRequested=false;

        std::vector<int> children;
        UILayout layout = NONE;/*controls how children are arranged within the object*/

        int ID=-1;

        bool hovered=false;

        std::function<void()> mouseOver;
        std::function<void()> mouseDown;
        std::function<void()> mouseUp;
        std::function<void()> onClick;

    private:
        Model textModel;
        
        Vec2f screenPositionPixels;  /*position in pixels used when rendering*/
        Vec2f screenSizePixels;      /*size in pixels used when rendering*/

        void layout_none();
        void layout_listVertical();
        void layout_splitHorizontal();

        void updateText();
    friend class UIObject;
    friend class UIManager;
};

//list of links to textures
struct UIStyle{
    int background=0;
    int font=0;
};
struct UIStyleSet{
    UIStyle normal;
    UIStyle hovered;
};
//maybe combine these two classes later? it was supposed to be UI is where everything is actually stored but idk anymore

class UIManager{
    public:
        static std::vector<UIObject> objects;

        //adds the given object to the parent object, returns a reference to the child object
        static UIObject& createChild(UIObject& parent, UIObject&& child);   
        //returns a ref to the object based on ID
        static UIObject& getObj(int ID);   
        /*
        Reserves extra space in the vector for objects to prevent reallocation.
        reserves additional space instead of how vector.reserve works
        */
        static void reserveObjects(int count);
        static void makeInteractable(UIObject& obj);
        static void styleUI();
        /*scans through and checks for hoverable objects*/
        static void hover(Vec2f mousePos);
        static void mouseDown();
        static void mouseUp();

        static int heldObject;
        static int hoveredObject;

        /*default style for stuff*/
        static UIStyleSet style;
    private:
        static void checkHover(UIObject& obj);
};
class OBJ_Module;
/*a static class where ui objects and references are stored*/
class UI{
    public:
        static void initUI();
        static void onResize(int width,int height);
        static void createModuleList();
        static void loadModuleDescription(int moduleID);

        static void openContextMenu(int menuObjectID);/*shows a context menu and brings it to the mouse, does not actually set the context of the menu*/
        static void closeContextMenu(int menuObjectID);
        static void openModuleContext(OBJ_Module* moduleObject);

        static UIObject mainUI;   //main ui object
        static int moduleMenu;
        static int moduleList;
        static int moduleDesc;

        static OBJ_Module* currentModuleContext;

        static int addModuleButton;

        struct moduleDescription{
            static int name;
            static int creator;
            static int desc;
        };
        
        struct contextMenus{
            static int cable;
            static int modules;
        };

    
        static inline constexpr float contextMenuWidth=200.0f;
        static inline constexpr float contextMenuButtonHeight=40.0f;

    private:
        static void initModuleMenu();
        static void initContextMenus();

        static void createOpenModuleMenuButton();
        static void createModuleMenuLayout();

        static UIObject& createContextMenuBase(int& IDstorageLocation,std::string menuName);
};