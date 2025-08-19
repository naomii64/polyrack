#pragma once
#include "EngineAssets.h"
#include "Engine.h"

namespace EngineAssets{
    Model mCablePort;
    Model mCableEnd;
    Model mTestSquare;
    Model mCube;
    Model mWireCube;
    Model mAxis;
    Model mCableModel;
    Model mBorderRectModel;
    //texture IDS
    int tBorder;
    int tBorderHovered;
    int tFont;
    int tFontHovered;
    
    int tRack;
    int tBlank;
    int tCable;

    int tAxis;

    void loadModel(Model& model, const juce::String& fileName) {
        auto file = FileManager::assetFolder.getChildFile(fileName);
        auto objStr = file.loadFileAsString().toStdString();
        auto parsed = parseOBJString(objStr);

        model.createGeometry(Engine::renderer->openGLContext, parsed);
    }
    void loadModels() {
        loadModel(mCablePort,"cable_socket.obj");
        loadModel(mCableEnd,"cable_end.obj");
        
        //primatives
        loadModel(mCube,"cube.obj");
        loadModel(mTestSquare,"test_square.obj");

        //debugging stuff
        loadModel(mWireCube,"wireFrameCube.obj");
        loadModel(mAxis,"axis.obj");

        createCableModel(Engine::renderer->openGLContext);
        createBorderRectModel(Engine::renderer->openGLContext);
    }
    void loadTexture(TextureManager& atlas ,int& texture, const juce::String& fileName){
        texture = atlas.addTexture(FileManager::readTextureFile(FileManager::assetFolder.getChildFile(fileName)));
    }
    void loadTextures(TextureManager& atlas){
        loadTexture(atlas,tRack,"rack.png");
        loadTexture(atlas,tCable,"Cable.png");

        loadTexture(atlas,tFont,"font_shaded.png");
        loadTexture(atlas,tFontHovered,"font_shaded_inverse.png");
        loadTexture(atlas,tBorder,"ui_border_fancy.png");
        loadTexture(atlas,tBorderHovered,"ui_border_fancy_inverse.png");

        loadTexture(atlas,tBlank,"blank.png");
        loadTexture(atlas,tAxis,"axis.png");    
    }
    void createBorderRectModel(juce::OpenGLContext &openGLContext){
        std::vector<Vertex> vertices;
        std::vector<uint8_t> groups;
        //create the 9 slices over eachother
        for(int i=0;i<9;i++){
            #define POS1 {0.0f,-1.0f,0.0f}
            #define POS2 {1.0f,-1.0f,0.0f}
            #define POS3 {1.0f,0.0f,0.0f}
            #define POS4 {0.0f,0.0f,0.0f}

            Vec2 UVA(0.0f);
            Vec2 UVB(1.0f);
            
            Vec2 uvOffset(0.0f);

            //UVS ARE ALSO WEIRD ABOUT WHATS UPSIDE DOWN OMG

            switch (i)
            {
                case NineSlice::BOTTOM_LEFT:
                    uvOffset=Vec2(0.0f);
                    break;
                case NineSlice::BOTTOM_MIDDLE:
                    uvOffset=Vec2(1.0f,0.0f);
                    break;
                case NineSlice::BOTTOM_RIGHT:
                    uvOffset=Vec2(2.0f,0.0f);
                    break;
                case NineSlice::MIDDLE_LEFT:
                    uvOffset=Vec2(0.0f,1.0f);
                    break;
                case NineSlice::MIDDLE_MIDDLE:
                    uvOffset=Vec2(1.0f,1.0f);
                    break;
                case NineSlice::MIDDLE_RIGHT:
                    uvOffset=Vec2(2.0f,1.0f);
                    break;
                    
                case NineSlice::TOP_LEFT:
                    uvOffset=Vec2(0.0f,2.0f);
                    break;
                case NineSlice::TOP_MIDDLE:
                    uvOffset=Vec2(1.0f,2.0f);
                    break;
                case NineSlice::TOP_RIGHT:
                    uvOffset=Vec2(2.0f,2.0f);
                    break;                
                    
                default:
                    break;
            }
            UVA+=uvOffset;
            UVB+=uvOffset;
            UVA/=3.0f;
            UVB/=3.0f;

            #define UV1 {UVA.x,UVA.y}
            #define UV2 {UVB.x,UVA.y}
            #define UV3 {UVB.x,UVB.y}
            #define UV4 {UVA.x,UVB.y}

            #define NORMAL {1.0f,1.0f,1.0f}
            #define COLOR {1.0f,1.0f,1.0f,1.0f}
            
            const Vertex newVertices[] = {
                {POS1,NORMAL,COLOR,UV1},
                {POS2,NORMAL,COLOR,UV2},
                {POS3,NORMAL,COLOR,UV3},
                {POS4,NORMAL,COLOR,UV4}
            };

            vertices.push_back(newVertices[0]);
            vertices.push_back(newVertices[1]);
            vertices.push_back(newVertices[3]);

            vertices.push_back(newVertices[3]);
            vertices.push_back(newVertices[1]);
            vertices.push_back(newVertices[2]);

            for(int group=0;group<6;group++) groups.push_back(i+1);

            #undef POS2
            #undef POS1
            #undef POS3
            #undef POS4

            #undef UV1
            #undef UV2
            #undef UV3
            #undef UV4

            #undef NORMAL
            #undef COLOR
        }

        mBorderRectModel.createGeometry(openGLContext,vertices,groups);
    }
    void createCableModel(juce::OpenGLContext &openGLContext)
    {
        //generate the model for cables
        std::vector<Vertex> cableVertices;
        std::vector<uint8_t> cableGroups;

        //cable settings
        constexpr int cableModelGroups=16;
        constexpr int cableResolution = 6; //cables will be hexagons
        constexpr float cableRadius = 0.15f;

        //amount to spread uvs into: 3 would mean each face has a third of the uv
        constexpr int uvSpread = 2;
        constexpr float uvSpreadWidth = 1.0f/float(uvSpread);

        int uvSpreadIndex = 0;
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

                Vec4 uv;
                float uvStart = float(uvSpreadIndex)*uvSpreadWidth;
                uv = {uvStart,0.0f,uvStart+uvSpreadWidth,1.0f};

                int group_UL = i;
                Vertex Vert_UL = {
                    POS1,
                    POS1,
                    COLOR,
                    {uv.x,uv.y}
                };
                int group_UR = i;
                Vertex Vert_UR = {
                    POS2,
                    POS2,
                    COLOR,
                    {uv.z,uv.y}
                };
                int group_DR = i+1;
                Vertex Vert_DR = {
                    POS2,
                    POS2,
                    COLOR,
                    {uv.z,uv.w}
                };
                int group_DL = i+1;
                Vertex Vert_DL = {
                    POS1,
                    POS1,
                    COLOR,
                    {uv.x,uv.w}
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
                
                uvSpreadIndex++;
                if(uvSpreadIndex>=uvSpread){
                    uvSpreadIndex=0;
                }
            }
            #undef COLOR
            #undef POS1
            #undef POS2
        }

        mCableModel.createGeometry(openGLContext,cableVertices,cableGroups);
    }
}