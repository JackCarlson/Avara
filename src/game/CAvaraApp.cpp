/*
    Copyright ©1994-1996, Juri Munkki
    All rights reserved.

    File: CAvaraApp.c
    Created: Wednesday, November 16, 1994, 01:26
    Modified: Friday, September 20, 1996, 02:05
*/

#define MAINAVARAAPP

#include "CAvaraApp.h"

#include "AvaraGL.h"
#include "AvaraScoreInterface.h"
#include "AvaraTCP.h"
#include "CAvaraGame.h"
#include "CBSPWorld.h"
#include "CCompactTagBase.h"
#include "CLevelDescriptor.h"
#include "CNetManager.h"
#include "CRC.h"
#include "CSoundMixer.h"
#include "CViewParameters.h"
#include "CommandList.h"
#include "KeyFuncs.h"
#include "LevelLoader.h"
#include "Parser.h"
#include "Preferences.h"
#include "Resource.h"
#include "System.h"
#include "InfoMessages.h"

// included while we fake things out
#include "CPlayerManager.h"

CAvaraApp::CAvaraApp() : CApplication("Avara") {
    itsGame = new CAvaraGame;
    gCurrentGame = itsGame;
    itsGame->IAvaraGame(this);
    itsGame->UpdateViewRect(mSize.x, mSize.y, mPixelRatio);

    gameNet->ChangeNet(kNullNet, "");

    setLayout(new nanogui::FlowLayout(nanogui::Orientation::Vertical, true, 20, 20));

    playerWindow = new CPlayerWindow(this);
    playerWindow->setFixedWidth(200);

    levelWindow = new CLevelWindow(this);
    levelWindow->setFixedWidth(200);

    networkWindow = new CNetworkWindow(this);
    networkWindow->setFixedWidth(200);

    trackerWindow = new CTrackerWindow(this);
    trackerWindow->setFixedWidth(300);

    rosterWindow = new CRosterWindow(this);

    performLayout();
}

CAvaraApp::~CAvaraApp() {
    itsGame->Dispose();
    DeallocParser();
}

void CAvaraApp::Done() {
    // This will trigger a clean disconnect if connected.
    gameNet->ChangeNet(kNullNet, "");
    CApplication::Done();
}

