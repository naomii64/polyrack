#include "Engine.h"

OBJ_Scene* Engine::scene=nullptr;
std::vector<std::unique_ptr<Object>> Engine::objects;
std::vector<PhysicsObject*> Engine::physicsObjects;
std::vector<OBJ_Comp_Socket*> Engine::sockets;
size_t Engine::nextTickedPhysicsObject = 0;

Vec2f Engine::mousePosition;
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
void Engine::runPhysics()
{
    if (physicsObjects.empty()) return;
    const float percentageOfPhysicsTick = float(Engine::deltaTime)/Engine::physicsDelta;
    size_t objectsToTick = std::trunc(float(Engine::physicsObjects.size())*percentageOfPhysicsTick);
    objectsToTick=std::min(objectsToTick,Engine::physicsObjects.size());

    for(size_t i=0;i<objectsToTick;i++){
        physicsObjects[Engine::nextTickedPhysicsObject]->physicsTick(Engine::physicsDelta);
        Engine::nextTickedPhysicsObject++;
        Engine::nextTickedPhysicsObject%=Engine::physicsObjects.size();
    }
}
void Engine::init(){
    //create a scene
    scene = &Engine::createObject<OBJ_Scene>();

    //test stuff
    //create a bunch of cables
    for(int i=0;i<50;i++){
        auto& newObj = Engine::createObject<POBJ_Cable>();
        physicsObjects.push_back(&newObj);
        int cablePoints=16;
        newObj.desiredDistance=1.0f;
        newObj.createCable(cablePoints,Vec3f(0.0f),Vec3f(1.0f));
        
        newObj.points[0].position={float(i),0.0f,0.0f};

        //attatch the test object to the scene object
        scene->addChild(&newObj);
        newObj.parent=scene;
    }
}
void Engine::draw(){
    if(scene==nullptr)return;
    scene->callDraw();

    POBJ_Cable::drawAll();
}

Vec3f Engine::screenPosToZPlane(Vec2f screenPos,float zValue)
{
        //THIS COULD PROBABLY BE MADE FASTER

        //calculate where it intersects the given z value
        Ray ray = Engine::renderer->rayFrom(screenPos);        
        Vec3f targetPosition = ray.direction;
        
        //find the difference
        float difference = zValue-ray.origin.z;
        
        targetPosition/=targetPosition.z;
        targetPosition*=difference;

        return ray.origin+targetPosition;
}
void Engine::updateDeletedObjects(){
    sockets.erase(
    std::remove_if(sockets.begin(), sockets.end(),
                   [](OBJ_Comp_Socket* socket) { return socket->deleted; }),
    sockets.end());
}