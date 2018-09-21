#include "CRosterWindow.h"

#include "AvaraDefines.h"
#include "CAbstractPlayer.h"
#include "CAvaraApp.h"
#include "CNetManager.h"
#include "CPlayerManager.h"

#include <nanogui/colorcombobox.h>
#include <nanogui/layout.h>
#include <numeric>
#include <sstream>
using namespace nanogui;

std::vector<long> player_colors =
    {kGreenTeamColor, kYellowTeamColor, kRedTeamColor, kPinkTeamColor, kPurpleTeamColor, kBlueTeamColor};

std::vector<Text *> statuses;
std::vector<Text *> chats;
std::vector<ColorComboBox *> colors;
OSType currentLevel;

const int CHAT_CHARS = 57;
const int ROSTER_FONT_SIZE = 15;
bool textInputStarted = false;
char backspace[1] = {'\b'};
char clearline[1] = {'\x1B'};
char endline[1] = {13};
char bellline[1] = {7};
char checkline[1] = {6};

CRosterWindow::CRosterWindow(CApplication *app) : CWindow(app, "Roster") {
    setFixedWidth(470);
    BoxLayout *blayout = new nanogui::BoxLayout(nanogui::Orientation::Vertical, nanogui::Alignment::Fill, 10, 0);
    AdvancedGridLayout *layout = new AdvancedGridLayout();
    setLayout(blayout);
    auto panel = add<Widget>();
    panel->setLayout(layout);
    theNet = ((CAvaraApp *)gApplication)->gameNet;
    for (int i = 0; i < kMaxAvaraPlayers; i++) {
        layout->appendRow(1, 1);
        layout->appendCol(1, 1);

        ColorComboBox *color = new ColorComboBox(panel, player_colors);
        //color->setFixedHeight(23);
        color->setSelectedIndex(((CAvaraApp *)gApplication)->gameNet->teamColors[i]);
        color->setCallback([this, color, i](int selectedIdx) {
            theNet->teamColors[i] = selectedIdx;
            theNet->SendColorChange();
        });
        color->popup()->setSize(nanogui::Vector2i(50, 230));
        layout->setAnchor(color, AdvancedGridLayout::Anchor(0, i * 2));
        Text *status = new Text(panel, "", false, ROSTER_FONT_SIZE + 2);
        layout->setAnchor(status, AdvancedGridLayout::Anchor(1, i * 2));
        layout->appendRow(1, 1);
        layout->appendCol(1, 1);

        nanogui::Text *chat = new nanogui::Text(panel, "", false, ROSTER_FONT_SIZE);
        layout->setAnchor(chat, AdvancedGridLayout::Anchor(0, i * 2 + 1, 2, 1));

        status->setAlignment(Text::Alignment::Right);
        chat->setAlignment(Text::Alignment::Left);

        chat->setFont("mono");

        status->setFixedWidth(120);
        chat->setFixedWidth(550);
        chat->setFixedHeight(30);
        color->setFixedWidth(225);

        statuses.push_back(status);
        chats.push_back(chat);
        colors.push_back(color);
    }

    levelLoaded = new Text(this, "", false, 16);
    levelDesigner = new Text(this, "", false, 16);
    levelDescription = new Label(this, "No level loaded");

    levelLoaded->setAlignment(Text::Alignment::Left);
    levelDesigner->setAlignment(Text::Alignment::Left);
    //levelDescription->setAlignment(Text::Alignment::Left);
    levelDescription->setFixedHeight(100);
    levelDescription->setFixedWidth(450);

    currentLevel = ((CAvaraApp *)gApplication)->itsGame->loadedTag;

    UpdateRoster();
}

CRosterWindow::~CRosterWindow() {}

