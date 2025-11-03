#include "PlayerAudio.h"

PlayerAudio::PlayerAudio()
{
    formatManager.registerBasicFormats();
    transportSource.setSource(nullptr);
    resampler.setResamplingRatio(playbackSpeed);
}

PlayerAudio::~PlayerAudio()
{
    transportSource.setSource(nullptr);
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resampler.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (transportSource.isPlaying())
    {
        resampler.getNextAudioBlock(bufferToFill);

        if (looping && transportSource.hasStreamFinished())
            transportSource.setPosition(0.0);

        if (abLooping && hasLoopStart && hasLoopEnd)
        {
            double currentPos = transportSource.getCurrentPosition();
            if (currentPos >= loopEnd)
                transportSource.setPosition(loopStart);
        }
    }
    else
    {
        bufferToFill.clearActiveBufferRegion();
    }
}

void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
    resampler.releaseResources();
}

void PlayerAudio::loadFile(const juce::File& audioFile)
{
    transportSource.stop();
    transportSource.setSource(nullptr);
    readerSource.reset();
    clearLoopPoints();

    if (audioFile.existsAsFile())
    {
        if (auto* reader = formatManager.createReaderFor(audioFile))
        {
            readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
            transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);
            resampler.setResamplingRatio(playbackSpeed);
            currentFile = audioFile;

            if (readerSource != nullptr)
                readerSource->setLooping(looping);
        }
    }
}

void PlayerAudio::start() { transportSource.start(); }

void PlayerAudio::stop() { transportSource.stop(); transportSource.setPosition(0.0); }

void PlayerAudio::setPosition(double position) { transportSource.setPosition(position); }

void PlayerAudio::setGain(float gain) { transportSource.setGain(gain); }

void PlayerAudio::setLooping(bool shouldLoop)
{
    looping = shouldLoop;
    if (readerSource != nullptr)
        readerSource->setLooping(looping);
}

void PlayerAudio::setLoopPointA()
{
    loopStart = transportSource.getCurrentPosition();
    hasLoopStart = true;
    if (hasLoopEnd && loopStart > loopEnd)
        loopEnd = loopStart;
}

void PlayerAudio::setLoopPointB()
{
    loopEnd = transportSource.getCurrentPosition();
    hasLoopEnd = true;
    if (hasLoopStart && loopEnd < loopStart)
        loopStart = loopEnd;
}

void PlayerAudio::toggleABLooping()
{
    if (hasLoopStart && hasLoopEnd)
        abLooping = !abLooping;
    else if (abLooping)
        abLooping = false;
}

bool PlayerAudio::isMuted() const { return muted; }

void PlayerAudio::Muted()
{
    if (!muted)
    {
        previousGain = transportSource.getGain();
        transportSource.setGain(0.0f);
        muted = true;
    }
    else
    {
        transportSource.setGain(previousGain);
        muted = false;
    }
}

void PlayerAudio::clearLoopPoints()
{
    hasLoopStart = false;
    hasLoopEnd = false;
    abLooping = false;
    loopStart = 0.0;
    loopEnd = 0.0;
}

void PlayerAudio::setPlaybackSpeed(double speed)
{
    playbackSpeed = juce::jlimit(0.25, 2.0, speed);
    resampler.setResamplingRatio(playbackSpeed);
}

juce::String PlayerAudio::getTitle() const
{
    return currentFile.getFileNameWithoutExtension();
}

juce::String PlayerAudio::getArtist() const
{
    return "Unknown Artist";
}
