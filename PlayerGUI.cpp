#include "PlayerGUI.h"

PlayerGUI::PlayerGUI(PlayerAudio& audioPlayer)
    : playerAudio(audioPlayer)
{
    juce::TextButton* buttons[] = {
        &loadButton, &playButton, &stopButton, &loopButton,
        &loopAButton, &loopBButton, &abLoopButton, &clearLoopButton,
        &muteButton
    };

    for (auto* btn : buttons)
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    // Volume Slider
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    volumeLabel.setText("Volume:", juce::dontSendNotification);
    volumeLabel.attachToComponent(&volumeSlider, true);
    addAndMakeVisible(volumeLabel);

    // Speed Slider
    speedSlider.setRange(0.25, 2.0, 0.05);
    speedSlider.setValue(1.0);
    speedSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    speedSlider.addListener(this);
    addAndMakeVisible(speedSlider);
    speedLabel.setText("Speed:", juce::dontSendNotification);
    speedLabel.attachToComponent(&speedSlider, true);
    addAndMakeVisible(speedLabel);

    // Position Slider
    positionSlider.setRange(0.0, 1.0, 0.001);
    positionSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    positionSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    positionSlider.addListener(this);
    addAndMakeVisible(positionSlider);

    currentTimeLabel.setText("0:00", juce::dontSendNotification);
    currentTimeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(currentTimeLabel);

    durationLabel.setText("0:00", juce::dontSendNotification);
    durationLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(durationLabel);

    loopStartLabel.setText("A: --:--", juce::dontSendNotification);
    loopStartLabel.setJustificationType(juce::Justification::centred);
    loopStartLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(loopStartLabel);

    loopEndLabel.setText("B: --:--", juce::dontSendNotification);
    loopEndLabel.setJustificationType(juce::Justification::centred);
    loopEndLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(loopEndLabel);

    loopStatusLabel.setText("Loop: Off", juce::dontSendNotification);
    loopStatusLabel.setJustificationType(juce::Justification::centred);
    loopStatusLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(loopStatusLabel);

    metadataLabel.setText("No file loaded", juce::dontSendNotification);
    metadataLabel.setJustificationType(juce::Justification::centredLeft);
    metadataLabel.setColour(juce::Label::backgroundColourId, juce::Colours::black.withAlpha(0.5f));
    metadataLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(metadataLabel);

    updateLoopButton();
    updateABLoopButton();
    updateLoopPointsDisplay();
    updateSpeedDisplay();
    updateMuteButton();
    startTimer(30);
}

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);

    auto bounds = getLocalBounds().toFloat().reduced(2.0f);
    g.setColour(juce::Colours::lightgrey.withAlpha(0.3f));
    g.drawRoundedRectangle(bounds, 5.0f, 2.0f);
}

void PlayerGUI::resized()
{
    auto area = getLocalBounds().reduced(10);

    auto metadataArea = area.removeFromTop(30);
    metadataLabel.setBounds(metadataArea);

    auto positionArea = area.removeFromTop(40);
    {
        currentTimeLabel.setBounds(positionArea.removeFromLeft(50));
        durationLabel.setBounds(positionArea.removeFromRight(50));
        positionSlider.setBounds(positionArea);
    }

    auto loopLabelsArea = area.removeFromTop(20);
    {
        loopStartLabel.setBounds(loopLabelsArea.removeFromLeft(60));
        loopLabelsArea.removeFromLeft(10);
        loopEndLabel.setBounds(loopLabelsArea.removeFromLeft(60));
        loopLabelsArea.removeFromLeft(10);
        loopStatusLabel.setBounds(loopLabelsArea.removeFromLeft(100));
    }

    auto controlsArea = area.removeFromTop(80);
    {
        auto volumeArea = controlsArea.removeFromTop(30).reduced(5);
        volumeLabel.setBounds(volumeArea.removeFromLeft(60));
        volumeSlider.setBounds(volumeArea);

        auto speedArea = controlsArea.removeFromTop(30).reduced(5);
        speedLabel.setBounds(speedArea.removeFromLeft(60));
        speedSlider.setBounds(speedArea);
    }

    auto buttonArea = area.removeFromTop(60);
    int buttonWidth = 80;
    int margin = 10;

    auto firstRow = buttonArea.removeFromTop(40);
    loadButton.setBounds(firstRow.removeFromLeft(buttonWidth + 10));
    firstRow.removeFromLeft(margin);
    playButton.setBounds(firstRow.removeFromLeft(buttonWidth));
    firstRow.removeFromLeft(margin);
    stopButton.setBounds(firstRow.removeFromLeft(buttonWidth));
    firstRow.removeFromLeft(margin);
    loopButton.setBounds(firstRow.removeFromLeft(buttonWidth));

    auto secondRow = buttonArea.removeFromTop(40);
    loopAButton.setBounds(secondRow.removeFromLeft(buttonWidth));
    secondRow.removeFromLeft(margin);
    loopBButton.setBounds(secondRow.removeFromLeft(buttonWidth));
    secondRow.removeFromLeft(margin);
    abLoopButton.setBounds(secondRow.removeFromLeft(buttonWidth));
    secondRow.removeFromLeft(margin);
    clearLoopButton.setBounds(secondRow.removeFromLeft(buttonWidth));

    auto soundArea = area.removeFromTop(40);
    muteButton.setBounds(soundArea.removeFromLeft(80));
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton) loadAudioFile();
    else if (button == &playButton) playerAudio.start();
    else if (button == &stopButton) { playerAudio.stop(); updateTimeDisplays(); }
    else if (button == &loopButton) { isLooping = !isLooping; playerAudio.setLooping(isLooping); updateLoopButton(); }
    else if (button == &loopAButton) { playerAudio.setLoopPointA(); updateLoopPointsDisplay(); updateABLoopButton(); }
    else if (button == &loopBButton) { playerAudio.setLoopPointB(); updateLoopPointsDisplay(); updateABLoopButton(); }
    else if (button == &abLoopButton) { playerAudio.toggleABLooping(); updateABLoopButton(); }
    else if (button == &clearLoopButton) { playerAudio.clearLoopPoints(); updateABLoopButton(); updateLoopPointsDisplay(); }
    else if (button == &muteButton)
    {
        playerAudio.toggleMute();
        updateMuteButton();
    }
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        playerAudio.setGain(static_cast<float>(slider->getValue()));
    }
    else if (slider == &positionSlider && !isDraggingPositionSlider)
    {
        double newPosition = positionSlider.getValue() * playerAudio.getLengthInSeconds();
        playerAudio.setPosition(newPosition);
        updateTimeDisplays();
    }
    else if (slider == &speedSlider)
    {
        playerAudio.setPlaybackSpeed(slider->getValue());
        updateSpeedDisplay();
    }
}

