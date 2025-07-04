#pragma once
#include "Includes/Includes.h"
#include "TextManager.h"
#include "../Files/FileManager.h"

namespace EngineAssets{
    //items will be prefixed with m if theyre a model

    extern Model mCablePort;
    extern Model mCableEnd;
    extern Model mTestSquare;
    //textuerIDs
    extern int tBorder;
    extern int tFont;
    extern int tRack;

    void loadModel(Model& model, const juce::OpenGLContext& context, const juce::String& fileName);
    void loadAll(juce::OpenGLContext &openGLContext);
}