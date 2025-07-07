#pragma once
#include <vector>
#include "Model.h"
#include "../Renderer.h"
#include "DataTypes.h"
#include "../../lib/exprtk.hpp"
class Renderer;


struct AnimFunctionCollection{
    std::string x;
    std::string y;
    std::string z;
    std::string rx;
    std::string ry;
    std::string rz;
    std::string sx;
    std::string sy;
    std::string sz;

    exprtk::symbol_table<float> symbolTable;
    exprtk::parser<float> parser;

    exprtk::expression<float> exp_x;
    exprtk::expression<float> exp_y;
    exprtk::expression<float> exp_z;    
    exprtk::expression<float> exp_rx;
    exprtk::expression<float> exp_ry;
    exprtk::expression<float> exp_rz;
    exprtk::expression<float> exp_sx;
    exprtk::expression<float> exp_sy;
    exprtk::expression<float> exp_sz;
};
struct Animation{
    std::vector<Model*> models;
    std::vector<AnimFunctionCollection> animFunctions;
    Vec3 hitboxSize={1.0f,1.0f,1.0f};
    void draw(Renderer& renderer,Vec3 position={0.0f,0.0f,0.0f},Vec3 rotation={0.0f,0.0f,0.0f},Vec3 scale={1.0f,1.0f,1.0f}){
        for(Model* model : models){
            renderer.drawModelAt(*model,position,rotation,scale,model->textureID);
        }
    }
};