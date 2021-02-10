#pragma once

#include <JuceHeader.h>
#include "UIOverlayServer.h"
#include "OtherLookAndFeel.h"
#include "ScrollableNumber.h"
#include "MidiKnob.h"
#include "BackendAlgebra.h"
#include "MyText.h"
#include "MyTextEditor.h"
#include "MetaSettingComponent.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::Component {
public:
    //==============================================================================
    MainComponent();

    ~MainComponent() override;

    //==============================================================================
    void paint(juce::Graphics &) override;

    void resized() override;

    void pushBackNewKnob();

    static juce::String getAllSystemInfo()
    {
        juce::String systemInfo;

        systemInfo
          << "Here are a few system statistics..." << juce::newLine
          << juce::newLine
          << "Time and date:    " << juce::Time::getCurrentTime().toString (true, true) << juce::newLine
          << "System up-time:   " << juce::RelativeTime::milliseconds ((juce::int64) juce::Time::getMillisecondCounterHiRes()).getDescription() << juce::newLine
          << "Compilation date: " << juce::Time::getCompilationDate().toString (true, false) << juce::newLine
          << juce::newLine
          << "Operating system: " << juce::SystemStats::getOperatingSystemName() << juce::newLine
          << "Host name:        " << juce::SystemStats::getComputerName()        << juce::newLine
          << "Device type:      " << juce::SystemStats::getDeviceDescription()   << juce::newLine
          << "Manufacturer:     " << juce::SystemStats::getDeviceManufacturer()  << juce::newLine
          << "User logon name:  " << juce::SystemStats::getLogonName()           << juce::newLine
          << "Full user name:   " << juce::SystemStats::getFullUserName()        << juce::newLine
          << "User region:      " << juce::SystemStats::getUserRegion()          << juce::newLine
          << "User language:    " << juce::SystemStats::getUserLanguage()        << juce::newLine
          << "Display language: " << juce::SystemStats::getDisplayLanguage()     << juce::newLine
          << juce::newLine;

        systemInfo
          << "Current working directory:  " << juce::File::getCurrentWorkingDirectory().getFullPathName() << juce::newLine
          << "Current application file:   " << juce::File::getSpecialLocation (juce::File::currentApplicationFile).getFullPathName() << juce::newLine
          << "Current executable file:    " << juce::File::getSpecialLocation (juce::File::currentExecutableFile) .getFullPathName() << juce::newLine
          << "Invoked executable file:    " << juce::File::getSpecialLocation (juce::File::invokedExecutableFile) .getFullPathName() << juce::newLine
          << juce::newLine;

        systemInfo
          << "User home folder:               " << juce::File::getSpecialLocation (juce::File::userHomeDirectory)             .getFullPathName() << juce::newLine
          << "User desktop folder:            " << juce::File::getSpecialLocation (juce::File::userDesktopDirectory)          .getFullPathName() << juce::newLine
          << "User documents folder:          " << juce::File::getSpecialLocation (juce::File::userDocumentsDirectory)        .getFullPathName() << juce::newLine
          << "User application data folder:   " << juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)  .getFullPathName() << juce::newLine
          << "User music folder:              " << juce::File::getSpecialLocation (juce::File::userMusicDirectory)            .getFullPathName() << juce::newLine
          << "User movies folder:             " << juce::File::getSpecialLocation (juce::File::userMoviesDirectory)           .getFullPathName() << juce::newLine
          << "User pictures folder:           " << juce::File::getSpecialLocation (juce::File::userPicturesDirectory)         .getFullPathName() << juce::newLine
          << "Common application data folder: " << juce::File::getSpecialLocation (juce::File::commonApplicationDataDirectory).getFullPathName() << juce::newLine
          << "Common documents folder:        " << juce::File::getSpecialLocation (juce::File::commonDocumentsDirectory)      .getFullPathName() << juce::newLine
          << "Local temp folder:              " << juce::File::getSpecialLocation (juce::File::tempDirectory)                 .getFullPathName() << juce::newLine
          << juce::newLine;



//        systemInfo
//          << "File System roots: "          << getFileSystemRoots() << juce::newLine
//          << "Free space in home folder: "  << File::descriptionOfSizeInBytes (File::getSpecialLocation (File::userHomeDirectory)
//                                                                                    .getBytesFreeOnVolume()) << juce::newLine
//          << juce::newLine
//          << getDisplayInfo() << juce::newLine
//          << "Network IP addresses: "       << juce::newLine << getIPAddressList()  << juce::newLine
//          << "Network card MAC addresses: " << juce::newLine << getMacAddressList() << juce::newLine;

        DBG (systemInfo);
        DBG ("FILENAMES");
        juce::Array<juce::File> childFiles = juce::File::getSpecialLocation (juce::File::tempDirectory). findChildFiles(juce::File::TypesOfFileToFind::findFilesAndDirectories, false);
        for (auto i: childFiles){
            DBG (i.getFileName ());
        }
        // ANDROID: (do we need local temp??) User home folder: /data/user/0/com.yourcompany.newproject, has cache, code_cache, files, databases, shared_prefs, no_backup, .temp
        // Rather go with common application data folder for android or local temp and choose parentdirectory cause it goes one to deep
        // MAC: We need the local temp folder: /Users/expert239/Library/Caches/NewProject ----NO FILES----
        // iOs: do go with local temp folder
        return systemInfo;
    };

private:
    //==============================================================================
    // Your private member variables go here...
    void manipulateMatrixValues();


    std::unique_ptr<juce::MidiOutput> midiOut;
    std::unique_ptr<MetaSettingComponent> metaSettings;

    std::vector<MidiKnob *> knobs;
    OtherLookAndFeel otherLookAndFeel;
    UIOverlayServer uiOverlay {};
    MatrixField matrix {uiOverlay};
    juce::Slider *editModeToggle;
    juce::Rectangle<int> backgroundArea;

    #if !JUCE_IOS
    juce::OpenGLContext openGLContext;
    #endif


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
