#pragma once

#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::Timer
{
public:
    PlayerGUI(PlayerAudio& audioPlayer);
    ~PlayerGUI() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    void timerCallback() override;

private:
    PlayerAudio& playerAudio;

    juce::TextButton loadButton{ "Load File" };
    juce::TextButton playButton{ "Play" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton loopButton{ "Loop" };
    juce::TextButton muteButton{ "Mute" };

    juce::TextButton loopAButton{ "Set A" };
    juce::TextButton loopBButton{ "Set B" };
    juce::TextButton abLoopButton{ "A-B Loop" };
    juce::TextButton clearLoopButton{ "Clear" };

    juce::Label loopStartLabel;
    juce::Label loopEndLabel;
    juce::Label loopStatusLabel;

    juce::Slider volumeSlider;
    juce::Label volumeLabel;

    juce::Slider speedSlider;
    juce::Label speedLabel;

    juce::Slider positionSlider;
    juce::Label currentTimeLabel;
    juce::Label durationLabel;

    std::unique_ptr<juce::FileChooser> fileChooser;
    bool isLooping = false;
    bool isDraggingPositionSlider = false;

    void loadAudioFile();
    void updateLoopButton();
    void updateABLoopButton();
    void updateLoopPointsDisplay();
    void updateTimeDisplays();
    void updateSpeedDisplay();
    juce::String formatTime(double seconds);

    juce::Label metadataLabel;
    void updateMetadataDisplay();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};
