#pragma once

#include <JuceHeader.h>

class PlayerAudio : public juce::AudioSource
{
public:
    PlayerAudio();
    ~PlayerAudio() override;

    void prepareToPlay(int, double) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo&) override;
    void releaseResources() override;

    void loadFile(const juce::File&);
    void start();
    void stop();
    void setPosition(double);
    void setGain(float);

    void setLooping(bool);
    bool isLooping() const { return looping; }

    void setLoopPointA();
    void setLoopPointB();
    void toggleABLooping();
    bool isABLooping() const { return abLooping; }
    void clearLoopPoints();

    double getLoopStart() const { return loopStart; }
    double getLoopEnd() const { return loopEnd; }
    bool hasLoopPoints() const { return hasLoopStart && hasLoopEnd; }

    bool isPlaying() const { return transportSource.isPlaying(); }
    double getCurrentPosition() const { return transportSource.getCurrentPosition(); }
    double getLengthInSeconds() const { return transportSource.getLengthInSeconds(); }

    void setMute(bool shouldMute);
    void toggleMute();
    bool isMuted() const { return muted; }

    void setPlaybackSpeed(double speed);
    double getPlaybackSpeed() const { return playbackSpeed; }

    juce::String getTitle() const;
    juce::String getArtist() const;
    double getDuration() const { return transportSource.getLengthInSeconds(); }

    int getNumChannels() const;
    double getSampleRate() const;

private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    juce::ResamplingAudioSource resampler{ &transportSource, false, 2 };
    juce::File currentFile;

    bool looping = false, abLooping = false;
    double loopStart = 0, loopEnd = 0;
    bool hasLoopStart = false, hasLoopEnd = false;
    bool muted = false;
    float currentGain = 0.5f;
    double playbackSpeed = 1.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
};
