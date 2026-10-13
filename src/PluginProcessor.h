#pragma once

#include <JuceHeader.h>
#include <array>

class AsteriaAmpAudioProcessor final : public juce::AudioProcessor {
public:
  AsteriaAmpAudioProcessor();
  ~AsteriaAmpAudioProcessor() override = default;

  void prepareToPlay(double sampleRate, int samplesPerBlock) override;

  void releaseResources() override;

  bool isBusesLayoutSupported(const BusesLayout &layouts) const override;

  void processBlock(juce::AudioBuffer<float> &buffer,
                    juce::MidiBuffer &midiMessages) override;

  juce::AudioProcessorEditor *createEditor() override;
  bool hasEditor() const override { return true; }

  const juce::String getName() const override { return JucePlugin_Name; }

  bool acceptsMidi() const override { return false; }
  bool producesMidi() const override { return false; }
  bool isMidiEffect() const override { return false; }

  double getTailLengthSeconds() const override { return 0.0; }

  int getNumPrograms() override { return 1; }
  int getCurrentProgram() override { return 0; }
  void setCurrentProgram(int) override {}
  const juce::String getProgramName(int) override { return {}; }
  void changeProgramName(int, const juce::String &) override {}

  void getStateInformation(juce::MemoryBlock &destData) override;

  void setStateInformation(const void *data, int sizeInBytes) override;

  juce::AudioProcessorValueTreeState parameters;

private:
  static juce::AudioProcessorValueTreeState::ParameterLayout
  createParameterLayout();

  std::array<float, 2> lowpassState{0.0f, 0.0f};

  juce::SmoothedValue<float> inputDbSmooth;
  juce::SmoothedValue<float> gainSmooth;
  juce::SmoothedValue<float> toneSmooth;
  juce::SmoothedValue<float> masterDbSmooth;
  juce::SmoothedValue<float> outputDbSmooth;

  double currentSampleRate = 44100.0;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AsteriaAmpAudioProcessor)
};
