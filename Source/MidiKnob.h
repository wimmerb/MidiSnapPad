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
    
    MidiKnob(std::unique_ptr<juce::MidiOutput> & midiOutr, UIOverlayServer & uiOverlayServer) : midiOut (midiOutr)
    {
        setWantsKeyboardFocus(true);
        //this->midiOut = midiOutr;
        
        
        addChildComponent (&midiChannel);
        addChildComponent (&midiControlChange);
        
        rotarySlider = new juce::Slider ();
        rotarySlider->setSliderStyle (juce::Slider::SliderStyle::Rotary);
        rotarySlider->setTextBoxStyle (juce::Slider::NoTextBox, true, 0, 0);
        rotarySlider->setRange (juce::Range<double>(0.0, 127.0), 1.0);
        addChildComponent (*rotarySlider);
        
        
        sliderTitle.setText ("param");
        sliderTitle.callForUIOverlay = [&]
        {
            uiOverlayServer.showTextEditorOverlay(sliderTitle);
        };
        
        addChildComponent (sliderTitle);
        
        rotarySlider->onValueChange = [&]
        {
            DBG ("onValueChange");
            int value = this->rotarySlider->getValue ();
            int * midiCh = midiChannel.getValue ();
            int * midiNr = midiControlChange.getValue ();
            //handle non-valid channels
            if ( midiCh == nullptr or midiNr == nullptr )
                return;
            auto msg = juce::MidiMessage::controllerEvent (*midiCh, *midiNr, value);
            if (this->midiOut == nullptr)
            {
                DBG ("isNull");
            }
            else
            {
                this ->midiOut->sendMessageNow (msg);
                DBG (this->midiOut->getName());
                DBG (this->midiOut->getDeviceInfo().name);
            }
            std::cout << "ValueChanged" << std::endl;
        };
        rotarySlider->onDragEnd = [&]
        {
            DBG ("onDragEnd");
            onValueManipulated ();
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
        auto bounds = getLocalBounds ().toFloat ();
        g.setColour (juce::Colour::fromRGB (90, 90, 90));
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
    
    void toggleEdit (bool isInEditMode)
    {
        this->isInEditMode = isInEditMode;
        
        midiChannel.toggleEdit (isInEditMode);
        midiControlChange.toggleEdit (isInEditMode);
        
        
        
        rotarySlider->setVisible ( !isInEditMode );
        sliderTitle.toggleEdit ( isInEditMode );
        std::cout << "jo" << std::endl;
        
        
        repaint ();
    }
    
    std::function <void ()> onValueManipulated;
    
    double getValue ()
    {
        return rotarySlider->getValue();
    }
    
    void setValue (double newValue)
    {
        newValue = std::min (127.0, std::max (newValue, 0.0) );
        rotarySlider->setValue(newValue);
    }
    
private:
    //==============================================================================
    // Your private member variables go here...
    juce::Slider * rotarySlider;
    std::unique_ptr<juce::MidiOutput> & midiOut;
    ScrollableNumber midiChannel {1, 16, 1, "Ch"};
    ScrollableNumber midiControlChange {0, 127, nr, "Nr"};
    
    MyTextEditor sliderTitle {false};
    
    bool isInEditMode {false};
    
    static int nr;
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiKnob)
};

inline int MidiKnob::nr = 0;