void CAvaraApp::idle() {
    CheckSockets();
    if(itsGame->GameTick()) {
        glClearColor(mBackground[0], mBackground[1], mBackground[2], mBackground[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        drawContents();
        SDL_GL_SwapWindow(mSDLWindow);
    }
}

void CAvaraApp::drawContents() {
    itsGame->Render(mNVGContext);
}

void CAvaraApp::WindowResized(int width, int height) {
    itsGame->UpdateViewRect(width, height, mPixelRatio);
    //performLayout();
}

bool CAvaraApp::handleSDLEvent(SDL_Event &event) {
    if(itsGame->IsPlaying()) {
        itsGame->HandleEvent(event);
        return true;
    }
    else {
        if (rosterWindow->handleSDLEvent(event)) return true;
        return CApplication::handleSDLEvent(event);
    }
}

void CAvaraApp::drawAll() {
    if (!itsGame->IsPlaying()) {
        rosterWindow->UpdateRoster();
        CApplication::drawAll();
    }
}

bool CAvaraApp::DoCommand(int theCommand) {
    std::string name = String(kPlayerNameTag);
    Str255 userName;
    userName[0] = name.length();
    BlockMoveData(name.c_str(), userName + 1, name.length());
    SDL_Log("DoCommand %d\n", theCommand);
    switch (theCommand) {
        case kReportNameCmd:
            gameNet->NameChange(userName);
            return true;
            // break;
        case kStartGame:
        case kResumeGame:
            itsGame->SendStartCommand();
            return true;
            // break;
        case kGetReadyToStartCmd:
            break;
        default:
            break;
    }
    return false;
    /*
        default:
            if(	theCommand >= kLatencyToleranceZero &&
                theCommand <= kLatencyToleranceMax)
            {	WriteShortPref(kLatencyToleranceTag, theCommand - kLatencyToleranceZero);
            }
            else
            if(	theCommand >= kRetransmitMin &&
                theCommand <= kRetransmitMax)
            {	WriteShortPref(kUDPResendPrefTag, theCommand - kRetransmitMin);

                if(gameNet->itsCommManager)
                {	gameNet->itsCommManager->OptionCommand(theCommand);
                }
            }
            if(	theCommand >= kSlowestConnectionCmd && theCommand <= kFastestConnectionCmd)
            {	WriteShortPref(kUDPConnectionSpeedTag, theCommand - kSlowestConnectionCmd);

                if(gameNet->itsCommManager)
                {	gameNet->itsCommManager->OptionCommand(theCommand);
                }
            }
            else
                inherited::DoCommand(theCommand);
            break;
    }
    */
}

OSErr CAvaraApp::LoadLevel(std::string set, OSType theLevel) {
    SDL_Log("LOADING LEVEL %d FROM %s\n", theLevel, set.c_str());
    itsGame->LevelReset(false);
    itsGame->loadedTag = theLevel;
    gCurrentGame = itsGame;

    std::string rsrcFile = std::string("levels/") + set + ".r";
    UseResFile(rsrcFile);

    OSType setTag;
    CLevelDescriptor *levels = LoadLevelListFromResource(&setTag);
    CLevelDescriptor *curLevel = levels;
    bool wasLoaded = false;
    while (curLevel) {
        if (curLevel->tag == theLevel) {
            std::string rsrcName((char *)curLevel->access + 1, curLevel->access[0]);
            BlockMoveData(curLevel->name, itsGame->loadedLevel, curLevel->name[0] + 1);
            Handle levelData = GetNamedResource('PICT', rsrcName);
            if (levelData) {
                ConvertToLevelMap(levelData);
                ReleaseResource(levelData);
                wasLoaded = true;
            }
            break;
        }
        curLevel = curLevel->nextLevel;
    }
    levels->Dispose();

    if (wasLoaded) {
        Fixed pt[3];
        itsGame->itsWorld->OverheadPoint(pt);
        SDL_Log("overhead %f, %f, %f\n", ToFloat(pt[0]), ToFloat(pt[1]), ToFloat(pt[2]));
        itsGame->itsView->yonBound = FIX(10000);
        itsGame->itsView->LookFrom(pt[0] + FIX(100), pt[1] + FIX(200), pt[2] + FIX(150));
        itsGame->itsView->LookAt(pt[0], pt[1], pt[2]);
        itsGame->itsView->PointCamera();
    }

    return noErr;
}

void CAvaraApp::NotifyUser() {
    // TODO: Bell sound(s)
}

// STUBBBBBZZZZZ

void CAvaraApp::SetIndicatorDisplay(short i, short v) {}
void CAvaraApp::NumberLine(long theNum, short align) {}
void CAvaraApp::DrawUserInfoPart(short i, short partList) {}
void CAvaraApp::BrightBox(long frameNum, short position) {}

void CAvaraApp::AddMessageLine(const char* line) {
    SDL_Log("Message: %s", line);
    messageLines.push_back(std::string(line));
    if (messageLines.size() > 5) {
        messageLines.pop_front();
    }
}
void CAvaraApp::MessageLine(short index, short align) {
    SDL_Log("CAvaraApp::MessageLine(%d)\n", index);
    switch(index) {
        case kmWelcome1:
        case kmWelcome2:
        case kmWelcome3:
        case kmWelcome4:
            AddMessageLine("Welcome to Avara.");
            break;
        case kmStarted:
            AddMessageLine("Starting new game.");
            break;
        case kmRestarted:
            AddMessageLine("Resuming game.");
            break;
        case kmAborted:
            AddMessageLine("Aborted.");
            break;
        case kmWin:
            AddMessageLine("You were victorious!");
            break;
        case kmGameOver:
            AddMessageLine("Game over.");
            break;
        case kmSelfDestruct:
            AddMessageLine("Self-destruct activated.");
            break;
        case kmFragmentAlert:
            AddMessageLine("ALERT: Reality fragmentation detected!");
            break;
        case kmRefusedLogin:
            AddMessageLine("Login refused.");
            break;
    }

}
void CAvaraApp::LevelReset() {}
void CAvaraApp::ParamLine(short index, short align, StringPtr param1, StringPtr param2) {
    SDL_Log("CAvaraApp::ParamLine(%d)\n", index);
    const char* fmt;
    char* buffa;
    std::string a = std::string((char *)param1 + 1, param1[0]);
    std::string b;
    if (param2) b = std::string((char *)param2 + 1, param2[0]);

    switch(index) {
        case kmPaused:
            {
                fmt = "Game paused by %s.";
                buffa = new char[32 + param1[0]];
                sprintf(buffa, fmt, a.c_str());
                AddMessageLine(buffa);
            }
            break;
        case kmWaitingForPlayer:
            {
                fmt = "Waiting for %s.";
                buffa = new char[32 + param1[0]];
                sprintf(buffa, fmt, a.c_str());
                AddMessageLine(buffa);
            }
        case kmAKilledBPlayer:
            {
                fmt  = "%s killed %s.";
                buffa = new char[32 + param1[0] + param2[0]];
                sprintf(buffa, fmt, a.c_str(), b.c_str());
                AddMessageLine(buffa);
            }
            break;
        case kmUnavailableNote:
            {
                fmt = "%s is busy.";
                buffa = new char[32 + param1[0]];
                sprintf(buffa, fmt, a.c_str());
                AddMessageLine(buffa);
            }
        case kmStartFailure:
            {
                fmt = "Couldn't gather %s for a game.";
                buffa = new char[32 + param1[0]];
                sprintf(buffa, fmt, a.c_str());
                AddMessageLine(buffa);
            }
            break;
    }
    delete buffa;
}
void CAvaraApp::StartFrame(long frameNum) {}
void CAvaraApp::StringLine(StringPtr theString, short align) {
    AddMessageLine(std::string((char* ) theString + 1, theString[0]).c_str());
}
void CAvaraApp::ComposeParamLine(StringPtr destStr, short index, StringPtr param1, StringPtr param2) {
    ParamLine(index, 0, param1, param2);
}
