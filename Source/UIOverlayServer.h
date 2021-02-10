/*
  ==============================================================================

    UIOverlayServer.h
    Created: 22 Sep 2020 12:05:51pm
    Author:  expert239

  ==============================================================================
*/

#pragma once

#include "OtherLookAndFeel.h"
#include "MyTextEditor.h"

class MyUIOverlayTextEditor : public juce::TextEditor
{
public:
    MyUIOverlayTextEditor (juce::String & initialText) : juce::TextEditor {initialText}
    {
        DBG ("texteditor!");
        setFont (16.0f);
        setText (initialText);
        setJustification (juce::Justification::topLeft);
        setWantsKeyboardFocus (true);
        setCaretVisible (true);
        setMultiLine (false);
    }

    void paintOverChildren (juce::Graphics& g) override {}

    void paint (juce::Graphics& g) override
    {
        auto bounds = getLocalBounds ().reduced (4,4).toFloat ();
        g.setColour (getLookAndFeel ().findColour (OtherLookAndFeel::Colours::archiveDarkGrey));
        g.fillRect (bounds);
        g.setColour (getLookAndFeel ().findColour (OtherLookAndFeel::Colours::archiveOutline));
        g.drawRect (bounds, 1.0f);
    }
private:
};

class MyButton : public juce::Component
{
public:
    MyButton (juce::String initialText ) : text (initialText){}
    void mouseDown(const juce::MouseEvent &event) override
    {
        alertParent ();
    }
    std::function<void ()> alertParent;
    void paint(juce::Graphics &g) override
    {
        auto bounds = getLocalBounds(). reduced (4, 4);
        g.setColour (getLookAndFeel().findColour (OtherLookAndFeel::Colours::whiteText));
        g.drawText (text, bounds, juce::Justification::right);
    }
private:
    juce::String text;
};

class UITextEditorOverlay : public juce::Component
{
public:
    UITextEditorOverlay (juce::String & initialText) : textEditor (initialText), okButton ("Done")
    {
        okButton.alertParent = [&]
                {
                        setVisible (false);
                        alertParent (textEditor.getText ());
                };

        addAndMakeVisible (textEditor);
        addAndMakeVisible (okButton);
    }
    std::function <void (juce::String text)> alertParent;
    void paint(juce::Graphics &g) override
    {
        auto bounds = getLocalBounds ().reduced (4,4);
        g.setColour (getLookAndFeel ().findColour (OtherLookAndFeel::Colours::archiveBackground));
        g.fillRect (bounds);
        g.setColour (getLookAndFeel ().findColour (OtherLookAndFeel::Colours::archiveOutline));
        g.drawRect (bounds, 1.0f);
    }
    void resized() override
    {
        auto bounds = getLocalBounds (). reduced (4,4);
        okButton.setBounds (bounds.removeFromBottom(std::max (0.2f*bounds.getHeight (), 20.0f)).removeFromRight (std::max (0.3f * bounds.getWidth (), 60.0f)));
        textEditor.setBounds (bounds);
    }
private:
    MyButton okButton;
    MyUIOverlayTextEditor textEditor;
};

class ScrollableString  : public juce::Component, juce::AnimatedPosition<juce::AnimatedPositionBehaviours::ContinuousWithMomentum>::Listener
{
public:
    ScrollableString (const juce::StringArray& itemsToAdd, int firstItemID, int currentSelection):
            itemList (itemsToAdd),
            begin (firstItemID),
            value (currentSelection)
    {
        DBG (itemsToAdd.size ());
        DBG ("asdg");
        DBG (itemList.size ());
        //setOpaque(true);
        //-1 to cope with .size
        this->end   = itemsToAdd.size () + begin - 1;
        this->value = std::max (begin - 1, std::min (end, value));

        animator.addListener(this);
        animator.setLimits(juce::Range<double> (begin, end));
        animator.behaviour.setFriction (0.1);
        animator.behaviour.setMinimumVelocity (1.0);
        animator.setPosition (this->value);


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

    juce::String getText ()
    {
        return assignValueText (value);
    }

    void paint (juce::Graphics& g) override
    {
        DBG (itemList.size ());
        DBG ("okok");
        auto bounds = getLocalBounds ().reduced (4,4).toFloat ();
        float cornerSize = 3.0f;

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

private:

    juce::String assignValueText (int value)
    {
        DBG (itemList.size ());
        if (value > end || value < begin)
            return juce::String ("");
        else
            return itemList [value - begin];
    }
    juce::StringArray itemList;
    int begin;
    int end;
    int value;

    juce::AnimatedPosition <juce::AnimatedPositionBehaviours::ContinuousWithMomentum> animator;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScrollableString)
};


class UISelectorOverlay : public juce::Component
{
public:
    UISelectorOverlay (const juce::StringArray& itemsToAdd, int firstItemID, int currentSelection) :
                        okButton ("Done"),
                        stringScroller (itemsToAdd, firstItemID, currentSelection)
    {
//        calling function should do something like this (midiOutputList is this UISelector):
//        midiOutputList.onChange = [this] { setMidiOutput (midiOutputList.getSelectedItemIndex()); };
        okButton.alertParent = [&]
        {
            setVisible (false);
            DBG ("jojo");
            alertParent (stringScroller.getText ());
        };
        addAndMakeVisible (okButton);
        addAndMakeVisible (stringScroller);


    }

//    now: copycat from scrollableNumber, maybe find a nicer way to transform...
    std::function <void (juce::String text)> alertParent;

