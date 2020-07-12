/*
  ==============================================================================

    ScrollableNumber.h
    Created: 8 Jul 2020 1:41:12pm
    Author:  expert239

  ==============================================================================
*/

#import <string>
#pragma once


class ScrollableNumber  : public juce::Component, juce::AnimatedPosition<juce::AnimatedPositionBehaviours::ContinuousWithMomentum>::Listener
{
public:
    //==============================================================================
    ScrollableNumber(int begin, int end, int initValue)
    {
        //setOpaque(true);
        this->begin = begin;
        this->end   = end;
        this->value = std::max (begin, std::min (end, initValue));
        animator.addListener(this);
        animator.setLimits(juce::Range<double> (begin - 1, end));
        animator.behaviour.setFriction (0.1);
        animator.behaviour.setMinimumVelocity (3.0);
    }
    
    //==============================================================================
    void paint (juce::Graphics& g) override
    {
        juce::Rectangle <float> bounds = getLocalBounds().reduced(0.5f, 0.5f).toFloat();
        g.setColour (juce::Colour (0x353130).interpolatedWith (juce::Colours::white, 0.3) );
        g.fillRoundedRectangle (bounds, 1.0f);
        
        juce::Path p;
        p.addRoundedRectangle (bounds, 1.0f);
        g.setColour (juce::Colour::fromRGBA(0, 0, 0, 20));
        
        for (int i = 0; i < 3; i++)
        {
            g.strokePath (p, juce::PathStrokeType (2.0f), juce::AffineTransform::translation (i * 1.0f, i * 1.0f) );
        }
        
        
        
        g.setColour (juce::Colours::white);
        g.drawText (assignValueText (value), bounds, juce::Justification::centred);
        
        g.setColour (juce::Colours::white.withAlpha (0.5f) );
        g.drawText (assignValueText (value+1), bounds.getX (), bounds.getY () - 8.0f, bounds.getWidth (), bounds.getHeight (), juce::Justification::centredTop);
        g.drawText (assignValueText (value-1), bounds.getX (), bounds.getY () + bounds.getHeight () - 8.0f, bounds.getWidth (), bounds.getHeight (), juce::Justification::centredTop);
        
        g.setColour (findColour (juce::ComboBox::backgroundColourId) );
        g.drawRoundedRectangle (bounds, 3.0f, 3.0f);
    }
    
    void mouseDown (const juce::MouseEvent &event) override
    {
        animator.beginDrag();
    }
    
    void mouseUp (const juce::MouseEvent &event) override
    {
        animator.endDrag();
    }
    
    void mouseDrag (const juce::MouseEvent &event) override
    {
        auto deltaY = 0.08f * event.getDistanceFromDragStartY ();
        animator.drag (deltaY);
        //value = std::round (deltaY);

//        value -= std::round (deltaY * 0.1f);
//        value = std::max (begin, std::min (value, end));
//
//        std::cout << value;
//
//        repaint();
    }
    void positionChanged (juce::AnimatedPosition <juce::AnimatedPositionBehaviours::ContinuousWithMomentum> & aniPos, double newPosition) override
    {
        value = std::round (newPosition);
        repaint();
    }
    
    int getValue ()
    {
        if (value == begin - 1)
            return NULL;
        else
            return value;
    }
private:
    
    juce::String assignValueText (int value)
    {
        if (value == begin - 1)
            return juce::String ("None");
        if (value > end || value < begin - 1)
            return juce::String ("");
        else
            return juce::String (std::to_string (value) );
    }
    
    int begin;
    int end;
    int value;
    
    juce::AnimatedPosition <juce::AnimatedPositionBehaviours::ContinuousWithMomentum> animator;
    
    
JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScrollableNumber)
};
