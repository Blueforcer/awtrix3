#include <MenuManager.h>
#include <Arduino.h>
#include <AudioManager.h>
#include <Globals.h>
#include <ServerManager.h>
#include <DisplayManager.h>
#include <updater.h>

String menuText;
int menuSelection;

int8_t menuIndex = 0;
int8_t stationIndex = 0;
bool isPlayingRadio = false;

enum MenuState
{
    MainMenu,
    RadioMenu,
    StationSelection,
    PlayingStation,
    VolumeMenu,
    BrightnessMenu,
    FPSMenu,
    ColorMenu,
    SwitchMenu,
    TspeedMenu,
    AppTimeMenu
};

const char *menuItems[] = {
    "BRIGHT",
    "FPS",
    "COLOR",
    "SWITCH",
    "T-SPEED",
    "APPTIME",
    "UPDATE"};

byte menuItemCount = 7;

MenuState currentState = MainMenu;

uint16_t textColors[] = {
    0xFFFF,  // White
    0xF800,  // Red
    0xF812,  // Dark orange
    0xF81F,  // Yellow
    0x881F,  // Dark green
    0x001F,  // Blue
    0x04FF,  // Light blue
    0x07FC,  // Cyan
    0x07E2,  // Seafoam green
    0xAFE0,  // Light green
    0xFFE0,  // Light yellow
    0xFD60,  // Dark yellow
    0xFBC0}; // Pink

uint8_t currentColor;

MenuManager_ &MenuManager_::getInstance()
{
    static MenuManager_ instance;
    return instance;
}

// Initialize the global shared instance
MenuManager_ &MenuManager = MenuManager.getInstance();

String MenuManager_::menutext()
{
    if (currentState == MainMenu)
    {
        return (menuItems[menuIndex]);
    }
    else if (currentState == BrightnessMenu)
    {
        if (AUTO_BRIGHTNESS)
        {
            return ("AUTO");
        }
        else
        {
            return (String(BRIGHTNESS_PERCENT) + "%");
        }
    }
    else if (currentState == FPSMenu)
    {
        return String(MATRIX_FPS) + " FPS";
    }
    else if (currentState == ColorMenu)
    {
        DisplayManager.setTextColor(textColors[currentColor]);
        String colorStr = String(textColors[currentColor], HEX);
        while (colorStr.length() < 4)
        {
            colorStr = "0" + colorStr;
        }
        return colorStr;
    }
    else if (currentState == SwitchMenu)
    {
        return AUTO_TRANSITION ? "ON" : "OFF";
    }
    else if (currentState == TspeedMenu)
    {
        float seconds = (float)TIME_PER_TRANSITION / 1000.0;
        return String(seconds, 1) + "s";
        ;
    }
    else if (currentState == AppTimeMenu)
    {
        float seconds = (float)TIME_PER_APP / 1000.0;
        return String(seconds, 0) + "s";
        ;
    }
    return "";
}

void MenuManager_::rightButton()
{
    if (!inMenu)
        return;
    if (currentState == MainMenu)
    {
        menuIndex++;
        if (menuIndex > menuItemCount - 1)
        {
            menuIndex = 0; // Wrap around to the first menu item
        }
    }
    else if (currentState == RadioMenu || currentState == StationSelection)
    {
        AudioManager.nextStation();
    }
    else if (currentState == BrightnessMenu)
    {
        if (!AUTO_BRIGHTNESS)
        {
            ++BRIGHTNESS_PERCENT;
            if (BRIGHTNESS_PERCENT > 100)
                BRIGHTNESS_PERCENT = 1;
            BRIGHTNESS = map(BRIGHTNESS_PERCENT, 0, 100, 0, 255);
            DisplayManager.setBrightness(BRIGHTNESS);
        }
    }
    else if (currentState == FPSMenu)
    {
        if (MATRIX_FPS < 30)
            ++MATRIX_FPS;
    }
    else if (currentState == ColorMenu)
    {
        int arraySize = sizeof(textColors) / sizeof(textColors[0]);
        currentColor = (currentColor + 1) % arraySize;
    }
    else if (currentState == SwitchMenu)
    {
        AUTO_TRANSITION = !AUTO_TRANSITION;
    }
    else if (currentState == TspeedMenu)
    {
        TIME_PER_TRANSITION = min(1200, TIME_PER_TRANSITION + 100);
    }
    else if (currentState == AppTimeMenu)
    {
        TIME_PER_APP = min(30000, TIME_PER_APP + 1000);
    }
}

