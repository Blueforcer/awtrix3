#include <MenuManager.h>
#include <Arduino.h>
#include <AudioManager.h>
#include <Settings.h>
#include <ServerManager.h>
#include <DisplayManager.h>

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
    Reset,
    Volume,
    Brightness,
    FPS,
    Color
};

const char *menuItems[] = {
    "BRIGHT",
    "FPS",
    "COLOR",
    "RESET"};

byte menuItemCount = 4;

MenuState currentState = MainMenu;

uint16_t textColors[] = {
0xFFFF, // White
0xF800, // Red
0xF812, // Dark orange
0xF81F, // Yellow
0x881F, // Dark green
0x001F, // Blue
0x04FF, // Light blue
0x07FC, // Cyan
0x07E2, // Seafoam green
0xAFE0, // Light green
0xFFE0, // Light yellow
0xFD60, // Dark yellow
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
    else if (currentState == Brightness)
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
    else if (currentState == FPS)
    {
        return String(MATRIX_FPS) + " FPS";
    }
    else if (currentState == Color)
    {
        DisplayManager.setTextColor(textColors[currentColor]);
        String colorStr = String(textColors[currentColor], HEX);
        while (colorStr.length() < 4)
        {
            colorStr = "0" + colorStr;
        }
        return colorStr;
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
    else if (currentState == Brightness)
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
    else if (currentState == FPS)
    {
        if (MATRIX_FPS < 30)
            ++MATRIX_FPS;
    }
    else if (currentState == Color)
    {
        int arraySize = sizeof(textColors) / sizeof(textColors[0]);
        currentColor = (currentColor + 1) % arraySize;
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
    else if (currentState == Brightness)
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
    else if (currentState == FPS)
    {
        if (MATRIX_FPS > 15)
            --MATRIX_FPS;
    }
    else if (currentState == Color)
    {
        int arraySize = sizeof(textColors) / sizeof(textColors[0]);
        currentColor = (currentColor - 1 + arraySize) % arraySize;
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
            currentState = Brightness;
        }
        else if (menuIndex == 1) // RESET
        {
            currentState = FPS;
        }
        else if (menuIndex == 2) // COLOR
        {
            currentState = Color;
        }
        else if (menuIndex == 3) // FPS
        {
            ESP.restart();
        }
    }
    else if (currentState == StationSelection)
    {
        AudioManager.startRadioStation(AudioManager.getCurrentRadioStation());
    }
    else if (currentState == Brightness)
    {
        AUTO_BRIGHTNESS = !AUTO_BRIGHTNESS;
    }
}

void MenuManager_::selectButtonLong()
{
    if (inMenu)
    {
        if (currentState == Brightness)
        {
            BRIGHTNESS = map(BRIGHTNESS_PERCENT, 0, 100, 0, 255);
            saveSettings();
        }
        else if (currentState == FPS)
        {
            DisplayManager.setFPS(MATRIX_FPS);
            saveSettings();
        }
        else if (currentState == Color)
        {
            TEXTCOLOR_565 = textColors[currentColor];
            saveSettings();
        }
        else if (currentState == MainMenu)
        {
            inMenu = false;
        }

        currentState = MainMenu;
    }
    else
    {
        inMenu = true;
    }
}