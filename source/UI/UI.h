#pragma once
#include "../Engine/Includes/DataTypes.h"
#include <vector>
#include "../Engine/Includes/Model.h"

/*how to arrange the objects within another object*/
enum UILayout {
    NONE, //objects are not positioned relative to the screen
    LIST_VERTICAL //objects are arranged from top to bottom
};
class UIManager;
/*The base ui object class that gets extended*/
class UIObject{
    public:
        virtual void callDraw();    /*positions and draws the children of the object*/
        virtual void onDraw();      /*draws the object itself*/

        void setText(std::string& text);

        Vec2 relativePosition;      /*position relative to parent container (only really used in freeform container)*/
        Vec2 relativeSize;          /*size relative to parent container (only really used in freeform container)*/

        bool drawSelf=true;         /*if false, the object wont draw itself but will still draw children*/
        float padding=10.0f;

        std::vector<int> children;
        UILayout layout = UILayout::NONE;/*controls how children are arranged within the object*/

        int ID=-1;

        //add a pointer to a style later so that buttons can have unique styles if they need to

        bool hovered=false;
    private:
        Model textModel;

        Vec2 screenPositionPixels;  /*position in pixels used when rendering*/
        Vec2 screenSizePixels;      /*size in pixels used when rendering*/

        void layout_none();
        void layout_listVertical();
        

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
        static std::vector<int> interactables;

        //adds the given object to the parent object, returns a reference to the child object
        static UIObject& createChild(UIObject& parent, UIObject&& child);   
        //returns a ref to the object based on ID
        static UIObject& getObj(int ID);   
        static void makeInteractable(UIObject& obj);
        static void styleUI();
        /*scans through and checks for hoverable objects*/
        static void hover(Vec2 mousePos);
        static int hoveredObject;

        /*default style for stuff*/
        static UIStyleSet style;
};