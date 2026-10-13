#pragma once

#include "PluginProcessor.h"
#include <JuceHeader.h>

class AsteriaAmpAudioProcessorEditor final : public juce::AudioProcessorEditor {
public:
  explicit AsteriaAmpAudioProcessorEditor(AsteriaAmpAudioProcessor &processor);

  ~AsteriaAmpAudioProcessorEditor() override = default;

  void paint(juce::Graphics &graphics) override;
  void resized() override;

private:
  AsteriaAmpAudioProcessor &processor;

  juce::Label title;

  juce::Slider input;
  juce::Slider gain;
  juce::Slider tone;
  juce::Slider master;
  juce::Slider output;

  using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;

  std::unique_ptr<Attachment> inputAttachment;
  std::unique_ptr<Attachment> gainAttachment;
  std::unique_ptr<Attachment> toneAttachment;
  std::unique_ptr<Attachment> masterAttachment;
  std::unique_ptr<Attachment> outputAttachment;

  void configureSlider(juce::Slider &slider);

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AsteriaAmpAudioProcessorEditor)
};
