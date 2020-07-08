#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    dogs.setBounds(0,0,100,30);
    addAndMakeVisible (&dogs);
    
    this->setLookAndFeel (&otherLookAndFeel);
    
    midiOut = juce::MidiOutput::openDevice (juce::MidiOutput::getDefaultDevice().identifier);
    setSize (600, 400);
    addAndMakeVisible (midiOutputList);
    midiOutputList.setTextWhenNoChoicesAvailable ("No MIDI Inputs Enabled");
    auto midiOutputs = juce::MidiOutput::getAvailableDevices ();
    
    juce::StringArray midiOutputNames;
    
    for (auto output : midiOutputs)
        midiOutputNames.add (output.name);
    
    midiOutputList.addItemList (midiOutputNames, 1);
    midiOutputList.onChange = [this] { setMidiOutput (midiOutputList.getSelectedItemIndex()); };
    if (midiOutputList.getSelectedId() == 0)
        setMidiOutput (0);
    
    //setOpaque (true);
    auto * slider = new juce::Slider ();
    slider->setSliderStyle (juce::Slider::SliderStyle::Rotary);
    slider->setTextBoxStyle (juce::Slider::NoTextBox, true, 0, 0);
    slider->setBounds (40, 40, 40, 40);
    addAndMakeVisible (*slider);
    slider->setRange (juce::Range<double>(0.0, 127.0), 1.0);
    
    slider->onValueChange = [this, slider]
    {
        int x = round(slider->getValue());
        auto msg = juce::MidiMessage::controllerEvent (1, 7, x);
        midiOut->sendMessageNow (msg);
    };
    
    knobs.push_back (slider);
    
    resized();
    
}


void MainComponent::setMidiOutput (int index)
{
    auto list = juce::MidiOutput::getAvailableDevices();

    auto newOutput = list[index];

    midiOut = juce::MidiOutput::openDevice (newOutput.identifier);
}

MainComponent::~MainComponent()
{
    for (auto i : knobs)
    {
        delete (i);
    }
    knobs.clear ();
    setLookAndFeel (nullptr);
    
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    /*g.setFont (juce::Font (16.0f));
    g.setColour (juce::Colours::white);
    g.drawText ("Hello World!", getLocalBounds(), juce::Justification::centred, true);*/
}

void MainComponent::resized ()
{
    auto area = getLocalBounds ();
    midiOutputList.setBounds (area.removeFromTop (36).reduced (8));
    auto area2 = area.removeFromTop (100);
    for (auto i : knobs)
    {
        i->setBounds (area2.removeFromLeft (100).reduced (8));
    }
    dogs.setBounds (area.removeFromTop (300). removeFromLeft(150));
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}
