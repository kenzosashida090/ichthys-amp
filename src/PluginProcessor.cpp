#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <cmath>

AsteriaAmpAudioProcessor::AsteriaAmpAudioProcessor()
    : AudioProcessor(
          BusesProperties()
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, juce::Identifier("ASTERIA_STATE"),
                 createParameterLayout()) {}

juce::AudioProcessorValueTreeState::ParameterLayout
AsteriaAmpAudioProcessor::createParameterLayout() {
  using FloatParam = juce::AudioParameterFloat;

  juce::AudioProcessorValueTreeState::ParameterLayout layout;

  layout.add(std::make_unique<FloatParam>(
      "input", "Input", juce::NormalisableRange<float>(-18.0f, 12.0f, 0.1f),
      0.0f));

  layout.add(std::make_unique<FloatParam>(
      "gain", "Gain", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f),
      0.25f));

  layout.add(std::make_unique<FloatParam>(
      "tone", "Tone", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f),
      0.65f));

  layout.add(std::make_unique<FloatParam>(
      "master", "Master", juce::NormalisableRange<float>(-24.0f, 6.0f, 0.1f),
      -6.0f));

  layout.add(std::make_unique<FloatParam>(
      "output", "Output", juce::NormalisableRange<float>(-24.0f, 12.0f, 0.1f),
      0.0f));

  return layout;
}

bool AsteriaAmpAudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const {
  const auto input = layouts.getMainInputChannelSet();
  const auto output = layouts.getMainOutputChannelSet();

  if (input != output)
    return false;

  return input == juce::AudioChannelSet::mono() ||
         input == juce::AudioChannelSet::stereo();
}

void AsteriaAmpAudioProcessor::prepareToPlay(double sampleRate, int) {
  currentSampleRate = sampleRate;

  inputDbSmooth.reset(sampleRate, 0.02);
  gainSmooth.reset(sampleRate, 0.02);
  toneSmooth.reset(sampleRate, 0.02);
  masterDbSmooth.reset(sampleRate, 0.02);
  outputDbSmooth.reset(sampleRate, 0.02);

  inputDbSmooth.setCurrentAndTargetValue(
      *parameters.getRawParameterValue("input"));

  gainSmooth.setCurrentAndTargetValue(*parameters.getRawParameterValue("gain"));

  toneSmooth.setCurrentAndTargetValue(*parameters.getRawParameterValue("tone"));

  masterDbSmooth.setCurrentAndTargetValue(
      *parameters.getRawParameterValue("master"));

  outputDbSmooth.setCurrentAndTargetValue(
      *parameters.getRawParameterValue("output"));

  lowpassState.fill(0.0f);
}

void AsteriaAmpAudioProcessor::releaseResources() { lowpassState.fill(0.0f); }

void AsteriaAmpAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                            juce::MidiBuffer &) {
  juce::ScopedNoDenormals noDenormals;

  const int inputChannels = getTotalNumInputChannels();
  const int outputChannels = getTotalNumOutputChannels();

  for (int ch = inputChannels; ch < outputChannels; ++ch)
    buffer.clear(ch, 0, buffer.getNumSamples());

  inputDbSmooth.setTargetValue(*parameters.getRawParameterValue("input"));

  gainSmooth.setTargetValue(*parameters.getRawParameterValue("gain"));

  toneSmooth.setTargetValue(*parameters.getRawParameterValue("tone"));

  masterDbSmooth.setTargetValue(*parameters.getRawParameterValue("master"));

  outputDbSmooth.setTargetValue(*parameters.getRawParameterValue("output"));

  const int channels = juce::jmin(inputChannels, buffer.getNumChannels(), 2);

  const int numSamples = buffer.getNumSamples();

  for (int i = 0; i < numSamples; ++i) {
    const float inputGain =
        juce::Decibels::decibelsToGain(inputDbSmooth.getNextValue());

    const float gain = gainSmooth.getNextValue();
    const float tone = toneSmooth.getNextValue();

    const float masterGain =
        juce::Decibels::decibelsToGain(masterDbSmooth.getNextValue());

    const float outputGain =
        juce::Decibels::decibelsToGain(outputDbSmooth.getNextValue());

    // Gain controls how hard the virtual preamp is driven.
    const float drive = 1.0f + gain * 15.0f;

    // A simple tone control: low settings are darker.
    const float cutoffHz = 600.0f + tone * 9400.0f;

    const float coefficient = static_cast<float>(
        1.0 - std::exp(-2.0 * juce::MathConstants<double>::pi * cutoffHz /
                       currentSampleRate));

    for (int ch = 0; ch < channels; ++ch) {
      const float dry = buffer.getSample(ch, i);
      const float preamp = dry * inputGain;

      // First-generation nonlinear amp stage.
      const float saturated = std::tanh(preamp * drive);

      // One-pole low-pass filter with per-channel memory.
      lowpassState[static_cast<size_t>(ch)] +=
          coefficient * (saturated - lowpassState[static_cast<size_t>(ch)]);

      const float toneOutput = lowpassState[static_cast<size_t>(ch)];

      const float wet = toneOutput * masterGain * outputGain;

      buffer.setSample(ch, i, wet);
    }
  }
}

void AsteriaAmpAudioProcessor::getStateInformation(
    juce::MemoryBlock &destData) {
  auto state = parameters.copyState();

  std::unique_ptr<juce::XmlElement> xml(state.createXml());

  if (xml != nullptr)
    copyXmlToBinary(*xml, destData);
}

void AsteriaAmpAudioProcessor::setStateInformation(const void *data,
                                                   int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));

  if (xml != nullptr && xml->hasTagName(parameters.state.getType())) {
    parameters.replaceState(juce::ValueTree::fromXml(*xml));
  }
}

juce::AudioProcessorEditor *AsteriaAmpAudioProcessor::createEditor() {
  return new AsteriaAmpAudioProcessorEditor(*this);
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new AsteriaAmpAudioProcessor();
}
