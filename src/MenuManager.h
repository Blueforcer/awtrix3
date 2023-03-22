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
};

extern MenuManager_ &MenuManager;

#endif