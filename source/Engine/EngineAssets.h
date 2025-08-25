#pragma once
#include "TextManager.h"
#include "../Files/FileManager.h"
#include "TextureManager.h"

namespace EngineAssets{
    //items will be prefixed with m if theyre a model

    extern Model mCablePort;
    extern Model mCableEnd;
    extern Model mTestSquare;
    extern Model mCube;

    extern Model mWireCube;
    extern Model mAxis;

    extern Model mCableModel;
    extern Model mBorderRectModel;
    //textuerIDs
    extern int tBorder;
    extern int tBorderHovered;
    extern int tFont;
    extern int tFontHovered;
    extern int tRack;
    extern int tBlank;
    extern int tCable;

    extern int tAxis;

    void loadModel(Model& model, const juce::String& fileName);
    void loadModels();
    void loadTexture(TextureManager& atlas ,int& texture, const juce::String& fileName);
    void loadTextures(TextureManager& atlas);
    
    void createCableModel(juce::OpenGLContext &openGLContext);
    void createBorderRectModel(juce::OpenGLContext &openGLContext);
}

enum NineSlice {
    TOP_LEFT = 0,
    TOP_MIDDLE = 1,
    TOP_RIGHT = 2,
    
    MIDDLE_LEFT = 3,
    MIDDLE_MIDDLE = 4,
    MIDDLE_RIGHT = 5,
    
    BOTTOM_LEFT = 6,
    BOTTOM_MIDDLE = 7,
    BOTTOM_RIGHT = 8
};