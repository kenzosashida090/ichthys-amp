#include "PluginEditor.h"

AsteriaAmpAudioProcessorEditor::AsteriaAmpAudioProcessorEditor(
    AsteriaAmpAudioProcessor &p)
    : AudioProcessorEditor(&p), processor(p) {
  setSize(620, 300);

  title.setText("ASTERIA AMP", juce::dontSendNotification);
  title.setJustificationType(juce::Justification::centredLeft);
  title.setFont(juce::Font(25.0f, juce::Font::bold));
  title.setColour(juce::Label::textColourId, juce::Colour(0xfff2eee7));

  addAndMakeVisible(title);

  configureSlider(input);
  configureSlider(gain);
  configureSlider(tone);
  configureSlider(master);
  configureSlider(output);

  input.setTextValueSuffix(" dB");
  master.setTextValueSuffix(" dB");
  output.setTextValueSuffix(" dB");

  auto &state = processor.parameters;

  inputAttachment = std::make_unique<Attachment>(state, "input", input);

  gainAttachment = std::make_unique<Attachment>(state, "gain", gain);

  toneAttachment = std::make_unique<Attachment>(state, "tone", tone);

  masterAttachment = std::make_unique<Attachment>(state, "master", master);

  outputAttachment = std::make_unique<Attachment>(state, "output", output);
}

void AsteriaAmpAudioProcessorEditor::configureSlider(juce::Slider &slider) {
  slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);

  slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 84, 24);

  slider.setColour(juce::Slider::rotarySliderFillColourId,
                   juce::Colour(0xffd8a65d));

  slider.setColour(juce::Slider::rotarySliderOutlineColourId,
                   juce::Colour(0xff45464e));

  slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xfff4e9d8));

  slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xfff2eee7));

  slider.setColour(juce::Slider::textBoxBackgroundColourId,
                   juce::Colour(0xff202126));

  slider.setColour(juce::Slider::textBoxOutlineColourId,
                   juce::Colours::transparentBlack);

  addAndMakeVisible(slider);
}

void AsteriaAmpAudioProcessorEditor::paint(juce::Graphics &g) {
  g.fillAll(juce::Colour(0xff17181d));

  g.setColour(juce::Colour(0xffd8a65d));
  g.fillRoundedRectangle(22.0f, 72.0f, static_cast<float>(getWidth() - 44),
                         2.0f, 1.0f);

  g.setColour(juce::Colour(0xffa7a8b1));
  g.setFont(12.0f);

  g.drawText("GUITAR PREAMP  /  PROTOTYPE 01", 24, 45, getWidth() - 48, 20,
             juce::Justification::centredLeft);
}

void AsteriaAmpAudioProcessorEditor::resized() {
  title.setBounds(24, 10, getWidth() - 48, 32);

  constexpr int sliderWidth = 108;
  constexpr int sliderHeight = 160;
  constexpr int gap = 8;
  constexpr int startX = 20;
  constexpr int startY = 94;

  juce::Slider *sliders[] = {&input, &gain, &tone, &master, &output};

  for (int i = 0; i < 5; ++i) {
    sliders[i]->setBounds(startX + i * (sliderWidth + gap), startY, sliderWidth,
                          sliderHeight);
  }
}
