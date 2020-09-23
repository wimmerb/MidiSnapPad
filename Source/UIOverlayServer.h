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
        setFont (16.0f);
        setText (initialText);

        juce::String bla {"joo"};
        initialText = bla;
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
        DBG ("texteditor");
        DBG (textEditor.getText ());
        addAndMakeVisible (textEditor);
        addAndMakeVisible (okButton);
        DBG ("afterEdit");
        DBG (initialText);
    }
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


class UIOverlayServer : public juce::Component
{
public:
    UIOverlayServer ()
    {
        setAlwaysOnTop (true);
        setVisible (false);
        juce::String bla {"j"};
        textEditorOverlay = new UITextEditorOverlay (bla);
    }
    void showTextEditorOverlay (MyTextEditor & callingTextEditor)
    {
        delete (textEditorOverlay);
        textEditorOverlay = new UITextEditorOverlay (callingTextEditor.name);
        setVisible (true);
        addAndMakeVisible (textEditorOverlay);
        repaint ();
    }
    void paintOverChildren (juce::Graphics& g) override {}
    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::black.withAlpha (0.8f));
    }
    void resized() override
    {
        DBG ("here");
        auto bounds = getLocalBounds ();
        bounds.removeFromBottom (bounds.getHeight () / 2);
        bounds.removeFromTop (bounds.getHeight () * 0.2f);
        textEditorOverlay->setBounds (bounds.reduced (8, 4));
    }
private:
    UITextEditorOverlay * textEditorOverlay;
};