    void resized() override
    {
        auto bounds = getLocalBounds (). reduced (4,4);
        okButton.setBounds (bounds.removeFromBottom(std::max (0.2f*bounds.getHeight (), 20.0f)).removeFromRight (std::max (0.3f * bounds.getWidth (), 60.0f)));
        stringScroller.setBounds (bounds);
    }

    void paint(juce::Graphics &g) override
    {
        auto bounds = getLocalBounds ().reduced (4,4);
        g.setColour (getLookAndFeel ().findColour (OtherLookAndFeel::Colours::archiveBackground));
        g.fillRect (bounds);
        g.setColour (getLookAndFeel ().findColour (OtherLookAndFeel::Colours::archiveOutline));
        g.drawRect (bounds, 1.0f);
    }

private:
    MyButton okButton;
    ScrollableString stringScroller;
};

class UIOverlayServer : public juce::Component
{
public:
    UIOverlayServer ()
    {
        setAlwaysOnTop (true);
        setVisible (false);
    }
    void showTextEditorOverlay (MyTextEditor & callingTextEditor)
    {
        DBG ("do TextEditorOverlay");
        // delete (textEditorOverlay);
        textEditorOverlay = new UITextEditorOverlay (callingTextEditor.name);
        textEditorOverlay->alertParent = [&] (juce::String text)
                {
                    setVisible (false);
                    callingTextEditor.setText (text);
                };
        setVisible (true);
        addAndMakeVisible (textEditorOverlay);
        resized ();
        repaint ();
        DBG ("did TextEditorOverlay");
    }
    void showSelectorOverlay (const juce::StringArray& itemsToAdd, int firstItemID, int currentSelection)
    {
        DBG ("do SelectorOverlay");
        selectorOverlay = new UISelectorOverlay (itemsToAdd, firstItemID, currentSelection);
        selectorOverlay->alertParent = [&] (juce::String text)
        {
            setVisible (false);
//            TODO callingTextEditor.setText (text);
        };
        setVisible (true);
        addAndMakeVisible (selectorOverlay);
        resized ();
        repaint ();
        DBG ("did SelectorOverlay");
    }

    void paintOverChildren (juce::Graphics& g) override {}
    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::black.withAlpha (0.8f));
    }
    void resized() override
    {
        auto bounds = getLocalBounds ();
        bounds.removeFromBottom (bounds.getHeight () / 2);
        bounds.removeFromTop (bounds.getHeight () * 0.2f);

        if (textEditorOverlay != nullptr)
            textEditorOverlay->setBounds (bounds.reduced (8, 4));
        if (selectorOverlay != nullptr)
            selectorOverlay->setBounds (bounds.reduced (8, 4));

    }

    void mouseDown (const juce::MouseEvent &event) override
    {
        //TODO CLOSE ALL WINDOWS
//        if (textEditorOverlay != nullptr)
//            //close texteditor
//        if (selectorOverlay != nullptr)
//            //close other
    }

    ~UIOverlayServer ()
    {
        delete (textEditorOverlay);
        delete (selectorOverlay);
    }
private:
    UITextEditorOverlay * textEditorOverlay = nullptr;
    UISelectorOverlay * selectorOverlay = nullptr;
};

