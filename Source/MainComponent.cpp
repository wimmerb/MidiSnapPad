#include "MainComponent.h"
//==============================================================================
MainComponent::MainComponent()
{
    matrix.updateKnobs = [&]
    {
        std::cout << "landedHEREEE" << std::endl;
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
    };
    
    
    this->setLookAndFeel (&otherLookAndFeel);
    
    midiOut = juce::MidiOutput::openDevice (juce::MidiOutput::getDefaultDevice().identifier);
    
    for (int i = 0; i < 8; i++)
    {
        pushBackNewKnob();
    }
    
    setSize (400, 800);
    addAndMakeVisible (midiOutputList);
    midiOutputList.setTextWhenNoChoicesAvailable ("No MIDI Inputs Enabled");
    auto midiOutputs = juce::MidiOutput::getAvailableDevices ();
    
    juce::StringArray midiOutputNames;
    
    for (auto output : midiOutputs)
        midiOutputNames.add (output.name);
    
    midiOutputList.addItemList (midiOutputNames, 1);
    midiOutputList.onChange = [this] { setMidiOutput (midiOutputList.getSelectedItemIndex()); };
    if (midiOutputList.getSelectedId() == 0)
        setMidiOutput (0);
    
    
    
    
    
    
    resized();
    
    editModeToggle->onValueChange ();
}


void MainComponent::setMidiOutput (int index)
{
    auto list = juce::MidiOutput::getAvailableDevices();

    auto newOutput = list[index];

    midiOut = juce::MidiOutput::openDevice (newOutput.identifier);
}

void MainComponent::pushBackNewKnob ()
{
    MidiKnob * tmp = new MidiKnob (midiOut);
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
    g.fillAll (getLookAndFeel ().findColour (juce::ResizableWindow::backgroundColourId));

    /*g.setFont (juce::Font (16.0f));
    g.setColour (juce::Colours::white);
    g.drawText ("Hello World!", getLocalBounds(), juce::Justification::centred, true);*/
}

void MainComponent::resized ()
{
    auto area = getLocalBounds ();
    
    //TODO textbox neben midiOutputList bzw. midiOutputList als eigenes Zeichen...
    midiOutputList.setBounds (area.removeFromTop (36).reduced (8));
    auto lowerarea2 = area.removeFromTop (300);
    auto upperarea2 = lowerarea2.removeFromTop (150);
    for (int i = 0; i < 4; i++)
    {
        knobs [i]->setBounds (upperarea2.removeFromLeft (100).reduced (8, 8) );
    }
    for (int i = 4; i < 8; i++)
    {
        knobs [i]->setBounds (lowerarea2.removeFromLeft (100).reduced (8, 8) );
    }
    matrix.setBounds (area.removeFromTop (300). reduced (8, 8) );
    editModeToggle->setBounds (area );
    
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}
