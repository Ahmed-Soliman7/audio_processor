#include "MainComponent.h"
MainComponent::MainComponent()
	: playerGUI(playerAudio), playerGUI2(playerAudio2)
{
	addAndMakeVisible(playerGUI);
	addAndMakeVisible(playerGUI2);
	addAndMakeVisible(playlist);
	playlist.addChangeListener(this);
	playlist.onLoadToLeft = [this](const juce::File& f) { playerGUI.loadFile(f); };
	playlist.onLoadToRight = [this](const juce::File& f) { playerGUI2.loadFile(f); };

	// Master Volume Slider
	masterVolumeSlider.setRange(0.0, 1.0, 0.01);
	masterVolumeSlider.setValue(0.8);
	masterVolumeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
	masterVolumeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
	masterVolumeSlider.setColour(juce::Slider::thumbColourId, juce::Colours::orange);
	masterVolumeSlider.setColour(juce::Slider::trackColourId, juce::Colours::lightblue);
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
	masterVolumeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
	masterVolumeLabel.attachToComponent(&masterVolumeSlider, true);
	addAndMakeVisible(masterVolumeLabel);

	// Balance Slider
	balanceSlider.setRange(-1.0, 1.0, 0.1);
	balanceSlider.setValue(0.0);
	balanceSlider.setSliderStyle(juce::Slider::LinearHorizontal);
	balanceSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
	balanceSlider.setColour(juce::Slider::thumbColourId, juce::Colours::yellow);
	balanceSlider.setColour(juce::Slider::trackColourId, juce::Colours::lightgreen);
	addAndMakeVisible(balanceSlider);

	balanceLabel.setText("Balance:", juce::dontSendNotification);
	balanceLabel.setColour(juce::Label::textColourId, juce::Colours::white);
	balanceLabel.attachToComponent(&balanceSlider, true);
	addAndMakeVisible(balanceLabel);

	setSize(1200, 800);
	if (juce::RuntimePermissions::isRequired(juce::RuntimePermissions::recordAudio)
		&& !juce::RuntimePermissions::isGranted(juce::RuntimePermissions::recordAudio))
	{
		juce::RuntimePermissions::request(juce::RuntimePermissions::recordAudio,
			[&](bool granted) { setAudioChannels(granted ? 2 : 0, 2); });
	}
	else
	{
		setAudioChannels(0, 2);
	}
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
	// Gradient background
	juce::ColourGradient gradient(
		juce::Colours::darkgrey, 0, 0,
		juce::Colours::darkslategrey, getWidth(), getHeight(),
		false
	);
	g.setGradientFill(gradient);
	g.fillAll(juce::Colours::darkblue);

	auto mixerArea = getLocalBounds().removeFromTop(80).reduced(10);

	// Audio Mixer title with better styling
	g.setColour(juce::Colours::lightblue);
	g.setFont(juce::Font(20.0f, juce::Font::bold | juce::Font::italic));
	g.drawText(" AUDIO MIXER ", mixerArea.removeFromTop(15), juce::Justification::centred);

	// Decorative line
}

void MainComponent::resized()
{
	auto area = getLocalBounds().reduced(10);
	auto mixerArea = area.removeFromTop(80);
	auto masterArea = mixerArea.removeFromTop(40).reduced(5);
	auto balanceArea = mixerArea.removeFromTop(40).reduced(5);

	masterVolumeLabel.setBounds(masterArea.removeFromLeft(120));
	masterVolumeSlider.setBounds(masterArea);

	balanceLabel.setBounds(balanceArea.removeFromLeft(120));
	balanceSlider.setBounds(balanceArea);

	auto playlistArea = area.removeFromBottom(200).reduced(5);
	playlist.setBounds(playlistArea);

	auto playersArea = area;
	auto leftArea = playersArea.removeFromLeft(playersArea.getWidth() / 2).reduced(5);
	auto rightArea = playersArea.reduced(5);

	playerGUI.setBounds(leftArea);
	playerGUI2.setBounds(rightArea);
}

void MainComponent::changeListenerCallback(juce::ChangeBroadcaster* source) {}
