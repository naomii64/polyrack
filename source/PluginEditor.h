#pragma once

#include "PluginProcessor.h"
#include "Engine/Renderer.h"
#include "Engine/EngineAssets.h"
#include "Files/FileManager.h"
#include "UI/UI.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void mouseMove(const juce::MouseEvent& event) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void updateMouse(const juce::MouseEvent& event);

    void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;

    void onRendererLoad();

    TextureManager textureAtlas;

    static AudioPluginAudioProcessorEditor* mainProcessEditor;

private:
    Vec2 previousMousePos;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;

    Renderer mainRenderer;

    void resizeUI();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
