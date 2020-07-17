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
    
    std::function <void ()> informParent;
    
    void paint (juce::Graphics& g) override
    {
        float roundedRectangleCornerSize = 6.0f;
        auto bounds = getLocalBounds (). reduced (3);
        
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
        
        //TODO is repaint necessary?
        // repaint ();
        informParent ();
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
        informParent ();
    }
    
    void parentSizeChanged() override
    {
        
        auto newBounds = getBoundsInParent();
        
        constrainer.checkBounds (newBounds, getBoundsInParent(),
                                 getParentComponent()->getLocalBounds(),
                                 false, false, false, false);
        setBounds (newBounds);
        
        constrainer.setMinimumOnscreenAmounts (getHeight(), getWidth(), getHeight(), getWidth());
        
        repaint ();
        
        informParent ();
    }
    
    void toggleEdit ()
    {
        setVisible (!isVisible ());
    }
    
private:
    
    juce::ComponentBoundsConstrainer constrainer;
    juce::ComponentDragger dragger;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DraggableCursor)
};

template <typename ValueType>
class MatrixFieldArea : public juce::TextEditor
{
public:
    MatrixFieldArea ()
    {
        TextEditor::setJustification (juce::Justification::centred);
        setText ("jo");
        setWantsKeyboardFocus(true);
        setCaretVisible(false);
        setReadOnly(true);
    }
    
    void setArea (juce::Rectangle<ValueType> newArea)
    {
        area = newArea;
    }
    
    juce::Rectangle<ValueType> getArea ()
    {
        return area;
    }
    
    bool contains (juce::Point <ValueType> point)
    {
        juce::Rectangle<ValueType> checkArea = area;
        checkArea.setWidth (checkArea.getWidth() + 1);
        checkArea.setHeight (checkArea.getHeight() + 1);
        return checkArea. contains (point);
    }
    
    ValueType getDistanceFromSnapshotPosition (juce::Point <ValueType> other)
    {
        return representedSnapshotPosition.getDistanceFrom (other);
    }
    
    void paint (juce::Graphics& g) override
    {
        auto bounds = getLocalBounds();

        if (isInEditMode)
        {
            g.setColour (juce::Colours::pink.withAlpha (0.3f));
            g.fillRect (bounds);
        }

//        g.setColour (juce::Colours::white);
//        g.drawText(name, bounds, juce::Justification::centred);

    }
    
//    void resized () override
//    {
//        repaint();
//    }
//
    void mouseDown (const juce::MouseEvent& e) override
    {
        if (not isInEditMode)
            return;
        name = "click";
        repaint();
    }
    
    void toggleEdit ()
    {
        isInEditMode = !isInEditMode;
        setReadOnly (!isReadOnly ());
    }
    
    
    
    
private:
    juce::Rectangle<ValueType> area;
    juce::Point<ValueType> representedSnapshotPosition;
    juce::String name = "Smooth";
    
    bool isInEditMode {false};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MatrixFieldArea)
};


class MatrixField : public juce::Component
{
public:
    //==============================================================================
    MatrixField()
    {
        
        
        
        addAndMakeVisible (innerField);
        
        cursorField.informParent = [&]
        {
            currentCursorPositionToInnerField = cursorField.getBoundsInParent (). getCentre ();
            getCurrentlySelectedArea ();
        };
        
        nrMatrixFieldAreasChanged ();
        
        
        innerField.addAndMakeVisible (cursorField);
        cursorField.setAlwaysOnTop (false);
        cursorField.setVisible (true);
        
        
    }
    
    ~MatrixField() override
    {
        for ( auto i : matrixFieldAreas )
        {
            delete (i);
        }
        matrixFieldAreas.clear ();
    }
    //==============================================================================
    void paint (juce::Graphics& g) override
    {
        if (isInEditMode)
        {
            g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
            return;
        }
        
        float strokePathThickness = 2.0f;
        float roundedRectangleCornerSize = 6.0f;
        auto bounds = currentAnimatedBounds;
        juce::Colour backgroundColour = findColour (juce::ComboBox::backgroundColourId);
        juce::Colour greyTone = juce::Colour(0x353130).interpolatedWith (juce::Colours::white, 0.4f);
        juce::Colour whiteTone = juce::Colour(0x353130).interpolatedWith (juce::Colours::white, 0.5f);
        juce::Colour shadowTone = juce::Colour::fromRGBA(0, 0, 0, 40);
        
        juce::Path p;
        p.addRoundedRectangle (bounds, roundedRectangleCornerSize);
        
        
        g.setColour (backgroundColour);
        
        g.fillPath (p);
        
//        g.setColour(getLookAndFeel ().findColour (juce::ResizableWindow::backgroundColourId) );
//        g.strokePath (p, juce::PathStrokeType (10.0f) );
//
//        g.setColour(shadowTone);
//        for (int i = 0; i < 30; i++)
//        {
//            g.strokePath (p, juce::PathStrokeType (2.0f), juce::AffineTransform::translation (i * 1.5f, i * 1.5f));
//        }
        
        
        
        for (int i = 0; i < nrRows - 1; i++)
        {
            p.startNewSubPath ( bounds.getX () ,
                               currentAnimatedBounds.getY() + currentAnimatedBounds.getHeight () * (float) (i+1) / (float) nrRows
                               );
            p.lineTo ( bounds.getX () + bounds.getWidth (),
                      currentAnimatedBounds.getY() + currentAnimatedBounds.getHeight () * (float) (i+1) / (float) nrRows
                      );
        }
        
        for (int i = 0; i < nrColumns - 1; i++)
        {
            p.startNewSubPath (currentAnimatedBounds.getX () + currentAnimatedBounds.getWidth () * (float) (i+1) / (float) nrColumns,
                               bounds.getY ()
                               );
            p.lineTo (currentAnimatedBounds.getX () + currentAnimatedBounds.getWidth () * (float) (i+1) / (float) nrColumns,
                      (float) bounds.getY () + bounds.getHeight ()
                      );
        }
        
        
        
        g.setColour (whiteTone);
        g.strokePath (p, juce::PathStrokeType (strokePathThickness));
        
        
        
        
    }
    
