#include <MenuManager.h>
#include <Arduino.h>
#include <Globals.h>
#include <ServerManager.h>
#include <DisplayManager.h>
#include <PeripheryManager.h>
// #include <update.h>
#include <icons.h>
#include <UpdateManager.h>

String menuText;
int menuSelection;

int8_t stationIndex = 0;
bool isPlayingRadio = false;

enum MenuState
{
    MainMenu,
    BrightnessMenu,
    ColorMenu,
    SwitchMenu,
    TspeedMenu,
    AppTimeMenu,
    TimeFormatMenu,
    DateFormatMenu,
    WeekdayMenu,
    TempMenu,
    Appmenu,
#ifdef ULANZI
    SoundMenu
#else
    SoundMenu,
    VolumeMenu
#endif
};

const char *menuItems[] PROGMEM = {
    "BRIGHT",
    "COLOR",
    "SWITCH",
    "T-SPEED",
    "APPTIME",
    "TIME",
    "DATE",
    "WEEKDAY",
    "TEMP",
    "APPS",
    "SOUND",
#ifndef ULANZI
    "VOLUME",
#endif
    "UPDATE"};

int8_t menuIndex = 0;
#ifdef ULANZI
uint8_t menuItemCount = 12;
#else
uint8_t menuItemCount = 13;
#endif

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
    "%d.%m.",   // 01.04.
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

int convertBRIPercentTo8Bit(int brightness_percent)
{
    int brightness;
    if (brightness_percent <= 10) {
        // Map 10 % or lower 1:1 to 0:255 range. Reasons:
        // * 1% would be mapped to 2 so lowest value would be inaccessible.
        // * Small changes in lower brightness are perceived by humans
        //   as big changes, so it makes sense to give higher
        //   "resolution" here.
        brightness = brightness_percent;
    } else {
        brightness = map(brightness_percent, 0, 100, 0, 255);
    }
    return brightness;
}

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
    case ColorMenu:
        DisplayManager.drawMenuIndicator(currentColor, sizeof(textColors) / sizeof(textColors[0]), 0xFBC0);
        DisplayManager.setTextColor(textColors[currentColor]);
        return "0x" + String(textColors[currentColor], HEX);
    case SwitchMenu:
        return AUTO_TRANSITION ? "ON" : "OFF";
    case SoundMenu:
        return SOUND_ACTIVE ? "ON" : "OFF";
    case TspeedMenu:
        return String(TIME_PER_TRANSITION / 1000.0, 1) + "s";
    case AppTimeMenu:
        return String(TIME_PER_APP / 1000.0, 0) + "s";
    case TimeFormatMenu:
        DisplayManager.drawMenuIndicator(timeFormatIndex, timeFormatCount, 0xFBC0);

        char display[9];
        if (timeFormat[timeFormatIndex][2] == ' ')
        {
            snprintf(display, sizeof(display), "%s", timeFormat[timeFormatIndex]);
            display[2] = now % 2 ? ' ' : ':';
        }
        else
        {
            snprintf(display, sizeof(display), "%s", timeFormat[timeFormatIndex]);
        }

        strftime(t, sizeof(t), display, localtime(&now));
        return t;
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
            DisplayManager.drawBMP(0, 0, icon_13, 8, 8);
            return SHOW_TIME ? "ON" : "OFF";
        case 1:
            DisplayManager.drawBMP(0, 0, icon_1158, 8, 8);
            return SHOW_DATE ? "ON" : "OFF";
        case 2:
            DisplayManager.drawBMP(0, 0, icon_234, 8, 8);
            return SHOW_TEMP ? "ON" : "OFF";
        case 3:
            DisplayManager.drawBMP(0, 0, icon_2075, 8, 8);
            return SHOW_HUM ? "ON" : "OFF";
#ifdef ULANZI
        case 4:
            DisplayManager.drawBMP(0, 0, icon_1486, 8, 8);
            return SHOW_BAT ? "ON" : "OFF";
