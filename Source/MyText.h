/*
  ==============================================================================

    MyText.h
    Created: 4 Sep 2020 10:06:03pm
    Author:  expert239

  ==============================================================================
*/

#pragma once

class MyText : public juce::Component
{
public:
    MyText () {}
    MyText (juce::String text)
    {
        this->text = text;
    }
    void paintOverChildren (juce::Graphics& g) override {}
    void paint (juce::Graphics& g) override
    {
        auto bounds = getLocalBounds (). reduced (4).toFloat ();
        g.setColour (findColour (OtherLookAndFeel::Colours::whiteText));
        g.drawText  (text, bounds, juce::Justification::centredLeft);
    }
private:
    juce::String text {"noTextYet"};
};
