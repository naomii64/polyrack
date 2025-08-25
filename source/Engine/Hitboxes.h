#pragma once
#include "../Defines.h"
#include "Includes/DataTypes.h"
#include <vector>
#include "Includes/Ray.h"
#include "Renderer.h"
class Renderer;

struct Hitbox
{
    Vec3f bounds;
    Vec3f position;
    std::function<void(Vec2f delta,Vec2f mousePos)> onDrag;
    std::function<void()> mouseDown;
    std::function<void()> mouseUp;
    std::function<void()> rightClick;
    bool active=true;/*if false, a hitbox is disabled and can be overritten*/

    #if draw_hitboxes
    Model model;
    #endif

};


class HitboxManager{
public:
    static std::vector<Hitbox> hitboxes;
    #if draw_hitboxes
    static void drawAllHitBoxes(Renderer& renderer);
    #endif
    static Hitbox& createHitbox();
    /*
    Creates a hitbox and returns the index of it in the vector of hitboxes.
    Useful to avoid hitboxes moving around in memory.
    */
    static int createHitboxID();
    static void deleteHitbox(int hitboxID);
    static void leftMouseDown();
    static void rightMouseDown();
    static void mouseUp(juce::MouseEvent event);
    static void dragHitbox(Vec2f delta,Vec2f mousePos);
    static Hitbox* getHitboxUnderPixel(Vec2f pixel);
    static Hitbox* selectedHitbox;

    static int deletedHitboxes;
};