void MenuManager_::leftButton()
{
    if (!inMenu)
        return;
    if (currentState == MainMenu)
    {
        menuIndex--;
        if (menuIndex < 0)
        {
            menuIndex = menuItemCount - 1; // Wrap around to the last menu item
        }
    }
    else if (currentState == RadioMenu || currentState == StationSelection)
    {
        AudioManager.prevStation();
    }
    else if (currentState == BrightnessMenu)
    {
        if (!AUTO_BRIGHTNESS)
        {
            --BRIGHTNESS_PERCENT;
            if (BRIGHTNESS_PERCENT < 1)
                BRIGHTNESS_PERCENT = 100;
            BRIGHTNESS = map(BRIGHTNESS_PERCENT, 0, 100, 0, 255);
            DisplayManager.setBrightness(BRIGHTNESS);
        }
    }
    else if (currentState == FPSMenu)
    {
        if (MATRIX_FPS > 15)
            --MATRIX_FPS;
    }
    else if (currentState == ColorMenu)
    {
        int arraySize = sizeof(textColors) / sizeof(textColors[0]);
        currentColor = (currentColor - 1 + arraySize) % arraySize;
    }
    else if (currentState == SwitchMenu)
    {
        AUTO_TRANSITION = !AUTO_TRANSITION;
    }
    else if (currentState == TspeedMenu)
    {
        TIME_PER_TRANSITION = max(200, TIME_PER_TRANSITION - 100);
    }
    else if (currentState == AppTimeMenu)
    {
        TIME_PER_APP = max(1000, TIME_PER_APP - 1000);
    }
}

void MenuManager_::selectButton()
{
    if (!inMenu)
        return;
    if (currentState == MainMenu)
    {
        if (menuIndex == 0) // BRIGHT
        {
            BRIGHTNESS_PERCENT = map(BRIGHTNESS, 0, 255, 0, 100);
            currentState = BrightnessMenu;
        }
        else if (menuIndex == 1) // RESET
        {
            currentState = FPSMenu;
        }
        else if (menuIndex == 2) // COLOR
        {
            currentState = ColorMenu;
        }
        else if (menuIndex == 3) // COLOR
        {
            currentState = SwitchMenu;
        }
        else if (menuIndex == 4) // TSPEED
        {
            currentState = TspeedMenu;
        }
        else if (menuIndex == 5) // AppTIme
        {
            currentState = AppTimeMenu;
        }
        else if (menuIndex == 6) // Updater
        {
           FirmwareVersionCheck();
        }
    }
    else if (currentState == StationSelection)
    {
        AudioManager.startRadioStation(AudioManager.getCurrentRadioStation());
    }
    else if (currentState == BrightnessMenu)
    {
        AUTO_BRIGHTNESS = !AUTO_BRIGHTNESS;
    }
}

void MenuManager_::selectButtonLong()
{
    if (inMenu)
    {
        if (currentState == BrightnessMenu)
        {
            BRIGHTNESS = map(BRIGHTNESS_PERCENT, 0, 100, 0, 255);
            saveSettings();
        }
        else if (currentState == FPSMenu)
        {
            DisplayManager.setFPS(MATRIX_FPS);
            saveSettings();
        }
        else if (currentState == ColorMenu)
        {
            TEXTCOLOR_565 = textColors[currentColor];
            saveSettings();
        }
        else if (currentState == MainMenu)
        {
            inMenu = false;
        }
        else if (currentState == SwitchMenu)
        {
            DisplayManager.setAutoTransition(AUTO_TRANSITION);
            saveSettings();
        }
        else if (currentState == TspeedMenu)
        {
            DisplayManager.applyAllSettings();
            saveSettings();
        }
        else if (currentState == AppTimeMenu)
        {
            DisplayManager.applyAllSettings();
            saveSettings();
        }
        currentState = MainMenu;
    }
    else
    {
        inMenu = true;
    }
}