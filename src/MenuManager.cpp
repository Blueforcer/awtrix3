#include <MenuManager.h>
#include <Arduino.h>
#include <Globals.h>
#include <ServerManager.h>
#include <DisplayManager.h>
#include <updater.h>
#include <icons.h>

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
    TempMenu,
    Appmenu,
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
    "APPS",
    "UPDATE"};

int8_t menuIndex = 0;
uint8_t menuItemCount = 12;

const char *timeFormat[] PROGMEM = {
    "%H:%M:%S",
    "%l:%M:%S",
    "%H:%M",
    "%H %M",
    "%l:%M",
    "%l %M",
    "%l:%M %p",
    "%l %M %p"};
int8_t timeFormatIndex;
uint8_t timeFormatCount = 8;

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

const char *appsItems[][2] PROGMEM = {
    {"13", "time"},
    {"1158", "date"},
    {"234", "temp"},
    {"2075", "hum"},
    {"1486", "bat"}};

int8_t appsIndex;
uint8_t appsCount = 5;

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
    time_t now = time(nullptr);
    char t[20];
    char display[20];
    switch (currentState)
    {
    case MainMenu:
        DisplayManager.drawMenuIndicator(menuIndex, menuItemCount, 0xF800);
        return menuItems[menuIndex];
    case BrightnessMenu:
        return AUTO_BRIGHTNESS ? "AUTO" : String(BRIGHTNESS_PERCENT) + "%";
    case FPSMenu:
        return String(MATRIX_FPS) + " FPS";
    case ColorMenu:
        DisplayManager.drawMenuIndicator(currentColor, sizeof(textColors) / sizeof(textColors[0]), 0xFBC0);
        DisplayManager.setTextColor(textColors[currentColor]);
        return "0x" + String(textColors[currentColor], HEX);
    case SwitchMenu:
        return AUTO_TRANSITION ? "ON" : "OFF";
    case TspeedMenu:
        return String(TIME_PER_TRANSITION / 1000.0, 1) + "s";
    case AppTimeMenu:
        return String(TIME_PER_APP / 1000.0, 0) + "s";
    case TimeFormatMenu:
        DisplayManager.drawMenuIndicator(timeFormatIndex, timeFormatCount, 0xFBC0);
        if (timeFormat[timeFormatIndex][2] == ' ')
        {
            strcpy(display, timeFormat[timeFormatIndex]);
            if (now % 2)
            {
                display[2] = ' ';
            }
            else
            {
                display[2] = ':';
            }
            strftime(t, sizeof(t), display, localtime(&now));
            return t;
        }
        else
        {
            strftime(t, sizeof(t), timeFormat[timeFormatIndex], localtime(&now));
            return t;
        }

    case DateFormatMenu:
        DisplayManager.drawMenuIndicator(dateFormatIndex, dateFormatCount, 0xFBC0);
        strftime(t, sizeof(t), dateFormat[dateFormatIndex], localtime(&now));
        return t;
    case WeekdayMenu:
        return START_ON_MONDAY ? "MON" : "SUN";
    case TempMenu:
        return IS_CELSIUS ? "°C" : "°F";
    case Appmenu:
        DisplayManager.drawMenuIndicator(appsIndex, appsCount, 0xFBC0);
        switch (appsIndex)
        {
        case 0:
            DisplayManager.drawBMP(0, 0, get_icon(13), 8, 8);
            return SHOW_TIME ? "ON" : "OFF";
        case 1:
            DisplayManager.drawBMP(0, 0, get_icon(1158), 8, 8);
            return SHOW_DATE ? "ON" : "OFF";
        case 2:
            DisplayManager.drawBMP(0, 0, get_icon(234), 8, 8);
            return SHOW_TEMP ? "ON" : "OFF";
        case 3:
            DisplayManager.drawBMP(0, 0, get_icon(2075), 8, 8);
            return SHOW_HUM ? "ON" : "OFF";
        case 4:
            DisplayManager.drawBMP(0, 0, get_icon(1486), 8, 8);
            return SHOW_BAT ? "ON" : "OFF";
        default:
            break;
        }
        break;
    default:
        break;
    }
    return "";
}

void MenuManager_::rightButton()
{
    if (!inMenu)
        return;
    switch (currentState)
    {
    case MainMenu:
        menuIndex = (menuIndex + 1) % menuItemCount;
        break;
    case BrightnessMenu:
        if (!AUTO_BRIGHTNESS)
        {
            BRIGHTNESS_PERCENT = (BRIGHTNESS_PERCENT % 100) + 1;
            BRIGHTNESS = map(BRIGHTNESS_PERCENT, 0, 100, 0, 255);
            DisplayManager.setBrightness(BRIGHTNESS);
        }
        break;
    case FPSMenu:
        if (MATRIX_FPS < 30)
            ++MATRIX_FPS;
        break;
    case ColorMenu:
        currentColor = (currentColor + 1) % (sizeof(textColors) / sizeof(textColors[0]));
        break;
    case SwitchMenu:
        AUTO_TRANSITION = !AUTO_TRANSITION;
        break;
    case TspeedMenu:
        TIME_PER_TRANSITION = min(1200, TIME_PER_TRANSITION + 100);
        break;
    case AppTimeMenu:
        TIME_PER_APP = min(30000, TIME_PER_APP + 1000);
        break;
    case TimeFormatMenu:
        timeFormatIndex = (timeFormatIndex + 1) % timeFormatCount;
        break;
    case DateFormatMenu:
        dateFormatIndex = (dateFormatIndex + 1) % dateFormatCount;
        break;
    case Appmenu:
        appsIndex = (appsIndex + 1) % appsCount;
        break;
    case WeekdayMenu:
        START_ON_MONDAY = !START_ON_MONDAY;
        break;
    case TempMenu:
        IS_CELSIUS = !IS_CELSIUS;
        break;
    default:
        break;
    }
}

