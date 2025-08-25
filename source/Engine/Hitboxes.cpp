#include "Hitboxes.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include "Renderer.h"
#include "Engine.h"

#if draw_hitboxes
#include "EngineAssets.h"
#endif

std::vector<Hitbox> HitboxManager::hitboxes; // DEFINITION
Hitbox* HitboxManager::selectedHitbox=nullptr;
int HitboxManager::deletedHitboxes=0;

Hitbox &HitboxManager::createHitbox()
{
    return hitboxes.emplace_back();
}

int HitboxManager::createHitboxID()
{
    int returnID;
    bool inactiveHitboxExists = deletedHitboxes>0;
    if(inactiveHitboxExists){
        returnID=0;
        for(Hitbox& hb : hitboxes){
            if(!hb.active){
                hb.active=true;
                deletedHitboxes--;
                break;
            }
            returnID++;
        }
    }else{
        returnID = int(hitboxes.size());
        hitboxes.emplace_back();
    }

    std::cout << "new hitbox #"<<returnID<<" | total count: "<<hitboxes.size()<<"\n";
    return returnID;
}

Hitbox* HitboxManager::getHitboxUnderPixel(Vec2f pixel){
    Ray ray=Engine::renderer->rayFrom(pixel);

    float dist;
    Hitbox* finalHitBox=nullptr;

    for(Hitbox& hb : hitboxes){
        if(!hb.active) continue;

        AABB aabb;
        aabb.min=hb.position-hb.bounds;
        aabb.max=hb.position+hb.bounds;

        RayHit hit=rayIntersectsAABB(ray,aabb);
        if(hit.hit){
            if((finalHitBox==nullptr)||(hit.distance<dist)){
                dist=hit.distance;
                finalHitBox=&hb;
            }
        }    
    }

    return finalHitBox;
}
void HitboxManager::rightMouseDown(){
    Hitbox* clickedHitbox = getHitboxUnderPixel(Engine::mousePosition);
    if(!clickedHitbox) return;

    //move the clicking functions to function like regular clicks later and not like mousedowns
    if(clickedHitbox->rightClick) clickedHitbox->rightClick();
}
void HitboxManager::leftMouseDown()
{
    Hitbox* clickedHitbox = getHitboxUnderPixel(Engine::mousePosition);
    if(!clickedHitbox) return;

    selectedHitbox=clickedHitbox;
    if(selectedHitbox->mouseDown) selectedHitbox->mouseDown();
}
void HitboxManager::mouseUp(juce::MouseEvent event)
{
    if(!selectedHitbox) return;
    if(selectedHitbox->mouseUp) selectedHitbox->mouseUp();
    selectedHitbox=nullptr;
}
void HitboxManager::dragHitbox(Vec2f delta,Vec2f mousePos)
{
    if(selectedHitbox==nullptr) return;
    if(selectedHitbox->onDrag) selectedHitbox->onDrag(delta,mousePos);
}
void HitboxManager::deleteHitbox(int hitboxID){
    Hitbox& hitbox = hitboxes[hitboxID];
    if(!hitbox.active) return;
    hitbox.active=false;
    deletedHitboxes++;
}

#if draw_hitboxes
void HitboxManager::drawAllHitBoxes(Renderer& renderer)
{
    for(Hitbox& hitbox : hitboxes){
        if(!hitbox.active) continue;
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
