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
#include <xtensor/xadapt.hpp>
#include <xtensor-blas/xlinalg.hpp>
#include "MyTextEditor.h"

class DraggableCursor : public juce::Component
{
public:
    DraggableCursor()
    {}
    
    std::function <void ()> informParent;
    std::function <void ()> alignPosition;
    
    void paint (juce::Graphics& g) override
    {
        float roundedRectangleCornerSize = 6.0f;
        auto bounds = getLocalBounds (). reduced (3);
        
        //============================ Cursor
        //        float ellipseRadius = 5.0f;
        float cursorLineThickness = 1.5f;
        juce::Colour cursorColour = findColour (OtherLookAndFeel::Colours::brightblue);
        g.setColour (cursorColour);
        //        g.fillEllipse (currentCursorPosition.getX() - ellipseRadius, currentCursorPosition.getY() - ellipseRadius, ellipseRadius * 2.0f, ellipseRadius * 2.0f);
        
        juce::Path p;
        p.addRoundedRectangle (bounds , roundedRectangleCornerSize - 2.0f );
        
        g.strokePath (p, juce::PathStrokeType (cursorLineThickness) );
        
        g.setColour (juce::Colours::white.interpolatedWith(juce::Colours::transparentWhite, 0.95f));
        g.fillPath (p);
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
    
    void mouseUp  (const juce::MouseEvent& e) override
    {
        alignPosition ();
        informParent ();
    }
    
    void parentSizeChanged() override
    {
        
        auto newBounds = getBoundsInParent();
        
        constrainer.checkBounds (newBounds, getBoundsInParent(),
                                 getParentComponent()->getLocalBounds(),
                                 false, false, false, false);
        setBounds (newBounds);
        
        constrainer.setMinimumOnscreenAmounts ( getHeight(), getWidth(), getHeight(), getWidth() );
        
        repaint ();
        
        informParent ();
    }
    
    void toggleEdit (bool isInEditMode)
    {
        setVisible (!isInEditMode);
    }
    
private:
    
    juce::ComponentBoundsConstrainer constrainer;
    juce::ComponentDragger dragger;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DraggableCursor)
};

template <typename ValueType>
class MatrixFieldArea : public MyTextEditor
{
public:
    MatrixFieldArea (juce::String param)
    {
        setText (param);
    }
    
    bool contains (juce::Point <ValueType> point)
    {
        juce::Rectangle<ValueType> checkArea = getBoundsInParent();
        checkArea.setWidth (checkArea.getWidth() + 1);
        checkArea.setHeight (checkArea.getHeight() + 1);
        return checkArea. contains (point);
    }
    
    //TODO deprecated in my current approach
    ValueType getDistanceFromSnapshotPosition (juce::Point <ValueType> other)
    {
        return representedSnapshotPosition.getDistanceFrom (other);
    }
    
    std::vector <double> & getRepresentedSnapshotKnobPositions ()
    {
        return representedSnapshotKnobPositions;
    }
    
    void mouseDown (const juce::MouseEvent& event) override
    {
        demandCursorMove ();
    }
    
    void setSelected ()
    {
        isSelected = true;
        repaint ();
    }
    
    void setUnselected ()
    {
        isSelected = false;
        repaint ();
    }
    
    std::function <void ()> demandCursorMove;
    
    void paint (juce::Graphics& g) override
    {
        if (isInEditMode)
            return;
        auto bounds = getLocalBounds ();
        
        if (isSelected)
        {
            g.setColour (juce::Colours::white.interpolatedWith(juce::Colours::transparentWhite, 0.9f));
            g.fillAll ();
        }
        
        g.setColour (findColour (OtherLookAndFeel::Colours::whiteText));
        g.setFont (16.0f);
        g.drawText  (name, bounds, juce::Justification::centred);
        
        
    }
    
    void setRepresentedSnapshotKnobPositions (std::vector <double> newPositions)
    {
        representedSnapshotKnobPositions = newPositions;
        for (auto position : representedSnapshotKnobPositions)
        {
            std::cout << "jo" << position << std::endl;
        }
    }
    
    
    
private:
    //TODO deprecated in my current approach
    juce::Point <ValueType> representedSnapshotPosition;
    std::vector <double> representedSnapshotKnobPositions;
    
