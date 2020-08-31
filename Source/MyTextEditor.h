/*
  ==============================================================================

    MyTextEditor.h
    Created: 22 Jul 2020 4:01:39pm
    Author:  expert239

  ==============================================================================
*/

#pragma once

class MyTextEditorEditPart : public juce::TextEditor
{
public:
    MyTextEditorEditPart ()
    {
        setJustification (juce::Justification::centred);
        setWantsKeyboardFocus (true);
        setCaretVisible (true);
        setMultiLine (false);
    }
    void paintOverChildren (juce::Graphics& g) override {}
    void paint (juce::Graphics& g) override
    {
        auto bounds = getLocalBounds ().toFloat ();
        if (hasKeyboardFocus (true))
        {
            g.setColour (juce::Colours::pink.withAlpha (0.5f));
        }
        else
            g.setColour (juce::Colours::pink.withAlpha (0.3f));
        g.fillRect (bounds);
    }
private:
};

class MyTextEditor : public juce::Component
{
public:
    
    MyTextEditor ()
    {
        addChildComponent (editor);
        editor.setVisible (false);
        editor.onTextChange = [&]
        {
            name = editor.getText ();
        };
    }
    
    void paint (juce::Graphics& g) override
    {
        if (isInEditMode)
            return;
        auto bounds = getLocalBounds ();
        g.setColour (juce::Colours::white);
        g.drawText  (name, bounds, juce::Justification::centred);
    }
    
    void toggleEdit (bool isInEditMode)
    {
        this->isInEditMode = isInEditMode;
        editor.setVisible (isInEditMode);
        std::cout << editor.isVisible () << std::endl;
        std::cout << name << std::endl;
        repaint ();
    }
    void resized () override
    {
        auto bounds = getLocalBounds ();
        editor.setBounds (bounds);
    }
    
    void setText (juce::String text)
    {
        editor.setText (text);
        name = text;
    }
    
private:
    MyTextEditorEditPart editor {};
    juce::String name = "name";
    
    bool isInEditMode = false;
};


