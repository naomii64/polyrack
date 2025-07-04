#pragma once
#include "TextureManager.h"
#include <algorithm>

#define DEBUG_MODE true

TextureManager::TextureManager(){}
TextureManager::~TextureManager(){}

int TextureManager::addTexture(juce::Image& image){
    textures.emplace_back(image);
    return textures.size();
}
juce::Image TextureManager::getTextureSheet() {
    #if DEBUG_MODE
        #define LINE "========================"

        std::cout << std::endl << LINE << "TextureManager::getTextureSheet" << LINE << std::endl;
        #undef LINE
    #endif

    int atlasWidth = 0;
    int atlasHeight = 0;

    //calculate the size of the strip
    for(juce::Image texture:textures){
        atlasWidth=std::max(atlasWidth,texture.getWidth());
        atlasHeight+=texture.getHeight();
    }

    juce::Image atlas(juce::Image::ARGB,atlasWidth,atlasHeight,true);
    juce::Graphics g(atlas);

    //draw the images
    int currentY=0;

    //calculate the scale factors
    const float scaleFactorX=1.0f/atlas.getWidth();
    const float scaleFactorY=1.0f/atlas.getHeight();
    const Vec4 scaleVector(scaleFactorX,scaleFactorY,scaleFactorX,scaleFactorY);

    #if DEBUG_MODE
        std::cout << "Generating texture atlas for " << textures.size() << " textures..." << std::endl;
        std::cout << "uvs:" << std::endl;
    #endif

    //reset the texture coord vector
    textureCoords.clear();
    //reserve a uv for each texture plus the default texture
    textureCoords.reserve(textures.size()+1);
    //add default texture that covers the whole sheet
    textureCoords.emplace_back(Vec4(0.0f,0.0f,1.0f,1.0f));
    
    for(juce::Image texture:textures){
        const Vec4 texturePosAndSize(
            0.0f,
            float(currentY),
            float(texture.getWidth()),
            float(texture.getHeight())
        );
        //draw the image to the texture
        g.drawImage(texture,juce::Rectangle<float>(
            texturePosAndSize.x,
            texturePosAndSize.y,
            texturePosAndSize.z,
            texturePosAndSize.w
        ));
        Vec4 newUV;
        newUV = texturePosAndSize*scaleVector;
        //add the uvs, scaled to 0 to 1
        textureCoords.emplace_back(newUV);
        currentY+=texture.getHeight();

        #if DEBUG_MODE
            std::cout <<"\t"<< newUV.toString() << std::endl;
        #endif
    }
    #if DEBUG_MODE
        std::cout << textureCoords.size() << " uvs were added" << std::endl;
    #endif
    
    return atlas;
}

/*
    Resets the texture manager by deleting all its contained images and uvs.
    For clearing up memory.
*/
void TextureManager::clearupTextures(){
    textures.clear();
    textureCoords.clear();
}