    void resized () override
    {
        currentAnimatedBounds = getLocalBounds().reduced (4, 4);
        
        innerField.setBounds (currentAnimatedBounds);
        
        resizeMatrixFieldAreas ();
        
        
        auto cursorFieldBounds = cursorField.getBounds ();
        cursorField.setBounds (cursorFieldBounds. getX (),
                               cursorFieldBounds. getY (),
                               std::floor ( (double) currentAnimatedBounds.getWidth () / (double) nrColumns ),
                               std::floor ( (double) currentAnimatedBounds.getHeight () / (double) nrRows )
                               );
        
        repaint ();
    }
    
    //==============================================================================
    
    MatrixFieldArea<int> * getCurrentlySelectedArea()
    {
        for ( int i = 0; i < nrRows * nrColumns; i++)
        {
            if (matrixFieldAreas.at (i) -> contains (currentCursorPositionToInnerField))
            {
                return matrixFieldAreas.at (i);
            }
        }
        std::cout << currentCursorPositionToInnerField.getX() << " " << currentCursorPositionToInnerField.getY() << std::endl;
        try
        {
            throw 20;
        }
        catch (int e)
        {
            std::cout << "An exception occurred. Exception Nr. " << e << '\n';
        }
        return nullptr;
    }
    
    void toggleEdit ()
    {
        for (auto i : matrixFieldAreas)
        {
            i->toggleEdit ();
        }
        cursorField.toggleEdit ();
        
        isInEditMode = !isInEditMode;
        
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
    
    void nrMatrixFieldAreasChanged ()
    {
        innerField.removeAllChildren ();
        
        if (matrixFieldAreas.size () < nrRows * nrColumns)
        {
            int nrMissingAreas = nrRows * nrColumns - (int) matrixFieldAreas.size ();
            for (int i = 0; i < nrMissingAreas; i++)
            {
                matrixFieldAreas.push_back (new MatrixFieldArea<int> () );
            }
        }
        //no else here: we want to keep "hidden" Fields, just keep the sizes right
        
        for (auto i : matrixFieldAreas)
        {
            innerField.addAndMakeVisible(i);
        }
        
        resizeMatrixFieldAreas();
    }
    
    void resizeMatrixFieldAreas ()
    {
        //distribute areas !!!within innerField!!!, so they comply with currentCursorPositionToInnerField
        
        for (int rowIndex = 0; rowIndex < nrRows; rowIndex++)
        {
            for (int columnIndex = 0; columnIndex < nrColumns; columnIndex++)
            {
                bool isBoundToLeft = columnIndex == 0;
                bool isBoundToTop = rowIndex == 0;
                bool isBoundToRight = columnIndex == nrColumns - 1;
                bool isBoundToBottom = rowIndex == nrRows -1;
                
                double width = (double) currentAnimatedBounds.getWidth () / (double) nrColumns;
                double height = (double) currentAnimatedBounds.getHeight () / (double) nrRows;
                
                double left = 0.0 + (double) columnIndex * width;
                double right = 0.0 + ((double) (columnIndex + 1) ) * width;
                double top = 0.0 + (double) rowIndex * height;
                double bottom = 0.0 + ((double) (rowIndex + 1) ) * height;
                
                left =      isBoundToLeft ?     0.0                                         : std::floor (left + 1.0);
                top =       isBoundToTop ?      0.0                                         : std::floor (top + 1.0);
                right =     isBoundToRight ?    (double) currentAnimatedBounds.getWidth ()  : std::floor (right);
                bottom =    isBoundToBottom ?   (double) currentAnimatedBounds.getHeight () : std::floor (bottom);
                
                
                int indexOfArea = rowIndex * nrColumns + columnIndex;
                
                matrixFieldAreas.at(indexOfArea) -> setArea (juce::Rectangle<int> ( (int) left,
                                                                                    (int) top,
                                                                                    (int) (right - left),
                                                                                    (int) (bottom - top))
                                                             );
                matrixFieldAreas.at (indexOfArea) -> setBounds (juce::Rectangle<int> ( (int) left,
                                                                                       (int) top,
                                                                                       (int) (right - left),
                                                                                       (int) (bottom - top))
                                                                 );
            }
        }
    }
    
    Component innerField;
    
    DraggableCursor cursorField;
    
    juce::Rectangle<int> currentAnimatedBounds {};
    
    juce::Point<int> currentCursorPositionToInnerField {50, 50};
    
    std::vector <MatrixFieldArea<int> *> matrixFieldAreas;
    
    int nrRows = 4;
    int nrColumns = 3;
    
    bool isInEditMode {false};
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MatrixField)
};



