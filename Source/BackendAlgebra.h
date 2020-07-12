/*
  ==============================================================================

    BackendAlgebra.h
    Created: 9 Jul 2020 5:23:23pm
    Author:  expert239

  ==============================================================================
*/

#pragma once
#include <xtensor/xarray.hpp>
#include <xtensor/xio.hpp>
#include <xtensor/xview.hpp>
#include <xtensor-blas/xlinalg.hpp>

class DraggableCursor : public juce::Component
{
public:
    DraggableCursor()
    {}
    
    void paint (juce::Graphics& g) override
    {
        float roundedRectangleCornerSize = 6.0f;
        auto bounds = getLocalBounds (). reduced (2);
        
        //============================ Cursor
        //        float ellipseRadius = 5.0f;
        float cursorLineThickness = 1.5f;
        juce::Colour cursorColour = juce::Colours::cyan.withAlpha(0.8f);
        g.setColour (cursorColour);
        //        g.fillEllipse (currentCursorPosition.getX() - ellipseRadius, currentCursorPosition.getY() - ellipseRadius, ellipseRadius * 2.0f, ellipseRadius * 2.0f);
        
        juce::Path p;
        p.addRoundedRectangle (bounds , roundedRectangleCornerSize - 2.0f );
        
        g.strokePath (p, juce::PathStrokeType (cursorLineThickness) );
    }
    
    void resized() override
    {
        // Just set the limits of our constrainer so that we don't drag ourselves off the screen
        constrainer.setMinimumOnscreenAmounts (getHeight(), getWidth(),
                                               getHeight(), getWidth());
        std::cout << "height, width: " << getHeight() << ", " << getWidth() << std::endl;
        //TODO is repaint necessary?
        // repaint ();
    }
    
    void mouseDown (const juce::MouseEvent& e) override
    {
        // Prepares our dragger to drag this Component
        dragger.startDraggingComponent (this, e);
    }
    
    void mouseDrag (const juce::MouseEvent& e) override
    {
        // Moves this Component according to the mouse drag event and applies our constraints to it
        dragger.dragComponent (this, e, &constrainer);
    }
    
    void parentSizeChanged() override
    {
        std::cout << "parentSizeChanged" << std::endl;
        
        auto newBounds = getBoundsInParent();
        
        constrainer.checkBounds (newBounds, getBoundsInParent(),
                                 getParentComponent()->getLocalBounds(),
                                 false, false, false, false);
        setBounds (newBounds);
        
        constrainer.setMinimumOnscreenAmounts (getHeight(), getWidth(), getHeight(), getWidth());
        
        repaint();
    }
    
private:
    juce::ComponentBoundsConstrainer constrainer;
    juce::ComponentDragger dragger;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DraggableCursor)
};

class MatrixField : public juce::Component
{
public:
    //==============================================================================
    MatrixField()
    {
        addAndMakeVisible (cursorField);
    }
    //==============================================================================
    void paint (juce::Graphics& g) override
    {
        float roundedRectangleCornerSize = 6.0f;
        auto bounds = getLocalBounds().reduced (4, 4);
        juce::Colour backgroundColour = findColour (juce::ComboBox::backgroundColourId);
        juce::Colour greyTone = juce::Colour(0x353130).interpolatedWith (juce::Colours::white, 0.4f);
        juce::Colour whiteTone = juce::Colour(0x353130).interpolatedWith (juce::Colours::white, 0.5f);
        juce::Colour shadowTone = juce::Colour::fromRGBA(0, 0, 0, 40);
        
        juce::Path p;
        p.addRoundedRectangle (bounds, roundedRectangleCornerSize);
        
//        g.setColour(getLookAndFeel ().findColour (juce::ResizableWindow::backgroundColourId) );
//        g.strokePath (p, juce::PathStrokeType (10.0f) );
//
//        g.setColour(shadowTone);
//        for (int i = 0; i < 30; i++)
//        {
//            g.strokePath (p, juce::PathStrokeType (2.0f), juce::AffineTransform::translation (i * 1.5f, i * 1.5f));
//        }
        
        
        g.setColour (backgroundColour);
        
        g.fillPath (p);
        
        g.setGradientFill (juce::ColourGradient (greyTone,
                                                 bounds.getX () ,
                                                 bounds.getY () ,
                                                 whiteTone,
                                                 bounds.getX () + bounds.getWidth(),
                                                 bounds.getY () + bounds.getHeight(),
                                                 false));
        g.strokePath (p, juce::PathStrokeType (2.0f));
        
        
    }
    
