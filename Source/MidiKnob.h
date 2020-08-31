/*
  ==============================================================================

    MidiSlider.h
    Created: 8 Jul 2020 8:27:52pm
    Author:  expert239

  ==============================================================================
*/

#pragma once
#include "MyTextEditor.h"

class MidiKnob  : public juce::Component
{
public:
    //==============================================================================
    
    MidiKnob(std::shared_ptr<juce::MidiOutput> midiOutr)
    {
        this->midiOut = midiOutr;
        
        
        addChildComponent (&midiChannel);
        addChildComponent (&midiControlChange);
        
        rotarySlider = new juce::Slider ();
        rotarySlider->setSliderStyle (juce::Slider::SliderStyle::Rotary);
        rotarySlider->setTextBoxStyle (juce::Slider::NoTextBox, true, 0, 0);
        rotarySlider->setRange (juce::Range<double>(0.0, 127.0), 1.0);
        addAndMakeVisible (*rotarySlider);
        
        
        sliderTitle.setText ("param");
        
        addAndMakeVisible (sliderTitle);
        
        rotarySlider->onValueChange = [this] () mutable
        {
            int value = this->rotarySlider->getValue ();
            int * midiCh = midiChannel.getValue ();
            int * midiNr = midiControlChange.getValue ();
            //handle non-valid channels
            if ( midiCh == nullptr or midiNr == nullptr )
                return;
            auto msg = juce::MidiMessage::controllerEvent (*midiCh, *midiNr, value);
            this ->midiOut->sendMessageNow (msg);
            
        };
        nr++;
    }
    ~MidiKnob() override
    {
         delete (rotarySlider);
    }
    //==============================================================================
    
    void paint (juce::Graphics& g) override
    {
//        if (!isInEditMode)
//            return;
//        auto bounds = getLocalBounds ().toFloat ();
//        g.setColour (juce::Colours::pink.withAlpha (0.3f));
        //g.fillRect (bounds);
    }
    
    void resized() override
    {
        auto area = getLocalBounds ();
        auto upperArea = area. removeFromTop (area. getHeight () * 0.7f );
        
        rotarySlider-> setBounds (upperArea. reduced (4.0f, 4.0f));
        
        midiChannel. setBounds (upperArea. removeFromLeft (upperArea.getWidth () / 2.0f ). reduced (4.0f, 4.0f));
        midiControlChange. setBounds (upperArea. reduced (4.0f, 4.0f));
        
        sliderTitle. setBounds (area. reduced (4.0f, 4.0f));
        
    }
    
    void toggleEdit ()
    {
        isInEditMode = !isInEditMode;
        
        midiChannel.toggleEdit ();
        midiControlChange.toggleEdit ();
        
        
        
        rotarySlider->setVisible ( !rotarySlider->isVisible () );
        sliderTitle.toggleEdit ( isInEditMode );
        std::cout << "jo" << std::endl;
        
        
        repaint ();
    }
    
private:
    //==============================================================================
    // Your private member variables go here...
    juce::Slider * rotarySlider;
    std::shared_ptr<juce::MidiOutput> midiOut;
    ScrollableNumber midiChannel {1, 16, 1, "Ch"};
    ScrollableNumber midiControlChange {0, 127, nr, "Nr"};
    
    MyTextEditor sliderTitle;
    
    bool isInEditMode {false};
    
    static int nr;
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiKnob)
};

inline int MidiKnob::nr = 0;
