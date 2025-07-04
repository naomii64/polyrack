#pragma once
#include <utility>

#include "juce_core/juce_core.h"
#include "juce_graphics/juce_graphics.h"

//not used i dont think
#include "../Engine/TextureManager.h"
#include "../Engine/ModuleManager.h"


class FileManager {
    public:
        static void init();
        static void loadModules(Renderer& renderer, TextureManager &textureAtlas);
        //texture reading
        static juce::Image readTextureFile(juce::File file);
        static juce::Image readTextureStream(std::unique_ptr<juce::InputStream> stream);

        static juce::File appDataFolder;
        static juce::File moduleFolder;
        static juce::File assetFolder;
};