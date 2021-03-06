#include "MainComponent.h"
//==============================================================================
MainComponent::MainComponent()
{
    getAllSystemInfo();
    this->setLookAndFeel (&otherLookAndFeel);

//    TODO MAKE USE OF THE SELECTOR OVERLAY
//    juce::StringArray bla;
//    bla.add("jo");
//    bla.add("man");
//    bla.add ("jahter");
//    uiOverlay.showSelectorOverlay(bla, 0, 1);

    addChildComponent (uiOverlay);
    #if !JUCE_IOS
    openGLContext.setOpenGLVersionRequired (juce::OpenGLContext::openGL3_2);
    openGLContext.setContinuousRepainting (false);
    openGLContext.setComponentPaintingEnabled(true);
    openGLContext.attachTo (*this);
    #endif

    matrix.updateKnobs = [&]
    {
        std::vector <double> newValues = matrix.produceNewKnobValues();
        for (int i = 0; i <  newValues.size(); i++)
        {
            knobs[i] -> setValue (newValues [i]);
        }
    };
    //make sure knobs are inited
    manipulateMatrixValues();
    addAndMakeVisible (matrix);
    
    //IDEE: algebraisches Zeug in dieser Komponente. eher nein.
    editModeToggle = new juce::Slider ();
    editModeToggle->setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
    editModeToggle->setTextBoxStyle (juce::Slider::NoTextBox, true, 0, 0);
    editModeToggle->setRange (juce::Range<double>(0.0, 1.0), 1.0);
    addAndMakeVisible(*editModeToggle);
    
    
    //TODO editmode speichern und so auch übergeben.
    editModeToggle->onValueChange = [&]
    {
        bool isInEditMode = (bool) editModeToggle->getValueObject ().getValue ();
        
        std::cout << "gi" << std::endl;
        std::cout << isInEditMode << std::endl;
        std::cout << "gi" << std::endl;
        
        matrix.toggleEdit (isInEditMode);
        for (auto i : knobs)
        {
            i->toggleEdit (isInEditMode);
        }
        metaSettings->toggleEdit(isInEditMode);
    };

    midiOut = std::move (juce::MidiOutput::openDevice (juce::MidiOutput::getDefaultDevice().identifier));

    metaSettings = std::make_unique<MetaSettingComponent> (midiOut);
    
    for (int i = 0; i < 8; i++)
    {
        pushBackNewKnob();
    }

    auto list = juce::MidiOutput::getAvailableDevices();
    auto newOutput = list[1];
    midiOut = juce::MidiOutput::openDevice (newOutput.identifier);
    
    addAndMakeVisible (*metaSettings);



    setSize (436, 800);
    resized();
    editModeToggle->onValueChange ();
}




void MainComponent::pushBackNewKnob ()
{
    MidiKnob * tmp = new MidiKnob (midiOut, uiOverlay);

    knobs.push_back (tmp);
    addAndMakeVisible (*tmp);
    tmp->onValueManipulated = [&]
    {
        manipulateMatrixValues ();
    };
}

void MainComponent::manipulateMatrixValues ()
{
    std::vector <double> knobValuesAtCurrentPosition;
    for (auto knob : knobs)
    {
        knobValuesAtCurrentPosition.push_back ( knob->getValue () );
    }
    matrix.manipulateValues(knobValuesAtCurrentPosition);
}

MainComponent::~MainComponent()
{
    #if !JUCE_IOS
    openGLContext.detach();
    #endif

    for (auto i : knobs)
    {
        delete (i);
    }
    knobs.clear ();
    //delete (midiOut);
    
    delete (editModeToggle);
    
    setLookAndFeel (nullptr);
    
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel ().findColour (OtherLookAndFeel::Colours::generalBackground));
    
//    g.setColour (getLookAndFeel ().findColour (OtherLookAndFeel::Colours::archiveBackground));
//    g.fillRoundedRectangle (backgroundArea.toFloat (), 20.0f);
//
//    g.setColour (juce::Colour::fromRGB (90, 90, 90));
//    g.drawRoundedRectangle(backgroundArea.toFloat (), 20.0f, 0.5f);
    auto currentIndicatedBounds = backgroundArea;
    auto cs = 20.0f;
    auto cs45 = 20.0f * 0.45f;
    
    juce::Path p;
    p.clear ();
    
    p.startNewSubPath (currentIndicatedBounds.getX (), currentIndicatedBounds.getY ());
    p.lineTo (currentIndicatedBounds.getX (), currentIndicatedBounds.getBottom () - cs);
    p.cubicTo (currentIndicatedBounds.getBottomLeft ().getX (), currentIndicatedBounds.getBottomLeft ().getY () - cs + cs45,
               currentIndicatedBounds.getBottomLeft ().getX () + cs45, currentIndicatedBounds.getBottomLeft ().getY (),
               currentIndicatedBounds.getBottomLeft ().getX () + cs, currentIndicatedBounds.getBottomLeft ().getY ());
    p.lineTo (currentIndicatedBounds.getBottomRight ().getX () - cs, currentIndicatedBounds.getBottomRight (). getY ());
    p.cubicTo (currentIndicatedBounds.getBottomRight ().getX () - cs + cs45, currentIndicatedBounds.getBottomRight ().getY (),
               currentIndicatedBounds.getBottomRight ().getX (), currentIndicatedBounds.getBottomRight ().getY () - cs45,
               currentIndicatedBounds.getBottomRight ().getX (), currentIndicatedBounds.getBottomRight ().getY () - cs);
    p.lineTo (currentIndicatedBounds.getTopRight ().getX (), currentIndicatedBounds.getTopRight (). getY ());
    
    g.setColour (getLookAndFeel ().findColour (OtherLookAndFeel::Colours::archiveBackground));
    g.fillPath(p);
    g.setColour (findColour (OtherLookAndFeel::Colours::archiveOutline));
    g.strokePath (p, juce::PathStrokeType (1.0f, juce::PathStrokeType::JointStyle::beveled, juce::PathStrokeType::EndCapStyle::square));
//
//
//
//    g.setColour (juce::Colour::fromRGB (90, 90, 90));
//    g.drawRoundedRectangle(bounds.toFloat (), 20.0f, 0.5f);
    
    

    /*g.setFont (juce::Font (16.0f));
    g.setColour (juce::Colours::white);
    g.drawText ("Hello World!", getLocalBounds(), juce::Justification::centred, true);*/
}

void MainComponent::resized ()
{
    uiOverlay.setBounds (getBounds ());
    auto area = getLocalBounds (). reduced (8);
    
    //TODO textbox neben midiOutputList bzw. midiOutputList als eigenes Zeichen...

    
    
    metaSettings->setBounds (area. removeFromTop(110));
    
    auto background = area;
    background.removeFromTop (60);
    this->backgroundArea = background.removeFromTop(300 + 300 + 10). reduced (4,0);
    
    editModeToggle->setBounds (area.removeFromTop (60));
    auto lowerarea2 = area.removeFromTop (300);
    auto upperarea2 = lowerarea2.removeFromTop (150);
    //horizontal offset
    lowerarea2.reduce (10,0);
    upperarea2.reduce (10,0);
    
    for (int i = 0; i < 4; i++)
    {
        knobs [i]->setBounds (upperarea2.removeFromLeft (100).reduced (4) );
    }
    for (int i = 4; i < 8; i++)
    {
        knobs [i]->setBounds (lowerarea2.removeFromLeft (100).reduced (4) );
    }
    matrix.setBounds (area.removeFromTop (300). reduced (10, 0). reduced (4, 0) );
    
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}