#endif
        default:
            break;
        }
        break;
#ifndef ULANZI
    case VolumeMenu:
        return String(VOLUME_PERCENT) + "%";
#endif
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
            BRIGHTNESS = convertBRIPercentTo8Bit(BRIGHTNESS_PERCENT);
            DisplayManager.setBrightness(BRIGHTNESS);
        }
        break;
    case ColorMenu:
        currentColor = (currentColor + 1) % (sizeof(textColors) / sizeof(textColors[0]));
        break;
    case SwitchMenu:
        AUTO_TRANSITION = !AUTO_TRANSITION;
        break;
    case TspeedMenu:
        TIME_PER_TRANSITION = min(2000, TIME_PER_TRANSITION + 100);
        break;
    case AppTimeMenu:
        TIME_PER_APP = min(static_cast<long>(30000), TIME_PER_APP + 1000);
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
    case SoundMenu:
        SOUND_ACTIVE = !SOUND_ACTIVE;
        break;
    case TempMenu:
        IS_CELSIUS = !IS_CELSIUS;
        break;
#ifndef ULANZI
    case VolumeMenu:
        if ((VOLUME_PERCENT + 1) > 100)
            VOLUME_PERCENT = 0;
        else
            VOLUME_PERCENT++;
#endif
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
            BRIGHTNESS = convertBRIPercentTo8Bit(BRIGHTNESS_PERCENT);
            DisplayManager.setBrightness(BRIGHTNESS);
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
        TIME_PER_APP = max(static_cast<long>(1000), TIME_PER_APP - 1000);
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
    case SoundMenu:
        SOUND_ACTIVE = !SOUND_ACTIVE;
        break;
#ifndef ULANZI
    case VolumeMenu:
        if ((VOLUME_PERCENT - 1) < 0)
            VOLUME_PERCENT = 100;
        else
            VOLUME_PERCENT--;
#endif
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
            // reverse of convertBRIPercentTo8Bit.
            if (BRIGHTNESS <= 10) {
                BRIGHTNESS_PERCENT = BRIGHTNESS;
            } else {
                BRIGHTNESS_PERCENT = map(BRIGHTNESS, 0, 255, 0, 100);
            }
            currentState = BrightnessMenu;
            break;
        case 1:
            currentState = ColorMenu;
            break;
        case 2:
            currentState = SwitchMenu;
            break;
        case 3:
            currentState = TspeedMenu;
            break;
        case 4:
            currentState = AppTimeMenu;
            break;
        case 5:
            currentState = TimeFormatMenu;
            break;
        case 6:
            currentState = DateFormatMenu;
            break;
        case 7:
            currentState = WeekdayMenu;
            break;
        case 8:
            currentState = TempMenu;
            break;
        case 9:
            currentState = Appmenu;
            break;
        case 10:
            currentState = SoundMenu;
            break;
        case 11:
#ifndef ULANZI
            currentState = VolumeMenu;
            break;
#endif
        case 12:
            if (UpdateManager.checkUpdate(true))
            {
                UpdateManager.updateFirmware();
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
            BRIGHTNESS = convertBRIPercentTo8Bit(BRIGHTNESS_PERCENT);
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
#ifdef ULANZI
        case 4:
            SHOW_BAT = !SHOW_BAT;
            break;
#endif
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
            //BRIGHTNESS = map(BRIGHTNESS_PERCENT, 0, 100, 0, 255);
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
            DATE_FORMAT = dateFormat[dateFormatIndex];
            saveSettings();
        case WeekdayMenu:
        case SoundMenu:
        case TempMenu:
            saveSettings();
            break;
        case Appmenu:
            DisplayManager.loadNativeApps();
            saveSettings();
            break;
#ifndef ULANZI
        case VolumeMenu:
            VOLUME = map(VOLUME_PERCENT, 0, 100, 0, 30);
            PeripheryManager.setVolume(VOLUME);
            saveSettings();
            break;
#endif
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
