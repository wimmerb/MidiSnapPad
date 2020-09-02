#pragma once

#include <JuceHeader.h>
#include "OtherLookAndFeel.h"
#include "ScrollableNumber.h"
#include "MidiKnob.h"
#include "BackendAlgebra.h"

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
    void pushBackNewKnob ();

private:
    //==============================================================================
    // Your private member variables go here...
    void manipulateMatrixValues ();
    
    std::shared_ptr<juce::MidiOutput> midiOut;
    juce::ComboBox midiOutputList;
    std::vector<MidiKnob * > knobs;
    OtherLookAndFeel otherLookAndFeel;
    MatrixField matrix {};
    juce::Slider * editModeToggle;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
