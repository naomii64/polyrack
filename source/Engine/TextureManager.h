#pragma once
#include <list>
#include <vector>
#include "juce_graphics/juce_graphics.h"
#include "Includes/DataTypes.h"


class TextureManager{
    public:
        struct Texture
        {
            std::vector<uint8_t> pixels; // RGBA8
            int width;
            int height;
        };

        TextureManager();
        ~TextureManager();
        int addTexture(juce::Image& image);
        void clearupTextures();
        juce::Image getTextureSheet();

        
        /*
        The uv coordinates of each texture as a vector of Vec4s.
        0 will always be (0,0,1,1) or the whole texture.
        */
        std::vector<Vec4> textureCoords;

    private:
        std::vector<juce::Image> textures;
};