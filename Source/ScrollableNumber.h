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
    ScrollableNumber(int begin, int end, int initValue, juce::String name)
    {
        //setOpaque(true);
        this->begin = begin;
        this->end   = end;
        this->value = std::max (begin - 1, std::min (end, initValue));
        this->name = name;
        
        animator.addListener(this);
        animator.setLimits(juce::Range<double> (begin - 1, end));
        animator.behaviour.setFriction (0.1);
        animator.behaviour.setMinimumVelocity (1.0);
        animator.setPosition (this->value);
        
        //setVisible (false);
    }
    
    //==============================================================================
    void paint (juce::Graphics& g) override
    {
        
        juce::Rectangle <float> bounds = getLocalBounds().toFloat().reduced (0.5f);
        float cornerSize = 3.0f;
        
        //name display==================================================================
        g.setColour (findColour (OtherLookAndFeel::Colours::whiteText));
        g.drawText(name, bounds.removeFromBottom (0.3f * bounds.getHeight () ), juce::Justification::centred);
        
        //background==================================================================
        g.setColour (juce::Colour (0x353130).interpolatedWith (juce::Colours::white, 0.3) );
        g.fillRoundedRectangle (bounds, cornerSize);
        
        //Number text==================================================================
        g.setColour (findColour (OtherLookAndFeel::Colours::whiteText));
        g.drawText (assignValueText (value), bounds, juce::Justification::centred);
        
        
        float fontHeight = g.getCurrentFont().getHeight();
        g.setColour (findColour (OtherLookAndFeel::Colours::whiteText).withAlpha (0.3f) );
        
        
        //"hidden" Text vertically rescaled to get number roll feel
        float hiddenTextOffset = bounds.getHeight () * 0.02f;
        g.saveState ();
        auto upperTextBounds = juce::Rectangle<float> (bounds.getX (), bounds.getY () + hiddenTextOffset, bounds.getWidth (), fontHeight );
        g.addTransform (juce::AffineTransform::scale (1.0f, 0.7f, upperTextBounds.getCentre ().getX (), upperTextBounds.getCentre ().getY () ) );
        g.drawText (assignValueText (value+1), upperTextBounds, juce::Justification::centredTop);
        g.restoreState ();
        
        g.saveState ();
        auto lowerTextBounds = juce::Rectangle<float> (bounds.getX (), bounds.getBottom () - fontHeight - hiddenTextOffset, bounds.getWidth (), fontHeight );
        g.addTransform (juce::AffineTransform::scale (1.0f, 0.7f, lowerTextBounds.getCentre ().getX (), lowerTextBounds.getCentre ().getY () ) );
        g.drawText (assignValueText (value-1), lowerTextBounds, juce::Justification::centredTop);
        g.restoreState ();
        
        //borderShadow==================================================================
        juce::Path p;
        
        //parameters for adaption see below, cs means cornersize for shadow!
        float cs = std::min (cornerSize - 1.0f, std::min (bounds.getWidth (), bounds.getHeight () ) * 0.5f );
        auto cs45 = cs * 0.45f;
        float x = bounds.getX (), y = bounds.getY ();
        
        //p.addRoundedRectangle (bounds, 1.0f);
        p.startNewSubPath (bounds.getX (), bounds.getBottom () - cs );
        p.lineTo (bounds.getX (), y + cs );
        
        //adapted this snippet from addRoundedRectangle implementation in Path
        //if (curveTopLeft)
        //{
        //    startNewSubPath (x, y + csy);
        //    cubicTo (x, y + cs45y, x + cs45x, y, x + csx, y);
        //}
        
        p.startNewSubPath (x, y + cs);
        p.cubicTo (x, y + cs45, x + cs45, y, x + cs, y);
        //end adaption
        
        p.lineTo (bounds.getRight () - 2.0f, bounds.getY () );
        
        for (int i = 2; i < 6; i++)
        {
            g.setColour (juce::Colour::fromRGBA(0, 0, 0, 30 - 5 * i));
            g.strokePath (p, juce::PathStrokeType (2.0f, juce::PathStrokeType::JointStyle::curved, juce::PathStrokeType::EndCapStyle::butt), juce::AffineTransform::translation (i * 0.5f, i * 0.5f) );
        }
        
        //border==================================================================
        g.setColour (findColour (juce::ComboBox::backgroundColourId) );
        g.drawRoundedRectangle (bounds, cornerSize, 1.5f);
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
        
    }
    void positionChanged (juce::AnimatedPosition <juce::AnimatedPositionBehaviours::ContinuousWithMomentum> & aniPos, double newPosition) override
    {
        value = std::round (newPosition);
        repaint();
    }
    
    int * getValue ()
    {
        if (value == begin - 1)
            return nullptr;
        else
            return new int (value);
    }
    
    void toggleEdit (bool isInEditMode)
    {
        setVisible ( isInEditMode );
        repaint ();
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
    
    juce::String name {};
    
    int begin;
    int end;
    int value;
    
    juce::AnimatedPosition <juce::AnimatedPositionBehaviours::ContinuousWithMomentum> animator;
    
    
JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScrollableNumber)
};
