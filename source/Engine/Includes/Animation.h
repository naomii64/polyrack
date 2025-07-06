#pragma once
#include <vector>
#include "Model.h"
#include "../Renderer.h"
#include "DataTypes.h"
class Renderer;
struct Animation{
    std::vector<Model*> models;
    Vec3 hitboxSize={1.0f,1.0f,1.0f};
    void draw(Renderer& renderer,Vec3 position={0.0f,0.0f,0.0f},Vec3 rotation={0.0f,0.0f,0.0f},Vec3 scale={1.0f,1.0f,1.0f}){
        for(Model* model : models){
            renderer.drawModelAt(*model,position,rotation,scale,model->textureID);
        }
    }
};