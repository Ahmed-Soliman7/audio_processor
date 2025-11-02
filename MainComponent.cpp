#include "MainComponent.h"

MainComponent::MainComponent()
    : playerGUI(playerAudio), playerGUI2(playerAudio2)
{
    addAndMakeVisible(playerGUI);
    addAndMakeVisible(playerGUI2);
    setSize(1200, 500);
    setAudioChannels(0, 2);
}

MainComponent::~MainComponent() { shutdownAudio(); }

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
    playerAudio2.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
    juce::AudioSourceChannelInfo info1(bufferToFill);
    playerAudio.getNextAudioBlock(info1);
    juce::AudioSourceChannelInfo info2(bufferToFill);
    playerAudio2.getNextAudioBlock(info2);
}

void MainComponent::releaseResources()
{
    playerAudio.releaseResources();
    playerAudio2.releaseResources();
}

void MainComponent::paint(juce::Graphics& g) { g.fillAll(juce::Colours::darkgrey); }

void MainComponent::resized()
{
    auto area = getLocalBounds().reduced(10);
    auto leftArea = area.removeFromLeft(area.getWidth() / 2).reduced(5);
    auto rightArea = area.reduced(5);
    playerGUI.setBounds(leftArea);
    playerGUI2.setBounds(rightArea);
}
