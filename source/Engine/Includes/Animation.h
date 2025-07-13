#pragma once
#include <vector>
#include "Model.h"
#include "../Renderer.h"
#include "DataTypes.h"

//disable type conversion warning
#pragma warning(push)
#pragma warning(disable : 4244)
#include "../../lib/exprtk.hpp"
#pragma warning(pop)

class Renderer;


struct AnimFunctionCollection{
    float y_value;
    float x_value;

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

    void compileExpressions() {
        symbolTable.add_variable("x", x_value);
        symbolTable.add_variable("y", y_value);
        symbolTable.add_constants();

        // Bind symbol table to all expressions
        exp_x.register_symbol_table(symbolTable);
        exp_y.register_symbol_table(symbolTable);
        exp_z.register_symbol_table(symbolTable);
        exp_rx.register_symbol_table(symbolTable);
        exp_ry.register_symbol_table(symbolTable);
        exp_rz.register_symbol_table(symbolTable);
        exp_sx.register_symbol_table(symbolTable);
        exp_sy.register_symbol_table(symbolTable);
        exp_sz.register_symbol_table(symbolTable);

        // Compile all expressions
        parser.compile(x, exp_x);
        parser.compile(y, exp_y);
        parser.compile(z, exp_z);
        parser.compile(rx, exp_rx);
        parser.compile(ry, exp_ry);
        parser.compile(rz, exp_rz);
        parser.compile(sx, exp_sx);
        parser.compile(sy, exp_sy);
        parser.compile(sz, exp_sz);
        
        //std::cout<<"expressions compiled!\n";
    }
};
struct Animation{
    std::vector<Model*> models;
    std::vector<std::unique_ptr<AnimFunctionCollection>> animFunctions;
    Vec3 hitboxSize={1.0f,1.0f,1.0f};
    
    void draw(const Vec2& values,Renderer& renderer,Vec3 position={0.0f,0.0f,0.0f},Vec3 scale={1.0f,1.0f,1.0f}){
        for(int i=0;i<models.size();i++){
            std::unique_ptr<AnimFunctionCollection>& funcs = animFunctions[i];
            Model* model = models[i];
            funcs->x_value=values.x;
            funcs->y_value=values.y;
            

            Vec3 animatedRotation;
            animatedRotation.x=funcs->exp_rx.value();
            animatedRotation.y=funcs->exp_ry.value();
            animatedRotation.z=funcs->exp_rz.value();

            animatedRotation*=-juce::MathConstants<float>::pi/180.0f;
            renderer.drawModelAt(*model,position,animatedRotation,scale,model->textureID);
        }
    }
};