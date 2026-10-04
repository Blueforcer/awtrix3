#ifndef MenuManager_h
#define MenuManager_h

#include <Arduino.h>


class MenuManager_
{
private:
    MenuManager_() = default;

public:
    static MenuManager_ &getInstance();
    bool inMenu;
    String menutext();
    void rightButton();
    void leftButton();
    void selectButton();
    void selectButtonLong();

#ifndef AWTRIX_DISABLE_TIMER
    // Open the TIMER menu directly from the Timer app's idle long-press:
    // list focus, first item, origin = App so a long-press out of the
    // list returns to the Timer app rather than the main menu.
    void openTimerMenuFromApp();
#endif
};

extern MenuManager_ &MenuManager;

#endif