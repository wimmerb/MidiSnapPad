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
        setFont (16.0f);
        setJustification (juce::Justification::centredTop);
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
            g.setColour (getLookAndFeel ().findColour (OtherLookAndFeel::Colours::brightblue).withAlpha (0.5f));
            //g.setColour (juce::Colours::pink.withAlpha (0.5f));
        }
        else
        {
            g.setColour (getLookAndFeel ().findColour (OtherLookAndFeel::Colours::brightblue).withAlpha (0.3f));
            //g.setColour (juce::Colours::pink.withAlpha (0.3f));
        }
        
        g.fillRect (bounds);
    }
private:
};

class MyTextEditor : public juce::Component
{
public:
    
    MyTextEditor (bool isCentred)
    {
        this->isCentred = isCentred;
        addChildComponent (editor);
        editor.onTextChange = [&]
        {
            name = editor.getText ();
        };
    }
    
    MyTextEditor () : MyTextEditor (true) {}
    
    void paint (juce::Graphics& g) override
    {
        if (isInEditMode)
            return;
        auto bounds = getLocalBounds ();
        
        g.setColour (findColour (OtherLookAndFeel::Colours::whiteText));
        g.setFont (16.0f);
        if (isCentred)
            g.drawText  (name, bounds, juce::Justification::centred);
        else
            g.drawText  (name, bounds, juce::Justification::centredTop);
    }
    
    void toggleEdit (bool isInEditMode)
    {
        this->setVisible (true);
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
    
    juce::String name = "name";
    
    bool isInEditMode = false;
    
private:
    MyTextEditorEditPart editor {};
    bool isCentred = true;
    
};


