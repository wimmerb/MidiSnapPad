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
    ScrollableNumber(int begin, int end)
    {
        //setOpaque(true);
        this->begin = begin;
        this->end   = end;
        this->value = begin;
        animator.addListener(this);
        animator.setLimits(juce::Range<double> (begin, end));
    }
    
    //==============================================================================
    void paint (juce::Graphics& g) override
    {
        juce::Rectangle <float> bounds = getLocalBounds().reduced(0.5f, 0.5f).toFloat();
        g.setColour (juce::Colours::grey);
        g.fillRoundedRectangle (bounds, 1.0f);
        
        juce::Path p;
        p.addRoundedRectangle (bounds, 1.0f);
        g.setColour (juce::Colour::fromRGBA(0, 0, 0, 20));
        
        for (int i = 0; i < 3; i++)
        {
            g.strokePath (p, juce::PathStrokeType (1.5f), juce::AffineTransform::translation (i * 1.0f, i * 1.0f));
        }
        
        g.setColour (juce::Colours::white);
        g.drawRoundedRectangle (bounds, 1.0f, 1.0f);
        g.drawText (std::to_string (value), bounds, juce::Justification::centred);
//        g.drawText (std::to_string (value+1), bounds.getX (), bounds.getY () - 8.0f, bounds.getWidth (), bounds.getHeight (), juce::Justification::centredTop);
//        g.drawText (std::to_string (value-1), bounds.getX (), bounds.getY () + bounds.getHeight () - 8.0f, bounds.getWidth (), bounds.getHeight (), juce::Justification::centredTop);
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
        auto deltaY = -0.2f * event.getDistanceFromDragStartY ();
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
private:
    int begin;
    int end;
    int value;
    
    juce::AnimatedPosition <juce::AnimatedPositionBehaviours::ContinuousWithMomentum> animator;
    
    
JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScrollableNumber)
};