void CRosterWindow::UpdateRoster() {
    for (int i = 0; i < kMaxAvaraPlayers; i++) {
        CPlayerManager *thisPlayer = theNet->playerTable[i];

        const std::string theName((char *)thisPlayer->playerName + 1, thisPlayer->playerName[0]);

        short status = thisPlayer->loadingStatus;
        std::string theStatus = GetStringStatus(status, thisPlayer->winFrame);

        std::string theChat = thisPlayer->GetChatString(CHAT_CHARS);

        statuses[i]->setValue(theStatus.c_str());
        chats[i]->setValue(theChat.c_str());
        colors[i]->setSelectedIndex(theNet->teamColors[i]);
        colors[i]->setCaption(theName.c_str());
    }


    CAvaraGame *theGame = ((CAvaraApp *)gApplication)->itsGame;
    if (theGame->loadedTag != currentLevel) {
        std::string theLevel((char* ) theGame->loadedLevel + 1, theGame->loadedLevel[0]);
        std::string theDesigner((char *)theGame->loadedDesigner + 1, theGame->loadedDesigner[0]);
        levelLoaded->setValue(theLevel);
        levelDesigner->setValue(theDesigner);

        std::string desc((char *)theGame->loadedInfo + 1, theGame->loadedInfo[0]);
        if (desc.length() > 0) levelDescription->setCaption(desc);
        currentLevel = theGame->loadedTag;
    }
}

bool CRosterWindow::DoCommand(int theCommand) {
    return false;
}

std::string CRosterWindow::GetStringStatus(short status, Fixed winFrame) {
    std::string strStatus;
    if (winFrame >= 0) {
        long timeTemp = FMulDiv(winFrame, ((CAvaraApp *)gApplication)->itsGame->frameTime, 10);
        auto hundreds1 = timeTemp % 10;
        timeTemp /= 10;
        auto hundreds2 = timeTemp % 10;
        timeTemp /= 10;
        auto secs1 = timeTemp % 10;
        timeTemp /= 10;
        auto secs2 = timeTemp % 6;
        timeTemp /= 6;

        std::ostringstream os;
        os << "[" << timeTemp << ":" << secs2 << secs1 << "." << hundreds2 << hundreds1 << "]";

        strStatus = os.str();
        return strStatus;
    }

    if (status == kLConnected) {
        strStatus = "connected";
    } else if (status == kLLoaded) {
        strStatus = "ready";
    } else if (status == kLWaiting) {
        strStatus = "waiting";
    } else if (status == kLTrying) {
        strStatus = "loading";
    } else if (status == kLMismatch) {
        strStatus = "version mismatch";
    } else if (status == kLNotFound) {
        strStatus = "level not found";
    } else if (status == kLPaused) {
        strStatus = "paused";
    } else if (status == kLActive) {
        strStatus = "active";
    } else if (status == kLNoVehicle) {
        strStatus = "HECTOR not available";
    } else {
        strStatus = "";
    }
    return strStatus;
}

bool CRosterWindow::mouseEnterEvent(const nanogui::Vector2i &p, bool enter) {
    if (enter && !textInputStarted) {
        SDL_StartTextInput();
        textInputStarted = true;
    }
    if (!enter && textInputStarted) {
        SDL_StartTextInput();
        textInputStarted = false;
    }
    return true;
};

void CRosterWindow::SendRosterMessage(int len, char *message) {
    ((CAvaraApp *)gApplication)->gameNet->SendRosterMessage(len, message);
}

bool CRosterWindow::handleSDLEvent(SDL_Event &event) {
    if (!textInputStarted)
        return false;
    if (event.type == SDL_TEXTINPUT) {
        // we already sent a checkmark, don't send a v
        if ((SDL_GetModState() & KMOD_ALT) && strcmp(event.text.text, "v") == 0)
            return true;
        SendRosterMessage(strlen(event.text.text), event.text.text);
        return true;
    } else if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_BACKSPACE:
                SendRosterMessage(1, backspace);
                return true;
            case SDLK_RETURN:
                SendRosterMessage(1, endline);
                return true;
            case SDLK_CLEAR:
            case SDLK_DELETE:
                SendRosterMessage(1, clearline);
                return true;
            case SDLK_g:
                if (SDL_GetModState() & KMOD_CTRL) {
                    SendRosterMessage(1, bellline);
                    return true;
                } else
                    return false;
            case SDLK_v:
                if (SDL_GetModState() & KMOD_ALT) {
                    SendRosterMessage(1, checkline);
                    return true;
                } else
                    return false;
            default:
                return false;
        }
    } else {
        return false;
    }
}