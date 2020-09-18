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
        setColour (juce::ResizableWindow::backgroundColourId, juce::Colour::fromRGB (47, 49, 52));
//        setColour (generalBackground, juce::Colour::fromRGB (30, 30, 30));
//        setColour (archiveBackground, juce::Colour::fromRGB (47, 49, 52));
//        setColour (archiveDarkGrey, juce::Colour::fromRGB (33, 40, 38));
        setColour (Colours::generalBackground, juce::Colour::fromRGB (30, 30, 30));
        setColour (Colours::archiveBackground, juce::Colour::fromRGB (49, 51, 52));
        setColour (Colours::archiveDarkGrey, juce::Colour::fromRGB (42, 42, 42));
        setColour (Colours::archiveLightGrey, juce::Colours::darkgrey);
        setColour (Colours::brightblue, juce::Colour::fromRGB (19, 190, 190));
        setColour (Colours::brightbrightblue, juce::Colour::fromRGB (20, 220, 220));
        setColour (Colours::whiteText, juce::Colour::fromRGB (240, 240, 240));
        setColour (Colours::darkText, juce::Colour::fromRGB (200, 200, 200));
        setColour (Colours::archiveOutline, juce::Colour::fromRGB (100, 100, 100));
    }
    
    struct Colours {
    enum Cols
    {
        generalBackground,
        archiveBackground,
        archiveLightGrey,
        archiveDarkGrey,
        archiveOutline,
        shadow,
        brightblue,
        brightbrightblue,
        darkblue,
        whiteText,
        darkText
    };
    };
    
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
        juce::Colour outerCircleBackgroundColour = this->findColour (Colours::archiveDarkGrey) ;
        juce::Colour outerCircleProgressBarColour = findColour (Colours::brightblue);
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
        g.strokePath (outerCircle, juce::PathStrokeType (outerCircleBackgroundThickness, juce::PathStrokeType::JointStyle::beveled, juce::PathStrokeType::EndCapStyle::rounded) );
        
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
        g.strokePath (outerCircle, juce::PathStrokeType (outerCircleProgressBarThickness, juce::PathStrokeType::JointStyle::beveled, juce::PathStrokeType::EndCapStyle::rounded) );
        
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
        g.setColour (findColour (Colours::archiveLightGrey));
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
        g.setColour (findColour (Colours::brightbrightblue));
        g.fillPath (p);
        
        
    }
    
    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           const juce::Slider::SliderStyle style, juce::Slider& slider) override
    {
        
        if (style != juce::Slider::LinearHorizontal)
        {
            juce::LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
            return;
        }
        
        auto bounds = g.getInternalContext().getClipBounds ().reduced (4, 0);
        bounds.removeFromTop (2);
        
        g.setColour (findColour (Colours::generalBackground));
        g.fillAll();

        juce::Path p;
//        p.addRoundedRectangle (bounds, roundedRectangleCornerSize);
//
//        g.setColour (backgroundColour);
//        //g.fillPath (p);
//
//        g.setColour (whiteTone);
//        g.strokePath (p, juce::PathStrokeType (strokePathThickness));
        
        
        auto boundsLeft = bounds.removeFromLeft (bounds.getWidth () / 2);
        auto boundsRight = bounds;
//        g.setColour (juce::Colours::pink);
//        g.fillRect (bounds);
        
        auto currentIndicatedBounds = boundsLeft;
        if (sliderPos == x) //left option chosen
            {
                //Drawing the grey background
                currentIndicatedBounds = boundsRight;
                currentIndicatedBounds.removeFromTop (2);
                currentIndicatedBounds.removeFromRight (2);
                auto cs = 20.0f;
                auto cs45 = 20.0f * 0.45f;
                
                
                p.clear ();
                p.startNewSubPath(currentIndicatedBounds.getBottomLeft ().toFloat ());
                p.lineTo (currentIndicatedBounds.getTopLeft (). toFloat ());
                p.lineTo (currentIndicatedBounds.getTopRight (). getX () - cs, currentIndicatedBounds.getTopRight (). getY ());
                //p.cubicTo (x, y + cs45, x + cs45, y, x + cs, y);
                p.cubicTo (currentIndicatedBounds.getTopRight (). getX () - cs + cs45, currentIndicatedBounds.getTopRight (). getY (),
                           currentIndicatedBounds.getTopRight (). getX (), currentIndicatedBounds.getTopRight (). getY () + cs45,
                           currentIndicatedBounds.getTopRight (). getX (), currentIndicatedBounds.getTopRight (). getY () + cs);
                p.lineTo (currentIndicatedBounds.getBottomRight (). getX (), currentIndicatedBounds.getBottomRight (). getY ());
                
                g.setColour (findColour (Colours::archiveLightGrey));
                g.fillPath (p);
                
                g.setColour (findColour (Colours::archiveOutline).interpolatedWith (findColour (Colours::archiveBackground), 0.5f));
                g.strokePath (p, juce::PathStrokeType (0.5f, juce::PathStrokeType::JointStyle::beveled, juce::PathStrokeType::EndCapStyle::square));

                
                //Drawing shadow on inside
                currentIndicatedBounds = boundsRight;
                p.clear ();
                p.startNewSubPath (currentIndicatedBounds.getX () - 3, currentIndicatedBounds.getY ());
                p.lineTo (currentIndicatedBounds.getX () - 3, currentIndicatedBounds.getBottom () + 3);
                p.lineTo (currentIndicatedBounds.getRight (), currentIndicatedBounds.getBottom () + 3);
                p.lineTo (currentIndicatedBounds.getRight (), currentIndicatedBounds.getBottom ());
                p.lineTo (currentIndicatedBounds.getX (), currentIndicatedBounds.getBottom ());
                p.lineTo (currentIndicatedBounds.getX (), currentIndicatedBounds.getY ());
                
                auto shadow = juce::DropShadow (juce::Colour::fromRGBA (0, 0, 0, 200), 7, juce::Point<int> (0, 0));
//                p.applyTransform (juce::AffineTransform::translation (0.0f, 0.5f));

                shadow.drawForPath (g, p);
                
                //Drawing white edge and background for highlighted button
                
                cs = 20.0f;
                cs45 = 20.0f * 0.45f;
                
                p.clear ();
                currentIndicatedBounds = boundsLeft;
                p.startNewSubPath(currentIndicatedBounds.getBottomRight ().toFloat ());
                p.lineTo (currentIndicatedBounds.getTopRight (). toFloat ());
                p.lineTo (currentIndicatedBounds.getTopLeft (). getX () + cs, currentIndicatedBounds.getTopLeft (). getY ());
                p.cubicTo (currentIndicatedBounds.getTopLeft (). getX () + cs - cs45, currentIndicatedBounds.getTopLeft (). getY (),
                           currentIndicatedBounds.getTopLeft (). getX (), currentIndicatedBounds.getTopLeft (). getY () + cs45,
                           currentIndicatedBounds.getTopLeft (). getX (), currentIndicatedBounds.getTopLeft (). getY () + cs);
                p.lineTo (currentIndicatedBounds.getBottomLeft (). getX (), currentIndicatedBounds.getBottomLeft (). getY ());
                
                
                g.setColour (findColour (Colours::archiveBackground));
                g.fillPath (p);
                
                g.setColour (findColour (Colours::archiveOutline));
                g.strokePath (p, juce::PathStrokeType (1.0f, juce::PathStrokeType::JointStyle::beveled, juce::PathStrokeType::EndCapStyle::square));
                
                p.clear ();
                currentIndicatedBounds = boundsRight;
                p.startNewSubPath (currentIndicatedBounds.getBottomRight ().toFloat ());
                p.lineTo (currentIndicatedBounds.getBottomLeft ().toFloat ());
                
                p.applyTransform (juce::AffineTransform::translation (0.0f, -0.5f));
                g.setColour (findColour (Colours::archiveOutline));
                g.strokePath (p, juce::PathStrokeType (1.0f, juce::PathStrokeType::JointStyle::beveled, juce::PathStrokeType::EndCapStyle::square));
                
                g.setFont (juce::Font (16.0f));
                
                g.setColour (findColour (Colours::whiteText));
                g.drawText("Play", boundsLeft, juce::Justification::centred);
                
                g.setColour (findColour (Colours::darkText));
                g.drawText("Edit", boundsRight, juce::Justification::centred);
            }
        if (sliderPos == x + width) //right option chosen
            {
                //Drawing the grey background
                currentIndicatedBounds = boundsLeft;
                currentIndicatedBounds.removeFromTop (2);
                currentIndicatedBounds.removeFromLeft (2);
                auto cs = 20.0f;
                auto cs45 = 20.0f * 0.45f;
                
                p.clear ();
                p.startNewSubPath(currentIndicatedBounds.getBottomRight ().toFloat ());
                p.lineTo (currentIndicatedBounds.getTopRight (). toFloat ());
                p.lineTo (currentIndicatedBounds.getTopLeft (). getX () + cs, currentIndicatedBounds.getTopLeft (). getY ());
                //p.cubicTo (x, y + cs45, x + cs45, y, x + cs, y);
                p.cubicTo (currentIndicatedBounds.getTopLeft (). getX () + cs - cs45, currentIndicatedBounds.getTopLeft (). getY (),
                           currentIndicatedBounds.getTopLeft (). getX (), currentIndicatedBounds.getTopLeft (). getY () + cs45,
                           currentIndicatedBounds.getTopLeft (). getX (), currentIndicatedBounds.getTopLeft (). getY () + cs);
                p.lineTo (currentIndicatedBounds.getBottomLeft (). getX (), currentIndicatedBounds.getBottomLeft (). getY ());
                
                g.setColour (findColour (Colours::archiveLightGrey));
                g.fillPath (p);
                
                g.setColour (findColour (Colours::archiveOutline).interpolatedWith (findColour (Colours::archiveBackground), 0.5f));
                g.strokePath (p, juce::PathStrokeType (0.5f, juce::PathStrokeType::JointStyle::beveled, juce::PathStrokeType::EndCapStyle::square));
                
                //Drawing shadow on inside
                currentIndicatedBounds = boundsLeft;
                p.clear ();
                p.startNewSubPath (currentIndicatedBounds.getRight () + 3, currentIndicatedBounds.getY ());
                p.lineTo (currentIndicatedBounds.getRight () + 3, currentIndicatedBounds.getBottom () + 3);
                p.lineTo (currentIndicatedBounds.getX (), currentIndicatedBounds.getBottom () + 3);
                p.lineTo (currentIndicatedBounds.getX (), currentIndicatedBounds.getBottom ());
                p.lineTo (currentIndicatedBounds.getRight (), currentIndicatedBounds.getBottom ());
                p.lineTo (currentIndicatedBounds.getRight (), currentIndicatedBounds.getY ());
                
                
                auto shadow = juce::DropShadow (juce::Colour::fromRGBA (0, 0, 0, 200), 7, juce::Point<int> (0, 0));
//                p.applyTransform (juce::AffineTransform::translation (0.0f, 0.5f));
                shadow.drawForPath (g, p);
                
                
                //Drawing white edge and background for highlighted button
                cs = 20.0f;
                cs45 = 20.0f * 0.45f;
                
                p.clear ();
                currentIndicatedBounds = boundsRight;
                p.startNewSubPath (currentIndicatedBounds.getBottomLeft ().toFloat ());
                p.lineTo (currentIndicatedBounds.getTopLeft (). toFloat ());
                p.lineTo (currentIndicatedBounds.getTopRight (). getX () - cs, currentIndicatedBounds.getTopLeft (). getY ());
                p.cubicTo (currentIndicatedBounds.getTopRight (). getX () - cs + cs45, currentIndicatedBounds.getTopRight (). getY (),
                           currentIndicatedBounds.getTopRight (). getX (), currentIndicatedBounds.getTopRight (). getY () + cs45,
                           currentIndicatedBounds.getTopRight (). getX (), currentIndicatedBounds.getTopRight (). getY () + cs);
                p.lineTo (currentIndicatedBounds.getBottomRight (). getX (), currentIndicatedBounds.getBottomRight (). getY ());
                
                g.setColour (findColour (Colours::archiveBackground));
                g.fillPath (p);
                
                g.setColour (findColour (Colours::archiveOutline));
                g.strokePath (p, juce::PathStrokeType (1.0f, juce::PathStrokeType::JointStyle::beveled, juce::PathStrokeType::EndCapStyle::square));
                
                p.clear ();
                currentIndicatedBounds = boundsLeft;
                p.startNewSubPath (currentIndicatedBounds.getBottomLeft ().toFloat ());
                p.lineTo (currentIndicatedBounds.getBottomRight ().toFloat ());
                p.applyTransform (juce::AffineTransform::translation (0.0f, -0.5f));
                g.setColour (findColour (Colours::archiveOutline));
                g.strokePath (p, juce::PathStrokeType (1.0f, juce::PathStrokeType::JointStyle::beveled, juce::PathStrokeType::EndCapStyle::square));
                
                
                g.setFont (juce::Font (16.0f));
                g.setColour (findColour (Colours::whiteText));
                g.drawText("Edit", boundsRight, juce::Justification::centred);
                g.setColour (findColour (Colours::darkText));
                g.drawText("Play", boundsLeft, juce::Justification::centred);
            }
    }
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OtherLookAndFeel)
};
