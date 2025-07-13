#include "Engine.h"

OBJ_Scene* Engine::scene=nullptr;
std::vector<std::unique_ptr<Object>> Engine::objects;
std::vector<PhysicsObject*> Engine::physicsObjects;

//engine renderer
Renderer* Engine::renderer=nullptr;

//physics
double Engine::physicsAccumulator=0.0;
double Engine::previousSeconds=0.0;
double Engine::deltaTime=0.0;
void Engine::calculateDeltaTime(){
    //calculate the delta between frames
    double seconds = juce::Time::getMillisecondCounterHiRes() / 1000.0;
    deltaTime=seconds-previousSeconds;
    previousSeconds=seconds;
}
void Engine::physicsTick()
{
    for(PhysicsObject* obj :physicsObjects){
        obj->physicsTick(physicsDelta);
    }
}
void Engine::runPhysics()
{
    Engine::physicsAccumulator+=Engine::deltaTime;
    if(float(Engine::physicsAccumulator)>Engine::physicsDelta){
        Engine::physicsAccumulator=0.0;
        Engine::physicsTick();
    }
}
void Engine::init(){
    //create a scene
    scene = &static_cast<OBJ_Scene&>(*objects.emplace_back(std::make_unique<OBJ_Scene>()));

    //test stuff
    auto& newObj = static_cast<POBJ_Cable&>(*objects.emplace_back(std::make_unique<POBJ_Cable>()));
    physicsObjects.push_back(&newObj);
    int cablePoints=16;
    newObj.desiredDistance=0.5f;
    newObj.createCable(cablePoints,Vec3(0.0f),Vec3(4.0f));

    //attatch the test object to the scene object
    scene->children.push_back(&newObj);
    newObj.parent=scene;
}
void Engine::draw(){
    if(scene==nullptr)return;
    scene->callDraw();
}