/*
  ==============================================================================

    MidiSlider.h
    Created: 8 Jul 2020 8:27:52pm
    Author:  expert239

  ==============================================================================
*/

#pragma once

class MidiKnob  : public juce::Component
{
public:
    //==============================================================================
    
    MidiKnob(std::shared_ptr<juce::MidiOutput> midiOutr)
    {
        this->midiOut = midiOutr;
        
        
        addAndMakeVisible (&midiChannel);
        addAndMakeVisible (&midiControlChange);
        
        rotarySlider = new juce::Slider ();
        rotarySlider->setSliderStyle (juce::Slider::SliderStyle::Rotary);
        rotarySlider->setTextBoxStyle (juce::Slider::NoTextBox, true, 0, 0);
        rotarySlider->setRange (juce::Range<double>(0.0, 127.0), 1.0);
        addAndMakeVisible (*rotarySlider);
        
        rotarySlider->onValueChange = [this] () mutable
        {
            int x = round(this->rotarySlider->getValue());
            auto msg = juce::MidiMessage::controllerEvent (1, 7, x);
            this ->midiOut->sendMessageNow (msg);
            
        };
        nr++;
    }
    ~MidiKnob() override
    {
         delete (rotarySlider);
    }
    //==============================================================================
    
    void resized() override
    {
        auto area = getLocalBounds ();
        rotarySlider->setBounds (area.removeFromTop (area.getHeight() *0.7f ). reduced (4.0f, 4.0f));
        midiChannel.setBounds (area.removeFromLeft (area.getWidth () / 2.0f ). reduced (4.0f, 4.0f));
        midiControlChange.setBounds (area. reduced (4.0f, 4.0f));
        
    }
    
private:
    //==============================================================================
    // Your private member variables go here...
    juce::Slider * rotarySlider;
    std::shared_ptr<juce::MidiOutput> midiOut;
    ScrollableNumber midiChannel {1, 16, 1};
    ScrollableNumber midiControlChange {0, 127, nr};
    static int nr;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiKnob)
};

inline int MidiKnob::nr = 0;
