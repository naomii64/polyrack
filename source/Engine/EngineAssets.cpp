#pragma once
#include "EngineAssets.h"

namespace EngineAssets{
    Model mCablePort;
    Model mCableEnd;
    Model mTestSquare;
    //texture IDS
    int tBorder;
    int tFont;
    int tRack;

    void loadModel(Model& model, juce::OpenGLContext& context, const juce::String& fileName) {
        auto file = FileManager::assetFolder.getChildFile(fileName);
        auto objStr = file.loadFileAsString().toStdString();
        auto parsed = parseOBJString(objStr);

        model.createGeometry(context, parsed);
    }
    void loadAll(juce::OpenGLContext &openGLContext) {
        loadModel(mCablePort,openGLContext,"cable_socket.obj");
        loadModel(mCableEnd,openGLContext,"cable_end.obj");
        loadModel(mTestSquare,openGLContext,"test_square.obj");
    }
}