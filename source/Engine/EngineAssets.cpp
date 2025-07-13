#pragma once
#include "EngineAssets.h"

namespace EngineAssets{
    Model mCablePort;
    Model mCableEnd;
    Model mTestSquare;
    Model mCube;
    Model mWireCube;
    Model mAxis;
    Model mCableModel;
    //texture IDS
    int tBorder;
    int tFont;
    int tRack;
    int tBlank;
    int tCable;

    int tAxis;

    void loadModel(Model& model, juce::OpenGLContext& context, const juce::String& fileName) {
        auto file = FileManager::assetFolder.getChildFile(fileName);
        auto objStr = file.loadFileAsString().toStdString();
        auto parsed = parseOBJString(objStr);

        model.createGeometry(context, parsed);
    }
    void loadAll(juce::OpenGLContext &openGLContext) {
        loadModel(mCablePort,openGLContext,"cable_socket.obj");
        loadModel(mCableEnd,openGLContext,"cable_end.obj");
        
        //primatives
        loadModel(mCube,openGLContext,"cube.obj");
        loadModel(mTestSquare,openGLContext,"test_square.obj");

        //debugging stuff
        loadModel(mWireCube,openGLContext,"wireFrameCube.obj");
        loadModel(mAxis,openGLContext,"axis.obj");

        //generate the model for cables
        std::vector<Vertex> cableVertices;
        std::vector<uint8_t> cableGroups;

        //cable settings
        const int cableModelGroups=16;
        const int cableResolution = 6; //cables will be hexagons
        const float cableRadius = 0.25f;

        for(uint8_t i=1;i<cableModelGroups;i++){
            for(int r=0;r<cableResolution;r++){
                float rPercent = float(r)/(float(cableResolution));
                float rRad = rPercent*2*juce::MathConstants<float>::pi;

                float nextrPercent = float(r+1)/(float(cableResolution));
                float nextrRad = nextrPercent*2*juce::MathConstants<float>::pi;

                float x=std::cos(rRad)*cableRadius;
                float y=std::sin(rRad)*cableRadius;
                float nextx=std::cos(nextrRad)*cableRadius;
                float nexty=std::sin(nextrRad)*cableRadius;
                
                Vec3 normal1(1.0f,1.0f,1.0f);

                #define COLOR {1.0f,1.0f,1.0f,1.0f}
                //positions. pos and normal are the same since its a cylindar
                #define POS1 {x,y,0.0f}
                #define POS2 {nextx,nexty,0.0f}

                int group_UL = i;
                Vertex Vert_UL = {
                    POS1,
                    POS1,
                    COLOR,
                    {0.0f,0.0f}
                };
                int group_UR = i;
                Vertex Vert_UR = {
                    POS2,
                    POS2,
                    COLOR,
                    {1.0f,0.0f}
                };
                int group_DR = i+1;
                Vertex Vert_DR = {
                    POS2,
                    POS2,
                    COLOR,
                    {1.0f,1.0f}
                };
                int group_DL = i+1;
                Vertex Vert_DL = {
                    POS1,
                    POS1,
                    COLOR,
                    {0.0f,1.0f}
                };
                //add the defined vertices to the vector
                cableVertices.push_back(Vert_UL);
                cableGroups.push_back(group_UL);

                cableVertices.push_back(Vert_UR);
                cableGroups.push_back(group_UR);

                cableVertices.push_back(Vert_DR);
                cableGroups.push_back(group_DR);

                cableVertices.push_back(Vert_UL);
                cableGroups.push_back(group_UL);

                cableVertices.push_back(Vert_DR);
                cableGroups.push_back(group_DR);   

                cableVertices.push_back(Vert_DL);
                cableGroups.push_back(group_DL);
             
            }
            #undef COLOR
            #undef POS1
            #undef POS2
        }

        mCableModel.createGeometry(openGLContext,cableVertices,cableGroups);
    }
}