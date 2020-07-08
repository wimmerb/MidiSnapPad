#pragma once

#include <JuceHeader.h>
#include "OtherLookAndFeel.h"
#include "ScrollableNumber.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::Component
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void setMidiOutput (int index);

private:
    //==============================================================================
    // Your private member variables go here...
    std::unique_ptr<juce::MidiOutput> midiOut;
    juce::AudioDeviceManager deviceManager;
    juce::ComboBox midiOutputList;
    std::vector<juce::Slider * > knobs;
    
    OtherLookAndFeel otherLookAndFeel;
    
    ScrollableNumber dogs{1, 127};
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