    void mouseDrag (const juce::MouseEvent &event) override
    {
        // make non-draggable when clicked in the field, but not on the cursor area ==================
        auto frameBounds = juce::Rectangle<int> (currentCursorPosition.getX() - horizontalOffset,
                                                 currentCursorPosition.getY() - verticalOffset,
                                                 horizontalOffset * 2,
                                                 verticalOffset * 2);
        
        
        if ( !frameBounds.contains (event.getPosition () ) )
            return;
        
        //move cursor area ===========================================================================
        currentCursorPosition = event.getPosition();
        currentCursorPosition.setXY (std::max (horizontalLeftBound, std::min (horizontalRightBound, currentCursorPosition.getX () ) ),
                                     std::max (verticalUpperBound,  std::min (verticalLowerBound,   currentCursorPosition.getY () ) ) );
        repaint();
    }
    
    void resized () override
    {
        currentBounds = getLocalBounds().reduced (4, 4);
        horizontalOffset = std::floor ( (double) currentBounds.getWidth () / (double) nrColumns / 2.0);
        verticalOffset = std::floor ( (double) currentBounds.getHeight () / (double) nrRows / 2.0);
        
        horizontalLeftBound = currentBounds.getX () + horizontalOffset;
        horizontalRightBound = currentBounds.getX () + currentBounds.getWidth () - horizontalOffset;
        
        verticalUpperBound = currentBounds.getY () + verticalOffset;
        verticalLowerBound = currentBounds.getY () + currentBounds.getHeight () - verticalOffset;
        
        currentCursorPosition.setXY (std::max (horizontalLeftBound, std::min (horizontalRightBound, currentCursorPosition.getX () ) ),
                                     std::max (verticalUpperBound,  std::min (verticalLowerBound,   currentCursorPosition.getY () ) ) );
        
        
        
        
        auto cursorFieldBounds = cursorField.getBounds ();
        cursorField.setBounds (cursorFieldBounds. getX (),
                               cursorFieldBounds. getY (),
                               std::floor ( (double) currentBounds.getWidth () / (double) nrColumns / 2.0 ),
                               std::floor ( (double) currentBounds.getHeight () / (double) nrRows / 2.0 )
                               );

        repaint ();
    }
    
private:
    void calculateNewMidiValues ()
    {
        double x1 = 0, y1 = 0, f1 = 1;
        double x2 = 1, y2 = 0, f2 = 0;
        double x3 = 1, y3 = 1, f3 = 1;
        double x4 = 0, y4 = 1, f4 = 0;
        
        xt::xarray<double> A
        {
            {1, x1, y1, x1 * y1},
            {1, x2, y2, x2 * y2},
            {1, x3, y3, x3 * y3},
            {1, x4, y4, x4 * y4}
        };
        
        xt::xarray<double> B
        {f1, f2, f3, f4};
        
        
        auto v = xt::linalg::solve (A, B);
        
        //xt::xarray<double> res = xt::view(arr1, 1) + arr2;
        std::function<double (xt::xarray<double> v, double x, double y)> giveFunctionOutput =
        [&](xt::xarray<double> v, double x, double y)
        {
            double a = v[0];
            double b = v[1];
            double c = v[2];
            double d = v[3];
            
            double ret = a + b*x + c*y + d*x*y;
            
            std::cout << "x, y: " << x << ", " << y << " -> " << ret << std::endl;
            
            return ret;
        };
        
        giveFunctionOutput (v, 0.5, 0.5);
        giveFunctionOutput (v, 0.3, 0.7);
        giveFunctionOutput (v, 0.1, 0.9);
        giveFunctionOutput (v, 0.99, 0.99);
    }
    
    DraggableCursor cursorField;
    
    juce::Rectangle<int> currentBounds {};
    
    juce::Point<int> currentCursorPosition {50, 50};
    
    int nrRows = 2;
    int nrColumns = 3;
    
    int horizontalLeftBound = 0;
    int horizontalRightBound = 100;
    
    int verticalUpperBound = 0;
    int verticalLowerBound = 100;
    
    int horizontalOffset = 0;
    int verticalOffset = 0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MatrixField)
};
