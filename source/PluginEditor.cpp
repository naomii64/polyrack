#pragma once
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Engine/Scene.h"

#include "Engine/Hitboxes.h"


//for debug
#define DEBUG_CONSOLE_ENABLED true

#include <iostream>
#if DEBUG_CONSOLE_ENABLED
#include "DebugConsole.h"
#endif

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
    setSize(900, 900);

    addAndMakeVisible(mainRenderer);

    //dont want to draw the base ui thing
    mainUI.drawSelf=false;

    //create ui stuff, will eventually make functions for this
    UIObject& moduleListUI = UIManager::createChild(mainUI,UIObject());
    moduleListUIOBJ = moduleListUI.ID;
    moduleListUI.layout=LIST_VERTICAL;

    resizeUI();
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor(){}
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g){}

void AudioPluginAudioProcessorEditor::resized()
{
    mainRenderer.setSize(getWidth(),getHeight());
    resizeUI();
}
void AudioPluginAudioProcessorEditor::resizeUI()
{
    if(mainUI.children.size()<=0) return;
    
    constexpr float padding = 10.0f;
    constexpr float padding2=padding*2.0f;

    UIObject& moduleListUI = UIManager::getObj(moduleListUIOBJ);
    moduleListUI.relativePosition = {padding,padding};
    moduleListUI.relativeSize = {float(getWidth())-padding2,float(getHeight())-padding2};
}
//PROBABLY MOVE THIS LATER
void AudioPluginAudioProcessorEditor::createModuleListUI()
{
    constexpr float listButtonSize = 40.0f;

    UIManager::objects.reserve(ModuleManager::modules.size());      //prevent the objects from resizing and losing reference
    UIObject& moduleListUI = UIManager::getObj(moduleListUIOBJ);

    for(ModuleData& module : ModuleManager::modules){
        //REPLACE THIS WITH A BUTTON LATER
        UIObject& newOBJ = UIManager::createChild(moduleListUI,UIObject());
        newOBJ.relativeSize.y = listButtonSize;
        newOBJ.setText(module.name);
        UIManager::makeInteractable(newOBJ);
    }
}

void AudioPluginAudioProcessorEditor::onRendererLoad(){
    
    EngineAssets::loadModels();
    EngineAssets::loadTextures(textureAtlas);


    FileManager::loadModules(mainRenderer,textureAtlas);

    UIManager::styleUI();
    //finish the the
    mainRenderer.mainTextureImage = textureAtlas.getTextureSheet();
    mainRenderer.mainTextureAtlas = &textureAtlas;

    mainRenderer.setCameraPosition({0.0f,0.0f,10.0f});

    createModuleListUI();

    #if DEBUG_CONSOLE_ENABLED
    Debugger::start();
    #endif
}

//========INPUT========//
void AudioPluginAudioProcessorEditor::mouseMove(const juce::MouseEvent& event){updateMouse(event);}
void AudioPluginAudioProcessorEditor::mouseDrag(const juce::MouseEvent& event){updateMouse(event);}
void AudioPluginAudioProcessorEditor::updateMouse(const juce::MouseEvent& event){
    Vec2 position = Vec2(event.position.x, event.position.y);
    Scene::mousePos=position;
    
    UIManager::hover(position);

    HitboxManager::dragHitbox(position - previousMousePos,position);

    previousMousePos = position;
}

void AudioPluginAudioProcessorEditor::mouseWheelMove(const juce::MouseEvent &event, const juce::MouseWheelDetails &wheel)
{
    float zoomSpeed=1.0f;
    //get the sign
    float sign=1.0f;
    if(wheel.deltaY<0){
        sign=-1.0f;
    }
    Scene::zoom-=sign*zoomSpeed;
}

void AudioPluginAudioProcessorEditor::mouseDown(const juce::MouseEvent& event) {
    if (event.mods.isRightButtonDown()) {
        // Right mouse button was clicked
        //TODO: redo ui
        //UIManager::showGroup(UIManager::moduleSelectionGroup);
    }
    if (event.mods.isLeftButtonDown()) {
        // Right mouse button was clicked
        //UIManager::click(event);
        HitboxManager::click(mainRenderer,event);
    }
}

void AudioPluginAudioProcessorEditor::mouseUp(const juce::MouseEvent &event)
{
    HitboxManager::mouseUp(event);
}
