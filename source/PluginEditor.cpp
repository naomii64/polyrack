#pragma once
#include "PluginProcessor.h"
#include "PluginEditor.h"

//for debug
#include <iostream>

void printImagePixels(const juce::Image& image, int count = 10)
{
    if (!image.isValid())
    {
        std::cout << "Invalid image!" << std::endl;
        return;
    }

    int width = image.getWidth();
    int height = image.getHeight();

    int printed = 0;
    for (int y = 0; y < height && printed < count; ++y)
    {
        for (int x = 0; x < width && printed < count; ++x)
        {
            juce::Colour c = image.getPixelAt(x, y);
            std::cout << "Pixel (" << x << ", " << y << "): "
                      << "R=" << static_cast<int>(c.getRed()) << " "
                      << "G=" << static_cast<int>(c.getGreen()) << " "
                      << "B=" << static_cast<int>(c.getBlue()) << " "
                      << "A=" << static_cast<int>(c.getAlpha()) << std::endl;
            ++printed;
        }
    }
}

AudioPluginAudioProcessorEditor* AudioPluginAudioProcessorEditor::mainProcessEditor;
//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);
    mainProcessEditor=this;

    //initialize file system
    FileManager::init();

    //mouse tracking
    mainRenderer.addMouseListener(this, true); 
    setResizable(true,true);
    setSize (900, 900);

    addAndMakeVisible(mainRenderer);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor(){}
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g){}

void AudioPluginAudioProcessorEditor::resized()
{
    mainRenderer.setSize(getWidth(),getHeight());
}
void AudioPluginAudioProcessorEditor::onRendererLoad(){
    std::cout << "Rendererer started!\n";
    //load textures
    EngineAssets::tRack = textureAtlas.addTexture(FileManager::readTextureFile(FileManager::assetFolder.getChildFile("rack.png")));
    textureAtlas.addTexture(FileManager::readTextureFile(FileManager::assetFolder.getChildFile("cable.png")));
    EngineAssets::tFont = textureAtlas.addTexture(FileManager::readTextureFile(FileManager::assetFolder.getChildFile("font_shaded.png")));

    //ui textures
    textureAtlas.addTexture(FileManager::readTextureFile(FileManager::assetFolder.getChildFile("ui_border_basic.png")));
    EngineAssets::tBorder = textureAtlas.addTexture(FileManager::readTextureFile(FileManager::assetFolder.getChildFile("ui_border_metal.png")));
    
    FileManager::loadModules(mainRenderer,textureAtlas);
    //finish the the
    mainRenderer.mainTextureImage = textureAtlas.getTextureSheet();
    mainRenderer.mainTextureAtlas = &textureAtlas;
}

//========INPUT========//
void AudioPluginAudioProcessorEditor::mouseMove(const juce::MouseEvent& event)
{
    Vec2 position = Vec2(event.position.x,event.position.y);
    UIManager::updateMousePos(mainRenderer,position);
}
void AudioPluginAudioProcessorEditor::mouseDown(const juce::MouseEvent& event) {
    if (event.mods.isRightButtonDown()) {
        // Right mouse button was clicked
        UIManager::showGroup(UIManager::moduleSelectionGroup);
    }
    if (event.mods.isLeftButtonDown()) {
        // Right mouse button was clicked
        UIManager::click(event);
    }
}