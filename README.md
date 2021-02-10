# MidiSnapPad

The idea – inspired by [Logic Pro X's Alchemy Synth](https://help.apple.com/logicpro-instruments/mac/10.2.1/#/lgsi1ebc8d34) – is to enable Midi users to smoothly interpolate between predefined Midi settings ("snapshots"). 

I hope to provide the possibility of sound transformations via multi-dimensional Midi manipulation in a less laborious way than conventional X/Y-Pads.

2D interpolation is done by solving equation systems very similar to [Bilinear Interpolation](https://en.wikipedia.org/wiki/Bilinear_interpolation).


## Example

The selector (turquoise rectangle) can be moved around between states. Each of the round parameter knobs above the selector surface will be manipulated accordingly. 

State 1|State 2|Interpolated 
-------------------------|-------------------------|-------------------------
![Alt text](/Screenshots/screen_dry.png?raw=true "Optional Title") | ![Alt text](/Screenshots/screen_wet.png?raw=true "Optional Title") | ![Alt text](/Screenshots/screen_interp.png?raw=true "Optional Title")

The columns **State 1** and **State 2** show the selector perfectly aligned to specific states ("snapshots"). The **Interpolated** column give an example of how we can smoothly transition between 2 "snapshots": Look for "Reverb" and "Detune" parameters. Note that the selector is not perfectly aligned and the parameters (e.g.  "Sustain") are influenced by the left neighbouring states.


## JUCE

This Midi tool is intended to become a cross-platform application for iOS, Android and MacOS, therefore
MidiSnapPad is built with the cross-platform framework [JUCE](https://juce.com/).
