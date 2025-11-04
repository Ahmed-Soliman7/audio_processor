#pragma once

#include <JuceHeader.h>
#include "PlayerGUI.h"
#include "PlayerAudio.h"

class MainComponent : public juce::AudioAppComponent
{
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    PlayerAudio playerAudio;
    PlayerAudio playerAudio2;
    PlayerGUI playerGUI;
    PlayerGUI playerGUI2;

    juce::Slider masterVolumeSlider;
    juce::Label masterVolumeLabel;
    juce::Slider balanceSlider;
    juce::Label balanceLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};