void MenuManager_::leftButton()
{
    if (!inMenu)
    {
        return;
    }
    switch (currentState)
    {
    case MainMenu:
        menuIndex = (menuIndex == 0) ? menuItemCount - 1 : menuIndex - 1;
        break;
    case BrightnessMenu:
        if (!AUTO_BRIGHTNESS)
        {
            BRIGHTNESS_PERCENT = (BRIGHTNESS_PERCENT == 1) ? 100 : BRIGHTNESS_PERCENT - 1;
            BRIGHTNESS = map(BRIGHTNESS_PERCENT, 0, 100, 0, 255);
            DisplayManager.setBrightness(BRIGHTNESS);
        }
        break;
    case FPSMenu:
        if (MATRIX_FPS > 15)
        {
            MATRIX_FPS--;
        }
        break;
    case ColorMenu:
        currentColor = (currentColor + sizeof(textColors) / sizeof(textColors[0]) - 1) % (sizeof(textColors) / sizeof(textColors[0]));
        break;
    case SwitchMenu:
        AUTO_TRANSITION = !AUTO_TRANSITION;
        break;
    case TspeedMenu:
        TIME_PER_TRANSITION = max(200, TIME_PER_TRANSITION - 100);
        break;
    case AppTimeMenu:
        TIME_PER_APP = max(1000, TIME_PER_APP - 1000);
        break;
    case TimeFormatMenu:
        timeFormatIndex = (timeFormatIndex == 0) ? timeFormatCount - 1 : timeFormatIndex - 1;
        break;
    case DateFormatMenu:
        dateFormatIndex = (dateFormatIndex == 0) ? dateFormatCount - 1 : dateFormatIndex - 1;
        break;
    case Appmenu:
        appsIndex = (appsIndex == 0) ? appsCount - 1 : appsIndex - 1;
        break;
    case WeekdayMenu:
        START_ON_MONDAY = !START_ON_MONDAY;
        break;
    case TempMenu:
        IS_CELSIUS = !IS_CELSIUS;
        break;
    default:
        break;
    }
}

void MenuManager_::selectButton()
{
    if (!inMenu)
    {
        return;
    }
    switch (currentState)
    {
    case MainMenu:
        switch (menuIndex)
        {
        case 0:
            BRIGHTNESS_PERCENT = map(BRIGHTNESS, 0, 255, 0, 100);
            currentState = BrightnessMenu;
            break;
        case 1:
            currentState = FPSMenu;
            break;
        case 2:
            currentState = ColorMenu;
            break;
        case 3:
            currentState = SwitchMenu;
            break;
        case 4:
            currentState = TspeedMenu;
            break;
        case 5:
            currentState = AppTimeMenu;
            break;
        case 6:
            currentState = TimeFormatMenu;
            break;
        case 7:
            currentState = DateFormatMenu;
            break;
        case 8:
            currentState = WeekdayMenu;
            break;
        case 9:
            currentState = TempMenu;
            break;
        case 10:
            currentState = Appmenu;
            break;
        case 11:
            if (FirmwareVersionCheck())
            {
                updateFirmware();
            }
            break;
        default:
            break;
        }
        break;
    case BrightnessMenu:
        AUTO_BRIGHTNESS = !AUTO_BRIGHTNESS;
        if (!AUTO_BRIGHTNESS)
        {
            BRIGHTNESS = map(BRIGHTNESS_PERCENT, 0, 100, 0, 255);
            DisplayManager.setBrightness(BRIGHTNESS);
        }
        break;
    case Appmenu:
        switch (appsIndex)
        {
        case 0:
            SHOW_TIME = !SHOW_TIME;
            break;
        case 1:
            SHOW_DATE = !SHOW_DATE;
            break;
        case 2:
            SHOW_TEMP = !SHOW_TEMP;
            break;
        case 3:
            SHOW_HUM = !SHOW_HUM;
            break;
        case 4:
            SHOW_BAT = !SHOW_BAT;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

void MenuManager_::selectButtonLong()
{
    if (inMenu)
    {
        switch (currentState)
        {
        case BrightnessMenu:
            BRIGHTNESS = map(BRIGHTNESS_PERCENT, 0, 100, 0, 255);
            saveSettings();
            break;
        case FPSMenu:
            DisplayManager.setFPS(MATRIX_FPS);
            saveSettings();
            break;
        case ColorMenu:
            TEXTCOLOR_565 = textColors[currentColor];
            saveSettings();
            break;
        case MainMenu:
            inMenu = false;
            break;
        case SwitchMenu:
            DisplayManager.setAutoTransition(AUTO_TRANSITION);
            saveSettings();
            break;
        case TspeedMenu:
        case AppTimeMenu:
            DisplayManager.applyAllSettings();
            saveSettings();
            break;
        case TimeFormatMenu:
            TIME_FORMAT = timeFormat[timeFormatIndex];
            saveSettings();
            break;
        case DateFormatMenu:
        case WeekdayMenu:
        case TempMenu:
            saveSettings();
            break;
        case Appmenu:
            DisplayManager.loadNativeApps();
            saveSettings();
            break;
        default:
            break;
        }
        currentState = MainMenu;
    }
    else
    {
        inMenu = true;
    }
}
