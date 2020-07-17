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
        
        
        addChildComponent (&midiChannel);
        addChildComponent (&midiControlChange);
        
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
    
    void paint (juce::Graphics& g) override
    {
        if (!isInEditMode)
            return;
        auto bounds = getLocalBounds ().toFloat ();
        g.setColour (juce::Colours::pink.withAlpha (0.3f));
        //g.fillRect (bounds);
    }
    
    void resized() override
    {
        auto area = getLocalBounds ();
        auto upperArea = area.removeFromTop (area.getHeight() *0.7f );
        
        rotarySlider->setBounds (upperArea. reduced (4.0f, 4.0f));
        
        midiChannel.setBounds (upperArea.removeFromLeft (upperArea.getWidth () / 2.0f ). reduced (4.0f, 4.0f));
        midiControlChange.setBounds (upperArea. reduced (4.0f, 4.0f));
        
    }
    
    void toggleEdit ()
    {
        midiChannel.toggleEdit ();
        midiControlChange.toggleEdit ();
        
        
        
        rotarySlider->setVisible ( !rotarySlider->isVisible () );
        
        isInEditMode = !isInEditMode;
        repaint ();
    }
    
private:
    //==============================================================================
    // Your private member variables go here...
    juce::Slider * rotarySlider;
    std::shared_ptr<juce::MidiOutput> midiOut;
    ScrollableNumber midiChannel {1, 16, 2, "Ch"}; //TODO bug in ScrollableNumber, if startValue = 1 here -> animation initially only showing on dragup.
    ScrollableNumber midiControlChange {0, 127, nr, "Nr"};
    
    bool isInEditMode {false};
    
    static int nr;
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiKnob)
};

inline int MidiKnob::nr = 0;
