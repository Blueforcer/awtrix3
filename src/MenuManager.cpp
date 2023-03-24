#include <MenuManager.h>
#include <Arduino.h>
#include <Globals.h>
#include <ServerManager.h>
#include <DisplayManager.h>
#include <updater.h>

String menuText;
int menuSelection;

int8_t stationIndex = 0;
bool isPlayingRadio = false;

enum MenuState
{
    MainMenu,
    BrightnessMenu,
    FPSMenu,
    ColorMenu,
    SwitchMenu,
    TspeedMenu,
    AppTimeMenu,
    TimeFormatMenu,
    DateFormatMenu,
    WeekdayMenu,
    TempMenu
};

const char *menuItems[] PROGMEM = {
    "BRIGHT",
    "FPS",
    "COLOR",
    "SWITCH",
    "T-SPEED",
    "APPTIME",
    "TIME",
    "DATE",
    "WEEKDAY",
    "TEMP",
    "UPDATE"};

int8_t menuIndex = 0;
uint8_t menuItemCount = 11;

const char *timeFormat[] PROGMEM = {
    "%H:%M:%S",
    "%l:%M:%S",
    "%H:%M",
    "%l:%M",
    "%l:%M %p"};
int8_t timeFormatIndex;
uint8_t timeFormatCount = 5;

const char *dateFormat[] PROGMEM = {
    "%d.%m.%y", // 01.04.22
    "%d.%m",    // 01.04
    "%y-%m-%d", // 22-04-01
    "%m-%d",    // 04-01
    "%m/%d/%y", // 04/01/22
    "%m/%d",    // 04/01
    "%d/%m/%y", // 01/04/22
    "%d/%m",    // 01/04
    "%m-%d-%y", // 04-01-22
};
int8_t dateFormatIndex;
uint8_t dateFormatCount = 9;

MenuState currentState = MainMenu;

uint16_t textColors[] PROGMEM = {
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
        DisplayManager.drawMenuIndicator(menuIndex, menuItemCount);
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
    }
    else if (currentState == AppTimeMenu)
    {
        float seconds = (float)TIME_PER_APP / 1000.0;
        return String(seconds, 0) + "s";
    }
    else if (currentState == TimeFormatMenu)
    {
        time_t now = time(nullptr);
        char t[20];
        strftime(t, sizeof(t), timeFormat[timeFormatIndex], localtime(&now));
        return t;
    }
    else if (currentState == DateFormatMenu)
    {
        time_t now = time(nullptr);
        char t[20];
        strftime(t, sizeof(t), dateFormat[dateFormatIndex], localtime(&now));
        return t;
    }
    else if (currentState == WeekdayMenu)
    {
        return START_ON_MONDAY ? "MON" : "SUN";
    }
    else if (currentState == TempMenu)
    {
        return IS_CELSIUS ? "°C" : "°F";
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
    else if (currentState == TimeFormatMenu)
    {
        timeFormatIndex++;
        if (timeFormatIndex > timeFormatCount - 1)
        {
            timeFormatIndex = 0; // Wrap around to the first menu item
        }
    }
    else if (currentState == DateFormatMenu)
    {
        dateFormatIndex++;
        if (dateFormatIndex > dateFormatCount - 1)
        {
            dateFormatIndex = 0; // Wrap around to the first menu item
        }
    }
    else if (currentState == WeekdayMenu)
    {
        START_ON_MONDAY = !START_ON_MONDAY;
    }
    else if (currentState == TempMenu)
    {
        IS_CELSIUS = !IS_CELSIUS;
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
    else if (currentState == TimeFormatMenu)
    {
        timeFormatIndex--;
        if (timeFormatIndex < 0)
        {
            timeFormatIndex = timeFormatCount - 1;
        }
    }
    else if (currentState == DateFormatMenu)
    {
        dateFormatIndex--;
        if (dateFormatIndex < 0)
        {
            dateFormatIndex = dateFormatCount - 1;
        }
    }
    else if (currentState == WeekdayMenu)
    {
        START_ON_MONDAY = !START_ON_MONDAY;
    }
    else if (currentState == TempMenu)
    {
        IS_CELSIUS = !IS_CELSIUS;
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
        else if (menuIndex == 6) // Time
        {
            currentState = TimeFormatMenu;
        }
        else if (menuIndex == 7) // date
        {
            currentState = DateFormatMenu;
        }
        else if (menuIndex == 8) // weekday
        {
            currentState = WeekdayMenu;
        }
        else if (menuIndex == 9) // temp
        {
            currentState = TempMenu;
        }
        else if (menuIndex == 10) // Updater
        {
            if (FirmwareVersionCheck())
                updateFirmware();
        }
    }
    else if (currentState == BrightnessMenu)
    {
        AUTO_BRIGHTNESS = !AUTO_BRIGHTNESS;
        if (!AUTO_BRIGHTNESS)
        {
            BRIGHTNESS = map(BRIGHTNESS_PERCENT, 0, 100, 0, 255);
            DisplayManager.setBrightness(BRIGHTNESS);
        }
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
        else if (currentState == TimeFormatMenu)
        {
            TIME_FORMAT = timeFormat[timeFormatIndex];
            saveSettings();
        }
        else if (currentState == DateFormatMenu)
        {
            DATE_FORMAT = dateFormat[dateFormatIndex];
            saveSettings();
        }
        else if (currentState == WeekdayMenu)
        {
            saveSettings();
        }
        else if (currentState == TempMenu)
        {
            saveSettings();
        }
        currentState = MainMenu;
    }
    else
    {
        inMenu = true;
    }
}