    bool isSelected = false;
    
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
//            currentCursorPositionToInnerField = cursorField.getBoundsInParent (). getCentre ();
//            std::cout << cursorField.getBoundsInParent (). getX () << std::endl;
//            std::cout << cursorField.getBounds (). getX () << std::endl;
            for (auto area : matrixFieldAreas)
            {
                area->setUnselected ();
            }
            MatrixFieldArea<int> * currentField = getCurrentlySelectedArea ();
            currentField->setSelected ();
            updateKnobs ();
        };
        
        cursorField.alignPosition = [&]
        {
            double criticalOffset = (double) std::min (this->getWidth (), this->getHeight ()) / 30.0f;
            MatrixFieldArea<int> * currentField = getCurrentlySelectedArea ();
            
            if (cursorField.getBoundsInParent ().getCentre ().getDistanceFrom (currentField->getBoundsInParent (). getCentre ()) > criticalOffset )
                return;
            
            cursorField.setBounds (currentField -> getBoundsInParent ());
            cursorField.informParent ();
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
        juce::Colour backgroundColour = getLookAndFeel ().findColour (OtherLookAndFeel::Colours::archiveDarkGrey);
        juce::Colour greyTone = juce::Colour(0x353130).interpolatedWith (juce::Colours::white, 0.4f);
        juce::Colour whiteTone = juce::Colour(0x353130).interpolatedWith (juce::Colours::white, 0.5f);
        juce::Colour shadowTone = juce::Colour::fromRGBA(0, 0, 0, 40);
        
        juce::Path p;
        p.addRoundedRectangle (bounds, roundedRectangleCornerSize);
        
        
        g.setColour (backgroundColour);
        
        g.fillPath (p);
        
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
    
    std::vector <double> produceNewKnobValues ()
    {
        
        MatrixFieldArea<int> * currentArea = getCurrentlySelectedArea ();
        
        //getting 3 neighbours of current cursorPosition and currentArea ->works ->blackbox this :)
        auto getNearestNeighbourRectangle =
        [&] () -> std::vector <MatrixFieldArea<int> *>
        {
            std::vector <std::vector <int>> indexes;
            int currentIndex = -1;
            for ( int i = 0; i < nrRows * nrColumns; i++)
            {
                if (matrixFieldAreas.at (i) == currentArea)
                {
                    currentIndex = i;
                }
            }
            if (currentIndex == -1)
            {
                try
                {
                    throw 21;
                }
                catch (int e)
                {
                    std::cout << "An exception occurred. Exception Nr. " << e << '\n';
                }
            }
            //leftupper
            std::vector <int> leftUpper;
            leftUpper.push_back (currentIndex - 1);
            leftUpper.push_back (currentIndex - nrColumns);
            leftUpper.push_back (currentIndex - nrColumns - 1);
            //rightupper
            std::vector <int> rightUpper;
            rightUpper.push_back (currentIndex + 1);
            rightUpper.push_back (currentIndex - nrColumns);
            rightUpper.push_back (currentIndex - nrColumns + 1);
            //leftlower
            std::vector <int> leftLower;
            leftLower.push_back (currentIndex - 1);
            leftLower.push_back (currentIndex + nrColumns);
            leftLower.push_back (currentIndex + nrColumns - 1);
            //rightlower
            std::vector <int> rightLower;
            rightLower.push_back (currentIndex + 1);
            rightLower.push_back (currentIndex + nrColumns);
            rightLower.push_back (currentIndex + nrColumns + 1);
            //push 'em
            if ( std::all_of (leftUpper.begin (), leftUpper.end (), [&](int i){return i >= 0 && i < nrRows * nrColumns;} ) )
                indexes.push_back (leftUpper);
            if ( std::all_of (rightUpper.begin (), rightUpper.end (), [&](int i){return i >= 0 && i < nrRows * nrColumns;} ) )
                indexes.push_back (rightUpper);
            if ( std::all_of (leftLower.begin (), leftLower.end (), [&](int i){return i >= 0 && i < nrRows * nrColumns;} ) )
                indexes.push_back (leftLower);
            if ( std::all_of (rightLower.begin (), rightLower.end (), [&](int i){return i >= 0 && i < nrRows * nrColumns;} ) )
                indexes.push_back (rightLower);
            
            if (indexes.size() < 1)
            {
                try
                {
                    throw 22;
                }
                catch (int e)
                {
                    std::cout << "An exception occurred. Exception Nr. " << e << '\n';
                }
            }
            
            std::vector <MatrixFieldArea<int> *> ret;
            int lowestSummedDistance = std::numeric_limits<int>::max ();
            for (auto vec : indexes)
            {
                int summedDistance = 0;
                std::vector <MatrixFieldArea<int> *> possibleNeighbour;
                
                for (int i : vec)
                {
                    possibleNeighbour.push_back (matrixFieldAreas.at (i));
                }
                
                for (auto area : possibleNeighbour)
                {
                    summedDistance += cursorField.getBoundsInParent (). getCentre (). getDistanceFrom (area ->getBoundsInParent (). getCentre ());
                }
                
                if (summedDistance < lowestSummedDistance)
                {
                    lowestSummedDistance = summedDistance;
                    ret = possibleNeighbour;
                }
                
            }
            
            return ret;
        };
        
        std::vector <MatrixFieldArea<int> *> neighbours = getNearestNeighbourRectangle ();
        //add the 4th in the group
        neighbours.push_back (currentArea);
        std::vector <double> aVector;
        for (auto area : neighbours)
        {
            aVector.push_back (1);
            aVector.push_back (area->getBoundsInParent (). getCentre (). getX ());
            aVector.push_back (area->getBoundsInParent (). getCentre (). getY ());
            aVector.push_back (area->getBoundsInParent (). getCentre (). getX () * area->getBoundsInParent (). getCentre (). getY ());
        }
        
        std::vector<std::size_t> shape = { 4, 4 };
        
        xt::xarray <double> A = xt::adapt (aVector, shape);
        //        produces something similar to the following:
        //        xt::xarray<double> A
        //        {
        //            {1, x1, y1, x1 * y1},
        //            {1, x2, y2, x2 * y2},
        //            {1, x3, y3, x3 * y3},
        //            {1, x4, y4, x4 * y4}
        //        };
        
        std::vector <double> newPositions;
        int nrSpecifiedKnobs = 0;
        for (auto area : neighbours)
        {
            //TODO
            int bla = area->getRepresentedSnapshotKnobPositions (). size ();
            nrSpecifiedKnobs = std::max (nrSpecifiedKnobs, bla );

            //            nrSpecifiedKnobs = std::max (nrSpecifiedKnobs, area->getRepresentedSnapshotKnobPositions (). size () );
        }
        
        for (int i = 0; i < nrSpecifiedKnobs; i++)
        {
            
            //Werte für alle Knöpfe errechnen!
            for (auto area : neighbours)
            {
                while (area->getRepresentedSnapshotKnobPositions ().size () < nrSpecifiedKnobs)
                {
                    area->getRepresentedSnapshotKnobPositions ().push_back (0.0);
                    std::cout << "reached this point" << std::endl;
                    std::cout << area->getRepresentedSnapshotKnobPositions ().size () << std::endl;
                }
            }
            
            
            
            double f1 = neighbours [0] ->getRepresentedSnapshotKnobPositions () [i];
            double f2 = neighbours [1] ->getRepresentedSnapshotKnobPositions () [i];
            double f3 = neighbours [2] ->getRepresentedSnapshotKnobPositions () [i];
            double f4 = neighbours [3] ->getRepresentedSnapshotKnobPositions () [i];
            
            xt::xarray<double> B
            {f1, f2, f3, f4};
            
            auto v = xt::linalg::solve (A, B);
            
            double a = v[0];
            double b = v[1];
            double c = v[2];
            double d = v[3];
            
            double x = cursorField.getBoundsInParent (). getCentre (). getX ();
            double y = cursorField.getBoundsInParent (). getCentre (). getY ();
            
            double ret = a + b*x + c*y + d*x*y;
            
            newPositions.push_back (ret);
        }
        return newPositions;
    }
    
    void manipulateValues (std::vector <double> knobValuesAtCurrentPosition)
    {
        
        MatrixFieldArea<int> * currentArea = getCurrentlySelectedArea ();
        
        //getting 3 neighbours next to currentCursorPosition and currentArea ->works ->blackbox this :)
        auto getNearestNeighbourRectangle =
        [&] () -> std::vector <MatrixFieldArea<int> *>
        {
            std::vector <std::vector <int>> indexes;
            int currentIndex = -1;
            for ( int i = 0; i < nrRows * nrColumns; i++)
            {
                if (matrixFieldAreas.at (i) == currentArea)
                {
                    currentIndex = i;
                }
            }
            if (currentIndex == -1)
            {
                try
                {
                    throw 21;
                }
                catch (int e)
                {
                    std::cout << "An exception occurred. Exception Nr. " << e << '\n';
                }
            }
            //leftupper
            std::vector <int> leftUpper;
            leftUpper.push_back (currentIndex - 1);
            leftUpper.push_back (currentIndex - nrColumns);
            leftUpper.push_back (currentIndex - nrColumns - 1);
            //rightupper
            std::vector <int> rightUpper;
            rightUpper.push_back (currentIndex + 1);
            rightUpper.push_back (currentIndex - nrColumns);
            rightUpper.push_back (currentIndex - nrColumns + 1);
            //leftlower
            std::vector <int> leftLower;
            leftLower.push_back (currentIndex - 1);
            leftLower.push_back (currentIndex + nrColumns);
            leftLower.push_back (currentIndex + nrColumns - 1);
            //rightlower
            std::vector <int> rightLower;
            rightLower.push_back (currentIndex + 1);
            rightLower.push_back (currentIndex + nrColumns);
            rightLower.push_back (currentIndex + nrColumns + 1);
            //push 'em
            if ( std::all_of (leftUpper.begin (), leftUpper.end (), [&](int i){return i >= 0 && i < nrRows * nrColumns;} ) )
                indexes.push_back (leftUpper);
            if ( std::all_of (rightUpper.begin (), rightUpper.end (), [&](int i){return i >= 0 && i < nrRows * nrColumns;} ) )
                indexes.push_back (rightUpper);
            if ( std::all_of (leftLower.begin (), leftLower.end (), [&](int i){return i >= 0 && i < nrRows * nrColumns;} ) )
                indexes.push_back (leftLower);
            if ( std::all_of (rightLower.begin (), rightLower.end (), [&](int i){return i >= 0 && i < nrRows * nrColumns;} ) )
                indexes.push_back (rightLower);
            
            if (indexes.size() < 1)
            {
                try
                {
                    throw 22;
                }
                catch (int e)
                {
                    std::cout << "An exception occurred. Exception Nr. " << e << '\n';
                }
            }
            
            std::vector <MatrixFieldArea<int> *> ret;
            int lowestSummedDistance = std::numeric_limits<int>::max ();
            for (auto vec : indexes)
            {
                int summedDistance = 0;
                std::vector <MatrixFieldArea<int> *> possibleNeighbour;
                
                for (int i : vec)
                {
                    possibleNeighbour.push_back (matrixFieldAreas.at (i));
                }
                
                for (auto area : possibleNeighbour)
                {
                    summedDistance += cursorField.getBoundsInParent (). getCentre (). getDistanceFrom (area ->getBoundsInParent (). getCentre ());
                }
                
                if (summedDistance < lowestSummedDistance)
                {
                    lowestSummedDistance = summedDistance;
                    ret = possibleNeighbour;
                }
                
            }
            
            return ret;
        };
        
        //knowing the neighbours we want to use as reference and the cursorPosition/knobPositions, we can deduct the new knobpositions for the center of currentArea
        std::vector <MatrixFieldArea<int> *> neighbours = getNearestNeighbourRectangle ();
        
        std::vector <double> aVector;
        aVector.push_back (1);
        aVector.push_back (cursorField.getBoundsInParent (). getCentre (). getX ());
        aVector.push_back (cursorField.getBoundsInParent (). getCentre (). getY ());
        aVector.push_back (cursorField.getBoundsInParent (). getCentre (). getX () * cursorField.getBoundsInParent (). getCentre (). getY ());
        for (auto area : neighbours)
        {
            aVector.push_back (1);
            aVector.push_back (area->getBoundsInParent (). getCentre (). getX ());
            aVector.push_back (area->getBoundsInParent (). getCentre (). getY ());
            aVector.push_back (area->getBoundsInParent (). getCentre (). getX () * area->getBoundsInParent (). getCentre (). getY ());
        }
        
        std::vector<std::size_t> shape = { 4, 4 };
        
        xt::xarray <double> A = xt::adapt (aVector, shape);
        //        produces something similar to the following:
        //        xt::xarray<double> A
        //        {
        //            {1, x1, y1, x1 * y1},
        //            {1, x2, y2, x2 * y2},
        //            {1, x3, y3, x3 * y3},
        //            {1, x4, y4, x4 * y4}
        //        };
        
        std::vector <double> newPositions;
        for (int i = 0; i < knobValuesAtCurrentPosition.size (); i++)
        {
            
            //Werte für alle Knöpfe errechnen!
            double f1 = knobValuesAtCurrentPosition [i];
            for (auto area : neighbours)
            {
                while (area->getRepresentedSnapshotKnobPositions ().size () < knobValuesAtCurrentPosition.size ())
                {
                    area->getRepresentedSnapshotKnobPositions ().push_back (0.0);
                    std::cout << "reached this point" << std::endl;
                    std::cout << area->getRepresentedSnapshotKnobPositions ().size () << std::endl;
                }
            }
            
            
            
            double f2 = neighbours [0] ->getRepresentedSnapshotKnobPositions () [i];
            double f3 = neighbours [1] ->getRepresentedSnapshotKnobPositions () [i];
            double f4 = neighbours [2] ->getRepresentedSnapshotKnobPositions () [i];
            
            xt::xarray<double> B
            {f1, f2, f3, f4};
            
            auto v = xt::linalg::solve (A, B);
            
            double a = v[0];
            double b = v[1];
            double c = v[2];
            double d = v[3];
            
            double x = currentArea->getBoundsInParent (). getCentre (). getX ();
            double y = currentArea->getBoundsInParent (). getCentre (). getY ();
            
            double ret = a + b*x + c*y + d*x*y;
            
            newPositions.push_back (ret);
        }
        
        currentArea->setRepresentedSnapshotKnobPositions (newPositions);
        
        return;
    }
    
    MatrixFieldArea<int> * getCurrentlySelectedArea()
    {
        //tested: seems to always have exactly one solution as it's supposed to be
        
        for ( int i = 0; i < nrRows * nrColumns; i++)
        {
            if (matrixFieldAreas.at (i) -> contains (cursorField.getBoundsInParent (). getCentre ()))
            {
                return matrixFieldAreas.at (i);
            }
        }
        
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
    
    void toggleEdit (bool isInEditMode)
    {
        this->isInEditMode = isInEditMode;
        for (auto i : matrixFieldAreas)
        {
            i->toggleEdit ( isInEditMode );
        }
        cursorField.toggleEdit ( isInEditMode );
        repaint ();
    }
    
    std::function <void ()> updateKnobs;
    
private:
    void nrMatrixFieldAreasChanged ()
    {
        innerField.removeAllChildren ();
        
        if (matrixFieldAreas.size () < nrRows * nrColumns)
        {
            int nrMissingAreas = nrRows * nrColumns - (int) matrixFieldAreas.size ();
            for (int i = 0; i < nrMissingAreas; i++)
            {
                MatrixFieldArea<int> * temp = new MatrixFieldArea<int> ("new mode");
                
                
                matrixFieldAreas.push_back (temp);
            }
        }
        //no else here: we want to keep "hidden" Fields, just keep the sizes right
        
        for (auto i : matrixFieldAreas)
        {
            innerField.addAndMakeVisible(i);
            i->demandCursorMove = [i, this]
            {
                std::cout << i->getBoundsInParent ().getX () << i->getBoundsInParent ().getY () << std::endl;
                cursorField.setBounds ( i->getBoundsInParent () );
                cursorField.informParent ();
            };
        }
        
        resizeMatrixFieldAreas();
    }
    
    void resizeMatrixFieldAreas ()
    {
        //distribute areas !!!within innerField!!!, so they comply with position of cursorfield
        
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
                
                matrixFieldAreas.at (indexOfArea) -> setBounds (juce::Rectangle<int> ( (int) left,
                                                                                       (int) top,
                                                                                       (int) (right - left),
                                                                                       (int) (bottom - top))
                                                                 );
            }
        }
    }
    
    Component innerField; //innerField is needed to restrict our draggableCursor correctly, it is also where the MatrixFieldAreas lie within
    
    DraggableCursor cursorField;
    
    juce::Rectangle<int> currentAnimatedBounds {}; //actually not needed, we could just use innerField's bounds
    
    std::vector <MatrixFieldArea<int> *> matrixFieldAreas;
    
    int nrRows = 4;
    int nrColumns = 3;
    
    bool isInEditMode {false};
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MatrixField)
};