void PlayerGUI::timerCallback()
{
    if (!isDraggingPositionSlider && playerAudio.getLengthInSeconds() > 0)
    {
        double currentPos = playerAudio.getCurrentPosition();
        double totalLength = playerAudio.getLengthInSeconds();
        double normalizedPos = currentPos / totalLength;
        positionSlider.setValue(normalizedPos, juce::dontSendNotification);
        updateTimeDisplays();
    }
}

void PlayerGUI::updateLoopButton()
{
    loopButton.setButtonText("Loop");
    loopButton.setColour(juce::TextButton::buttonColourId, isLooping ? juce::Colours::green : juce::Colours::red);
}

void PlayerGUI::updateABLoopButton()
{
    bool hasPoints = playerAudio.hasLoopPoints();
    bool isABLooping = playerAudio.isABLooping();

    if (isABLooping) {
        abLoopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
        loopStatusLabel.setText("A-B Loop: ON", juce::dontSendNotification);
        loopStatusLabel.setColour(juce::Label::textColourId, juce::Colours::green);
    }
    else if (hasPoints) {
        abLoopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::yellow);
        loopStatusLabel.setText("A-B Loop: READY", juce::dontSendNotification);
        loopStatusLabel.setColour(juce::Label::textColourId, juce::Colours::yellow);
    }
    else {
        abLoopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::black);
        loopStatusLabel.setText("A-B Loop: OFF", juce::dontSendNotification);
        loopStatusLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    }
}

void PlayerGUI::updateLoopPointsDisplay()
{
    double loopStart = playerAudio.getLoopStart();
    double loopEnd = playerAudio.getLoopEnd();
    loopStartLabel.setText("A: " + formatTime(loopStart), juce::dontSendNotification);
    loopEndLabel.setText("B: " + formatTime(loopEnd), juce::dontSendNotification);
    updateABLoopButton();
}

void PlayerGUI::updateSpeedDisplay()
{
    speedSlider.setTextValueSuffix("x");
}

void PlayerGUI::updateTimeDisplays()
{
    double currentTime = playerAudio.getCurrentPosition();
    double duration = playerAudio.getLengthInSeconds();
    currentTimeLabel.setText(formatTime(currentTime), juce::dontSendNotification);
    durationLabel.setText(formatTime(duration), juce::dontSendNotification);
}

void PlayerGUI::updateMuteButton()
{
    muteButton.setButtonText(playerAudio.isMuted() ? "Unmute" : "Mute");
    muteButton.setColour(juce::TextButton::buttonColourId,
        playerAudio.isMuted() ? juce::Colours::red : juce::Colours::grey);
}

void PlayerGUI::loadAudioFile()
{
    fileChooser = std::make_unique<juce::FileChooser>("Select an audio file...", juce::File{}, "*.wav;*.mp3;*.aiff;*.flac");
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (file.existsAsFile())
            {
                playerAudio.loadFile(file);
                positionSlider.setValue(0.0, juce::dontSendNotification);
                updateTimeDisplays();
                updateLoopPointsDisplay();
                updateABLoopButton();
                updateMetadataDisplay();
                updateMuteButton();
            }
        });
}

juce::String PlayerGUI::formatTime(double seconds)
{
    if (seconds < 0) return "--:--";
    int totalSeconds = static_cast<int>(seconds);
    int minutes = totalSeconds / 60;
    int secs = totalSeconds % 60;
    return juce::String::formatted("%d:%02d", minutes, secs);
}

void PlayerGUI::updateMetadataDisplay()
{
    juce::String metadataText;

    if (playerAudio.getLengthInSeconds() > 0)
    {
        metadataText = "Title: " + playerAudio.getTitle() +
            " | Artist: " + playerAudio.getArtist() +
            " | Duration: " + formatTime(playerAudio.getDuration()) +
            " | Sample Rate: " + juce::String(playerAudio.getSampleRate()) + " Hz" +
            " | Channels: " + juce::String(playerAudio.getNumChannels());
    }
    else
    {
        metadataText = "No file loaded";
    }

    metadataLabel.setText(metadataText, juce::dontSendNotification);
}