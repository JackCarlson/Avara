#pragma once

#include "CWindow.h"

#include <string>
#include <vector>

class CLevelWindow : public CWindow {
public:
    CLevelWindow(CApplication *app);

    virtual ~CLevelWindow();

    // Handles a command broadcasted by CApplication::BroadcastCommand. Returns true if it was actually handled.
    virtual bool DoCommand(int theCommand);

    void SelectSet(int selected);
    void SendLoad();

    nanogui::ComboBox *setBox;
    nanogui::ComboBox *levelBox;
    nanogui::Button *loadBtn;
    nanogui::Button *startBtn;

    std::vector<std::string> levelSets;
    std::vector<std::string> levelNames;
    std::vector<OSType> levelTags;
};
