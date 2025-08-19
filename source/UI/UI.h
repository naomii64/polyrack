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
        virtual void callDraw();    /*positions and draws the children of the object*/
        virtual void onDraw();      /*draws the object itself*/

        void setText(std::string text);

        Vec2 relativePosition;      /*position relative to parent container (only really used in freeform container)*/
        Vec2 relativeSize;          /*size relative to parent container (only really used in freeform container)*/

        bool drawSelf=true;         /*if false, the object wont draw itself but will still draw children*/
        bool drawBackground=true;   /*if false, the background wont be drawn and only the text*/
        bool visible = true;

        bool interactable = false;  /*will it change color on hover*/

        float fontSize=20.0f;       /*how big the font of this object is, only updates when setText is run*/
        float padding=10.0f;

        std::string textContent;
        bool textChangeRequested=false;

        std::vector<int> children;
        UILayout layout = UILayout::NONE;/*controls how children are arranged within the object*/

        int ID=-1;

        bool hovered=false;

        std::function<void()> mouseOver;
        std::function<void()> mouseDown;
        std::function<void()> mouseUp;
        std::function<void()> onClick;

    private:
        Model textModel;

        Vec2 screenPositionPixels;  /*position in pixels used when rendering*/
        Vec2 screenSizePixels;      /*size in pixels used when rendering*/

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

class UIManager{
    public:
        static std::vector<UIObject> objects;

        //adds the given object to the parent object, returns a reference to the child object
        static UIObject& createChild(UIObject& parent, UIObject&& child);   
        //returns a ref to the object based on ID
        static UIObject& getObj(int ID);   
        static void makeInteractable(UIObject& obj);
        static void styleUI();
        /*scans through and checks for hoverable objects*/
        static void hover(Vec2 mousePos);
        static void mouseDown();
        static void mouseUp();

        static int heldObject;
        static int hoveredObject;

        /*default style for stuff*/
        static UIStyleSet style;
    private:
        static void checkHover(UIObject& obj);
};
/*a static class where ui objects and references are stored*/
class UI{
    public:
        static void initUI();
        static void onResize(int width,int height);
        static void createModuleList();
        static void loadModuleDescription(int moduleID);

        static UIObject mainUI;   //main ui object
        static int moduleMenu;
        static int moduleList;
        static int moduleDesc;

        static int addModuleButton;

        struct moduleDescription{
            static int name;
            static int creator;
            static int desc;
        };
};