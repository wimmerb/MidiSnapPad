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
        
        auto radius = (float) juce::jmin (width / 2, height / 2) - 14.0f; //resulting in -4 reduction for component bound (see variable outerCircle/outerRadius)
        auto centreX = (float) x + (float) width  * 0.5f;
        auto centreY = (float) y + (float) height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        
        
        
        // Outer Index Circle ==============================================================================
        juce::Path outerCircle;
        juce::Colour outerCircleBackgroundColour = this->findColour (juce::ComboBox::backgroundColourId) ;
        juce::Colour outerCircleProgressBarColour = juce::Colours::cyan.withAlpha(0.8f);
        float outerCircleBackgroundThickness = 8.0f;
        float outerCircleProgressBarThickness = 3.0f;
        
        auto outerRadius = radius + 10.0f;
        juce::Point<float> circlePoint {0, -outerRadius};
        float nrSamplingPoints = 127.0f;
        
        // outer circle Background
        for (int i = 0; i < nrSamplingPoints + 1.0f; i++)
        {
            float tmpAngle = rotaryStartAngle + i / nrSamplingPoints * (rotaryEndAngle - rotaryStartAngle);
            if (i == 0)
                outerCircle.startNewSubPath (circlePoint.transformedBy (juce::AffineTransform::rotation (tmpAngle).translated (centreX, centreY) ) );
            else
                outerCircle.lineTo (circlePoint.transformedBy (juce::AffineTransform::rotation (tmpAngle).translated (centreX, centreY) ) );
        }
        
        g.setColour (outerCircleBackgroundColour);
        g.strokePath (outerCircle, juce::PathStrokeType (outerCircleBackgroundThickness, juce::PathStrokeType::JointStyle::mitered, juce::PathStrokeType::EndCapStyle::rounded) );
        
        outerCircle.clear();
        
        // outer circle "progress bar"
        for (int i = 0; i / nrSamplingPoints <= sliderPos; i++)
        {
            float tmpAngle = rotaryStartAngle + i / nrSamplingPoints * (rotaryEndAngle - rotaryStartAngle);
            if (i == 0)
                outerCircle.startNewSubPath (circlePoint.transformedBy (juce::AffineTransform::rotation (tmpAngle).translated (centreX, centreY) ) );
            else
                outerCircle.lineTo (circlePoint.transformedBy (juce::AffineTransform::rotation (tmpAngle).translated (centreX, centreY) ) );
        }
        
        g.setColour (outerCircleProgressBarColour);
        g.strokePath (outerCircle, juce::PathStrokeType (outerCircleProgressBarThickness, juce::PathStrokeType::JointStyle::mitered, juce::PathStrokeType::EndCapStyle::rounded) );
        
        // Knob ==============================================================================
        juce::Path ellipse;
        ellipse.addEllipse (rx, ry, rw, rw);
        
        // Shadow for Knob
        g.setColour(juce::Colour::fromRGBA(0, 0, 0, 10));
        for (int i = 0; i < 5; i++)
        {
            g.fillPath (ellipse, juce::AffineTransform::translation (i * 1.5f, i * 1.5f) );
        }
        
        // knob
        g.setColour (juce::Colours::darkgrey);
        g.fillPath(ellipse);
        
        // knob rim
        juce::Colour greyTone = juce::Colour(0x353130);
        juce::Colour whiteTone = greyTone.interpolatedWith(juce::Colours::white, 0.5f);
        g.setGradientFill (juce::ColourGradient (greyTone, rx+rw, ry+rw, whiteTone, rx, ry, false));
        g.strokePath (ellipse, juce::PathStrokeType (1.5f));
        
        // Pointer ==============================================================================
        juce::Path p;
        auto pointerLength = radius * 0.4f;
        auto pointerThickness = 6.0f;
        auto cornerThickness = 0.45f * pointerThickness;
        p.addRoundedRectangle (-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength, cornerThickness);
        p.applyTransform (juce::AffineTransform::rotation (angle).translated (centreX, centreY));
        
        // pointer shadow
        g.setColour(juce::Colour::fromRGBA(0, 0, 0, 20));
        for (int i = 0; i < 3; i++)
        {
            g.fillPath (p, juce::AffineTransform::translation (i * 1.5f, i * 1.5f));
        }
        
        
        // pointer
        g.setColour (juce::Colours::cyan);
        g.fillPath (p);
        
        
    }
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OtherLookAndFeel)
};
