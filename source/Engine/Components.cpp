#include "Components.h"
#include "EngineAssets.h"
/*
Component::Component(){};
Component::~Component(){};
StaticMeshComponent::StaticMeshComponent(){}
StaticMeshComponent::~StaticMeshComponent(){};
*/
void Component::draw(Renderer& renderer) const{
    //std::cout<<"dra3333wing comp\n";
    //renderer.drawModelAt(EngineAssets::mTestSquare,Vec3(position.x,position.y,-5.0f));
}
void Component::init(const juce::var &obj, ModuleData &moduleData){}
void StaticMeshComponent::draw(Renderer &renderer) const
{
    if(model==nullptr){return;}
    renderer.drawModelAt(*model,Vec3(position.x,position.y,-7.0f),rotation,scale,model->textureID);
}
void StaticMeshComponent::init(const juce::var &obj, ModuleData &moduleData)
{
    int modelID = obj.getProperty("modelID",0);
    model = &moduleData.models[modelID];
}
void SocketComponent::draw(Renderer &renderer) const
{
    renderer.drawModelAt(EngineAssets::mCablePort,Vec3(position.x,position.y,-7.0f),rotation,scale,EngineAssets::tRack);
}