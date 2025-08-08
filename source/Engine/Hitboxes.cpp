#include "Hitboxes.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include "Renderer.h"

#if draw_hitboxes
#include "EngineAssets.h"
#endif

std::vector<Hitbox> HitboxManager::hitboxes; // DEFINITION
Hitbox* HitboxManager::selectedHitbox=nullptr;

Hitbox &HitboxManager::createHitbox()
{
    return hitboxes.emplace_back();
}

int HitboxManager::createHitboxID()
{
    hitboxes.emplace_back();
    return int(hitboxes.size())-1;
}

void HitboxManager::click(Renderer &renderer, juce::MouseEvent event)
{
    Vec2 clickPos={float(event.x),float(event.y)};
    Ray ray=renderer.rayFrom(clickPos);
    /*
    std::cout << "screen click pos: " << clickPos.toString() << std::endl;
    std::cout << "ray start: " << ray.origin.toString() << std::endl;
    std::cout << "ray direction: " << ray.direction.toString() << std::endl;
    */

    float dist;

    Hitbox* finalHitBox=nullptr;

    for(Hitbox& hb : hitboxes){
        AABB aabb;
        aabb.min=hb.position-hb.bounds;
        aabb.max=hb.position+hb.bounds;
        
        RayHit hit=rayIntersectsAABB(ray,aabb);
        if(hit.hit){
            //if this is the first detected box, set the distance
            if((finalHitBox==nullptr)||(hit.distance<dist)){
                dist=hit.distance;
                finalHitBox=&hb;
            }
        }    
    }

    if(finalHitBox!=nullptr){
        //if a hitbox was clicked then 
        selectedHitbox=finalHitBox;
        if(selectedHitbox->mouseDown) selectedHitbox->mouseDown();
    }
}
void HitboxManager::mouseUp(juce::MouseEvent event)
{
    if(!selectedHitbox) return;
    if(selectedHitbox->mouseUp) selectedHitbox->mouseUp();
    selectedHitbox=nullptr;
}
void HitboxManager::dragHitbox(Vec2 delta,Vec2 mousePos)
{
    if(selectedHitbox==nullptr) return;
    if(selectedHitbox->onDrag) selectedHitbox->onDrag(delta,mousePos);
}

#if draw_hitboxes
void HitboxManager::drawAllHitBoxes(Renderer& renderer)
{
    for(Hitbox& hitbox : hitboxes){
        renderer.drawModelAt(
            EngineAssets::mWireCube,
            hitbox.position,
            {0.0f,0.0f,0.0f},
            hitbox.bounds,
            EngineAssets::tBlank,
            {1.0f,1.0f,1.0f,0.5f}
        );
    }
}
#endif
