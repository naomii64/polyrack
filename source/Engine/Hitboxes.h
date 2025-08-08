#pragma once
#include "../Defines.h"
#include "Includes/DataTypes.h"
#include <vector>
#include "Includes/Ray.h"
#include "Renderer.h"
class Renderer;

struct Hitbox
{
    Vec3 bounds;
    Vec3 position;
    std::function<void(Vec2 delta,Vec2 mousePos)> onDrag;
    std::function<void()> mouseDown;
    std::function<void()> mouseUp;

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
    static void click(Renderer& renderer,juce::MouseEvent event);
    static void mouseUp(juce::MouseEvent event);
    static void dragHitbox(Vec2 delta,Vec2 mousePos);
    static Hitbox* selectedHitbox;
};