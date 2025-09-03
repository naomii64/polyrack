#include "Object.h"
#include "../ModuleManager.h"
#include "../Hitboxes.h"
#include "../Engine.h"
#include "../../UI/UI.h"
#include "../EngineAssets.h"

void OBJ_Module::createFrom(ModuleData *moduleData)
{
    width = moduleData->width;
    height = moduleData->height;

    mainHitboxID=HitboxManager::createHitboxID();
    auto& hitbox = HitboxManager::hitboxes[mainHitboxID];
    hitbox.bounds=Vec3f(float(width),float(height),1.0f)*0.5f;
    Hitbox* hbptr=&hitbox;
    hitbox.onDrag=[this,hbptr](Vec2f delta, Vec2f mousePos){
        Vec2f prevPos = mousePos-delta;

        const float zValue = 0.0f;

        Vec3f pointA = Engine::screenPosToZPlane(prevPos,zValue);
        Vec3f pointB = Engine::screenPosToZPlane(mousePos,zValue);

        Vec3f delta3D = pointB-pointA;
        this->truePosition+=delta3D;
        this->setPosition(Vec3f::round(this->truePosition));
        
        //std::cout<<"dragging: "<< delta3D.toString()<<"\n";
    };
    hitbox.rightClick = [this]{
        UI::openModuleContext(this);        
    };

    //TODO: FIX THIS LATERRRR
    for (const juce::var& obj : moduleData->layout){
        juce::String componentType = obj.getProperty("type", "[NONE]");
        //std::cout<<componentType<<"\n";

        OBJ_Component* component;
        //MOVE ALL THIS STUFF TO FUNCTIONS LATER
        if (componentType == "static mesh") {
            component = &Engine::createObject<OBJ_Comp_Mesh>();
        } else if (componentType == "socket") {
            component = &Engine::createObject<OBJ_Comp_Socket>();
        } else if (componentType == "input") {
            component = &Engine::createObject<OBJ_Comp_Input>();
        }else{
            continue;
        }

        //read the values like position and stuff
        Vec3f vPosition = readVec3FromObj(obj["position"]);
        Vec3f vRotation = readVec3FromObj(obj["rotation"]);
        Vec3f vScale = readVec3FromObj(obj["scale"], Vec3f(1.0f));

        component->setTransform(vPosition,vRotation,vScale);
        component->initComponent(obj,*moduleData);

        addChild(component);
    }
}

void OBJ_Module::deleteObj()
{
    //dettatch all cables
    for (Object* child : children) {
            child->deleteObj();
    }
    HitboxManager::deleteHitbox(mainHitboxID);
    deleted=true;
}
void OBJ_Comp_Socket::deleteObj()
{
    //detatch any connected cables
    if(connectedPoint){
        //might need to change where this is idk
        connectedPoint->lockRotation=false;
        connectedPoint->fixed=false;
        connectedPoint->detachFromSocket();
    }
    deleted=true;
}
void OBJ_Comp_Input::deleteObj()
{
    HitboxManager::deleteHitbox(hitboxID);
    deleted=true;
}

void OBJ_Module::onTransformChange()
{
    //update the hitbox position
    auto& hitbox = HitboxManager::hitboxes[mainHitboxID];
    hitbox.position=transform.position-(hitbox.bounds*Vec3f{-1.0f,-1.0f,1.0f});
}

void OBJ_Comp_Mesh::onDraw()
{
    if (!model) return;
    Engine::renderer->drawModelWithMatrix(*model,matrix,normalMatrix,model->textureID);
}
void OBJ_Comp_Socket::onDraw()
{
    Engine::renderer->drawModelWithMatrix(EngineAssets::mCablePort,matrix,normalMatrix,EngineAssets::tRack);
}

void OBJ_Comp_Input::onDraw()
{
    if(!animation) return;
    animation->draw(values,*Engine::renderer,matrix);
}
void OBJ_Comp_Input::onTransformChange()
{
    auto& hitbox = HitboxManager::hitboxes[hitboxID];
    hitbox.position=worldTranslation;
}
void OBJ_Comp_Socket::onTransformChange()
{
    if(!connectedPoint) return;
    connectedPoint->connectToSocket();
}

void OBJ_Component::initComponent(const juce::var &fromObject,ModuleData& moduleData){}
void OBJ_Comp_Input::initComponent(const juce::var &fromObject,ModuleData& moduleData){
    int animationID = fromObject.getProperty("animationID",0);
    animation=&moduleData.animations[animationID];
            
    hitboxID=HitboxManager::createHitboxID();
    Hitbox& hitbox = HitboxManager::hitboxes[hitboxID];
    hitbox.bounds=animation->hitboxSize;

    hitbox.onDrag = [this](Vec2f delta,Vec2f mousePos){
        const float sensitivity=0.005f;
        this->values+=delta*Vec2f(1.0f,-1.0f)*sensitivity;
        this->values.x=std::clamp(this->values.x,0.0f,1.0f);
        this->values.y=std::clamp(this->values.y,0.0f,1.0f);
    };
}
void OBJ_Comp_Socket::initComponent(const juce::var &fromObject,ModuleData& moduleData){
    Engine::sockets.push_back(this);
}
void OBJ_Comp_Mesh::initComponent(const juce::var &fromObject,ModuleData& moduleData){
    int modelID = fromObject.getProperty("modelID",0);
    model = &moduleData.models[modelID];
}