#include "MainComponent.h"

MainComponent::MainComponent()
    : playerGUI(playerAudio), playerGUI2(playerAudio2)
{
    addAndMakeVisible(playerGUI);
    addAndMakeVisible(playerGUI2);

    masterVolumeSlider.setRange(0.0, 1.0, 0.01);
    masterVolumeSlider.setValue(0.8);
    masterVolumeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    masterVolumeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    masterVolumeSlider.onValueChange = [this]
        {
            float masterGain = static_cast<float>(masterVolumeSlider.getValue());

            
            if (playerAudio.isMuted())
            {
                playerAudio.setMute(false);
                playerGUI.refreshMuteButton();
                playerGUI.refreshMuteClick();
            }

            if (playerAudio2.isMuted())
            {
                playerAudio2.setMute(false);
                playerGUI2.refreshMuteButton();
                playerGUI2.refreshMuteClick();
                
            }
        };
    addAndMakeVisible(masterVolumeSlider);

    masterVolumeLabel.setText("Master Volume:", juce::dontSendNotification);
    masterVolumeLabel.attachToComponent(&masterVolumeSlider, true);
    addAndMakeVisible(masterVolumeLabel);

    balanceSlider.setRange(-1.0, 1.0, 0.1);
    balanceSlider.setValue(0.0);
    balanceSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    balanceSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    addAndMakeVisible(balanceSlider);

    balanceLabel.setText("Balance:", juce::dontSendNotification);
    balanceLabel.attachToComponent(&balanceSlider, true);
    addAndMakeVisible(balanceLabel);

    setSize(1200, 800);
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

    juce::AudioBuffer<float> tempBuffer1(bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);
    juce::AudioBuffer<float> tempBuffer2(bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);

    juce::AudioSourceChannelInfo info1(&tempBuffer1, 0, bufferToFill.numSamples);
    juce::AudioSourceChannelInfo info2(&tempBuffer2, 0, bufferToFill.numSamples);

    playerAudio.getNextAudioBlock(info1);
    playerAudio2.getNextAudioBlock(info2);

    float balance = static_cast<float>(balanceSlider.getValue());
    float leftGain = 1.0f;
    float rightGain = 1.0f;

    if (balance < 0) {
        rightGain = 1.0f + balance;
    }
    else if (balance > 0) {
        leftGain = 1.0f - balance;
    }

    float masterGain = static_cast<float>(masterVolumeSlider.getValue());

    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
    {
        auto* output = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);
        auto* input1 = tempBuffer1.getReadPointer(channel % tempBuffer1.getNumChannels());
        auto* input2 = tempBuffer2.getReadPointer(channel % tempBuffer2.getNumChannels());

        for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
        {

            float mixedSample = (input1[sample] * leftGain * masterGain) +
                (input2[sample] * rightGain * masterGain);

            output[sample] = juce::jlimit(-1.0f, 1.0f, mixedSample);
        }
    }
}

void MainComponent::releaseResources()
{
    playerAudio.releaseResources();
    playerAudio2.releaseResources();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);

    auto mixerArea = getLocalBounds().removeFromTop(80).reduced(10);
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(16.0f, juce::Font::bold));
    g.drawText("Audio Mixer", mixerArea.removeFromTop(10), juce::Justification::centred);
}

void MainComponent::resized()
{
    auto area = getLocalBounds().reduced(10);

    auto mixerArea = area.removeFromTop(80);
    auto masterArea = mixerArea.removeFromTop(40).reduced(5);
    auto balanceArea = mixerArea.removeFromTop(40).reduced(5);

    masterVolumeLabel.setBounds(masterArea.removeFromLeft(100));
    masterVolumeSlider.setBounds(masterArea);

    balanceLabel.setBounds(balanceArea.removeFromLeft(100));
    balanceSlider.setBounds(balanceArea);

    auto playersArea = area;
    auto leftArea = playersArea.removeFromLeft(playersArea.getWidth() / 2).reduced(5);
    auto rightArea = playersArea.reduced(5);

    playerGUI.setBounds(leftArea);
    playerGUI2.setBounds(rightArea);
}



