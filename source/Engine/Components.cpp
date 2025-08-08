#include "Components.h"
#include "EngineAssets.h"
//init=====;
void Component::init(const juce::var &obj, ModuleData &moduleData){}

void Component::initInstance(){
    std::cout << "new component instance created"<<std::endl;
}

void StaticMeshComponent::init(const juce::var &obj, ModuleData &moduleData)
{
    int modelID = obj.getProperty("modelID",0);
    model = &moduleData.models[modelID];
}
void SocketComponent::init(const juce::var &obj, ModuleData &moduleData)
{
    bool isInputBool = obj.getProperty("isInput",false);
    isInput=isInputBool;
}
void InputComponent::init(const juce::var &obj, ModuleData &moduleData)
{
    int animationID = obj.getProperty("animationID",0);
    animation = &moduleData.animations[animationID];
    hitboxSize=animation->hitboxSize;
}
//INIT INSTANCE=====;
void InputComponent::initInstance()
{
    hitbox=&HitboxManager::createHitbox();
    hitbox->onDrag=[this](Vec2 delta,Vec2 mousePos){
        //CONSTANT
        float sensitivity=0.005f;
        this->value+=delta*Vec2(1.0f,-1.0f)*sensitivity;
        this->value.x=std::max(this->value.x,0.0f);
        this->value.y=std::max(this->value.y,0.0f);
        this->value.x=std::min(this->value.x,1.0f);
        this->value.y=std::min(this->value.y,1.0f);
    };
    hitbox->bounds=hitboxSize;
    hitbox->position=position;
}


//DRAW=====;
void Component::draw(Renderer& renderer) const{}
void StaticMeshComponent::draw(Renderer &renderer) const
{
    if(model==nullptr){return;}
    renderer.drawModelAt(*model,position,rotation,scale,model->textureID);
}
void SocketComponent::draw(Renderer &renderer) const
{
    renderer.drawModelAt(EngineAssets::mCablePort,position,rotation,scale,EngineAssets::tRack);
}
void InputComponent::draw(Renderer &renderer) const
{
    if(animation==nullptr){return;}
    animation->draw(value,renderer,position);
}