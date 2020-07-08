/*
  ==============================================================================

    OtherLookAndFeel.h
    Created: 8 Jul 2020 11:11:05am
    Author:  expert239

  ==============================================================================
*/
#pragma once

class OtherLookAndFeel : public juce::LookAndFeel_V4
{
public:
    OtherLookAndFeel ()
    {
        setColour (juce::Slider::thumbColourId, juce::Colours::red);
    }
    
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider&) override
    {
        auto radius = (float) juce::jmin (width / 2, height / 2) - 4.0f;
        auto centreX = (float) x + (float) width  * 0.5f;
        auto centreY = (float) y + (float) height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        
        
        juce::Path ellipse;
        ellipse.addEllipse (rx, ry, rw, rw);
        
        g.setColour(juce::Colour::fromRGBA(0, 0, 0, 10));
        for (int i = 0; i < 5; i++)
        {
            g.fillPath (ellipse, juce::AffineTransform::translation (i*1.0f, i*1.0f) );
        }
        
        
        g.setColour (juce::Colours::darkgrey);
        g.fillPath(ellipse);
    
        g.setGradientFill (juce::ColourGradient (juce::Colour(0x353130), rx+rw, ry+rw, juce::Colours::white, rx-rw, ry-rw, false));
        g.strokePath(ellipse, juce::PathStrokeType (1.5f));
        
        
        juce::Path p;
        auto pointerLength = radius * 0.4f;
        auto pointerThickness = 6.0f;
        auto cornerThickness = 0.45f * pointerThickness;
        p.addRoundedRectangle (-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength, cornerThickness);
        p.applyTransform (juce::AffineTransform::rotation (angle).translated (centreX, centreY));
        
        
        g.setColour(juce::Colour::fromRGBA(0, 0, 0, 20));
        for (int i = 0; i < 3; i++)
        {
            g.fillPath (p, juce::AffineTransform::translation (i * 1.0f, i * 1.0f));
        }
        
        
        // pointer
        g.setColour (juce::Colours::cyan);
        g.fillPath (p);
        
        
    }
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OtherLookAndFeel)
};
