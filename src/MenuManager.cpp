#include <MenuManager.h>
#include <Arduino.h>
#include <Globals.h>
#include <ServerManager.h>
#include <DisplayManager.h>
#include <PeripheryManager.h>
#include "timer.h"
#ifndef AWTRIX_DISABLE_TIMER
#include "TimerManager.h"
#include "TimerMenu.h"
#include "TimerMenuNav.h"
#include "TimerConfigEditor.h"
#endif
#include "MQTTManager.h"
#ifndef AWTRIX_DISABLE_TIMER
#include "TimerHaHost.h"
#endif
#include <icons.h>
#include <UpdateManager.h>
#include "Functions.h"   // getTextWidth (centering the duration leaf + its underline)

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
#ifndef AWTRIX_DISABLE_TIMER
    TimerConfigMenu,
#endif
    Appmenu,
    SoundMenu,
    VolumeMenu,
    UpdateMenu,
    MaxMenu
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
#ifndef AWTRIX_DISABLE_TIMER
    "TIMER",
#endif
    "APPS",
    "SOUND",
    "VOLUME",
    "UPDATE"};

int8_t menuIndex = 0;
uint8_t menuItemCount = MaxMenu - 1;

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
#ifndef AWTRIX_DISABLE_TIMER
#ifndef awtrix2_upgrade
uint8_t appsCount = 6;
#else
uint8_t appsCount = 5;
#endif
#else
#ifndef awtrix2_upgrade
uint8_t appsCount = 5;
#else
uint8_t appsCount = 4;
#endif
#endif

#ifndef AWTRIX_DISABLE_TIMER
uint8_t timerConfigCount = TIMER_MENU_SLOT_COUNT;
// The TIMER menu's drill-in navigation state machine. MAIN
// is the last slot (a Navigation row); the device keeps only drawing + the commit.
TimerMenuNav timerNav(TIMER_MENU_SLOT_COUNT, TIMER_MENU_SLOT_COUNT - 1);

// The DURATION leaf reuses the existing display-free duration edit engine,
// a menu-owned instance. The editor has no auto-apply timeout; the menu only
// drives its hold-to-repeat, and the edited duration commits via setDuration on
// leaf back-out (run-state, not the list -> main config batch).
TimerConfigEditor timerDurationEditor;

// Is the cursor on the DURATION row?
static bool timerNavOnDuration()
{
    return TIMER_MENU_SLOTS[timerNav.index()].kind == TimerMenuKind::Duration;
}

// The one-shot TIMER-menu commit: a single PersistBatch window (enum edits
// deferred during scroll in "timer" ns + table-row knob/toggle keys in "awtrix"
// ns), then the HA attribute republish. Fires once on the list -> main-menu
// transition (long-press out of the list, or selecting MAIN). A config edit
// never propagates to peers on its own (run-scoped config mirror): config
// travels only bundled with a `start`.
static void commitTimerMenu()
{
    {
        TimerManager_::PersistBatch batch(TimerManager);
        batch.markTableDirty();
    }
    TimerManager.publishAllAttributeGroups();
}
#endif

MenuState currentState = MainMenu;

uint32_t textColors[] = {
    0xFFFFFF, // White
    0xFF0000, // Red
    0x00FF00, // Green
    0x0000FF, // Blue
    0xFFFF00, // Yellow
    0xFF00FF, // Magenta
    0x00FFFF, // Cyan
    0xFFA500, // Orange
    0x800080, // Purple
    0x008080, // Teal
    0x808000, // Olive
    0x800000, // Maroon
    0x008000, // Dark Green
    0x000080, // Navy
    0x808080  // Gray
};

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
    if (brightness_percent <= 10)
    {
        // Map 10 % or lower 1:1 to 0:255 range. Reasons:
        // * 1% would be mapped to 2 so lowest value would be inaccessible.
        // * Small changes in lower brightness are perceived by humans
        //   as big changes, so it makes sense to give higher
        //   "resolution" here.
        brightness = brightness_percent;
    }
    else
    {
        brightness = map(brightness_percent, 0, 100, 0, 255);
    }
    return brightness;
}

String MenuManager_::menutext()
{
    char t[20];
    char display[20];
    switch (currentState)
    {
    case MainMenu:
        DisplayManager.drawMenuIndicator(menuIndex, menuItemCount, 0xF80000);
        return menuItems[menuIndex];
    case BrightnessMenu:
        return AUTO_BRIGHTNESS ? "AUTO" : String(BRIGHTNESS_PERCENT) + "%";
    case ColorMenu:
        DisplayManager.drawMenuIndicator(currentColor, sizeof(textColors) / sizeof(textColors[0]), 0xFBC000);
        DisplayManager.setTextColor(textColors[currentColor]);
        return "0X" + String(textColors[currentColor], HEX);
    case SwitchMenu:
        return AUTO_TRANSITION ? "ON" : "OFF";
    case SoundMenu:
        return SOUND_ACTIVE ? "ON" : "OFF";
    case TspeedMenu:
        return String(TIME_PER_TRANSITION / 1000.0, 1) + "s";
    case AppTimeMenu:
        return String(TIME_PER_APP / 1000.0, 0) + "s";
    case TimeFormatMenu:
        DisplayManager.drawMenuIndicator(timeFormatIndex, timeFormatCount, 0xFBC000);

        char display[9];
        if (timeFormat[timeFormatIndex][2] == ' ')
        {
            snprintf(display, sizeof(display), "%s", timeFormat[timeFormatIndex]);
            display[2] = timer_time() % 2 ? ' ' : ':';
        }
        else
        {
            snprintf(display, sizeof(display), "%s", timeFormat[timeFormatIndex]);
        }

        strftime(t, sizeof(t), display, timer_localtime());
        return t;
    case DateFormatMenu:
        DisplayManager.drawMenuIndicator(dateFormatIndex, dateFormatCount, 0xFBC000);
        strftime(t, sizeof(t), dateFormat[dateFormatIndex], timer_localtime());
        return t;
    case WeekdayMenu:
        return START_ON_MONDAY ? "MON" : "SUN";
    case TempMenu:
        return IS_CELSIUS ? "°C" : "°F";
    case Appmenu:
        DisplayManager.drawMenuIndicator(appsIndex, appsCount, 0xFBC000);
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
#ifndef awtrix2_upgrade
        case 4:
            DisplayManager.drawBMP(0, 0, icon_1486, 8, 8);
            return SHOW_BAT ? "ON" : "OFF";
#endif
#ifndef AWTRIX_DISABLE_TIMER
#ifndef awtrix2_upgrade
        case 5:
#else
        case 4:
#endif
            DisplayManager.drawBMP(0, 0, icon_timer, 8, 8);
            return SHOW_TIMER ? "ON" : "OFF";
#endif
        default:
            break;
        }
        break;
    case VolumeMenu:
        if (!(DFPLAYER_ACTIVE || BUZ_VOL))
        {
            return "N/A";
        }
        else
        {
            return String(SOUND_VOLUME);
        }
#ifndef AWTRIX_DISABLE_TIMER
    case TimerConfigMenu:
        // List focus: walk the named items (indicator over the list). Leaf focus:
        // show the bare value only, no indicator.
        if (timerNav.focus() == TimerNavFocus::List)
        {
            DisplayManager.drawMenuIndicator(timerNav.index(), timerConfigCount, 0xFBC000);
            return timerMenuName(timerNav.index());
        }
        // DURATION leaf: HH:MM:SS wheel with the active-field underline when
        // editable; the static value (no underline) when read-only.
        if (timerNavOnDuration())
        {
            if (timerDurationEditor.isActive())
            {
                // Drive the editor's hold-to-repeat from the raw button reads each
                // frame. The menu is timeout-free and the editor no longer has an
                // auto-apply timeout, so there is nothing else to handle.
                EasyButton *bL = PeripheryManager.buttonL;
                EasyButton *bR = PeripheryManager.buttonR;
                TimerConfigEditor::ButtonState buttons{bL && bL->isPressed(),
                                                       bR && bR->isPressed()};
                timerDurationEditor.tick(millis(), buttons);

                snprintf(t, sizeof(t), "%02u:%02u:%02u",
                         (unsigned)timerDurationEditor.hh(),
                         (unsigned)timerDurationEditor.mm(),
                         (unsigned)timerDurationEditor.ss());
                // Active-field underline, aligned under the centered HH:MM:SS (the
                // same step/geometry the Timer-app config screen uses).
                int16_t textX = (32 - (int)getTextWidth(t, 2)) / 2;
                int16_t ux = textX + timerDurationEditor.field() * 10;
                DisplayManager.drawLine(ux, 7, ux + 7, 7, TEXTCOLOR_888);
                return String(t);
            }
            return timerMenuValue(timerNav.index());   // read-only: current value
        }
        return timerMenuValue(timerNav.index());
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
    case VolumeMenu:
        if (!(DFPLAYER_ACTIVE || BUZ_VOL))
            break;
        if ((SOUND_VOLUME + 1) > 30)
            SOUND_VOLUME = 0;
        else
            SOUND_VOLUME++;
        break;
#ifndef AWTRIX_DISABLE_TIMER
    case TimerConfigMenu:
    {
        // List focus walks the list; a value leaf steps its value live; the
        // DURATION leaf steps the active H/M/S field; a read-only leaf is a no-op.
        TimerNavOutcome o = timerNav.navigate(+1);
        if (o == TimerNavOutcome::AdjustValue)
            timerMenuAdjust(timerNav.index(), +1);
        else if (o == TimerNavOutcome::AdjustField)
            timerDurationEditor.adjust(+1);
        break;
    }
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
    case VolumeMenu:
        if (!(DFPLAYER_ACTIVE || BUZ_VOL))
            break;
        if ((SOUND_VOLUME - 1) < 0)
            SOUND_VOLUME = 30;
        else
            SOUND_VOLUME--;
        break;
#ifndef AWTRIX_DISABLE_TIMER
    case TimerConfigMenu:
    {
        TimerNavOutcome o = timerNav.navigate(-1);
        if (o == TimerNavOutcome::AdjustValue)
            timerMenuAdjust(timerNav.index(), -1);
        else if (o == TimerNavOutcome::AdjustField)
            timerDurationEditor.adjust(-1);
        break;
    }
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
        currentState = (MenuState)(menuIndex + 1);
        switch (currentState)
        {
        case BrightnessMenu:
            // reverse of convertBRIPercentTo8Bit.
            if (BRIGHTNESS <= 10)
            {
                BRIGHTNESS_PERCENT = BRIGHTNESS;
            }
            else
            {
                BRIGHTNESS_PERCENT = map(BRIGHTNESS, 0, 255, 0, 100);
            }
            break;
        case UpdateMenu:
            if (UpdateManager.checkUpdate(true))
            {
                UpdateManager.updateFirmware();
            }
            break;
#ifndef AWTRIX_DISABLE_TIMER
        case TimerConfigMenu:
            // Open the TIMER menu at the top of the list (origin = main menu).
            timerNav.enter(TIMER_MENU_SLOT_COUNT, TIMER_MENU_SLOT_COUNT - 1,
                           TimerNavOrigin::Menu);
            break;
#endif
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
#ifndef AWTRIX_DISABLE_TIMER
    case TimerConfigMenu:
        // Short press.
        if (timerNav.focus() == TimerNavFocus::List)
        {
            // Drill in (passing how the target leaf behaves), commit + leave on MAIN.
            TimerNavLeaf leaf = timerMenuLeafKind(timerNav.index(), TimerManager.getState());
            TimerNavOutcome o = timerNav.select(leaf);
            if (o == TimerNavOutcome::GoToMainMenu)
            {
                commitTimerMenu();
                currentState = MainMenu;
            }
            else if (o == TimerNavOutcome::EnterLeaf && leaf == TimerNavLeaf::DurationEditable)
            {
                timerDurationEditor.enter(TimerManager.getDuration());
            }
        }
        else if (timerNav.select() == TimerNavOutcome::CycleField)
        {
            // DURATION leaf: cycle H -> M -> S (value leaves just confirm back).
            timerDurationEditor.cycleField();
        }
        break;
#endif
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
#ifndef awtrix2_upgrade
        case 4:
            SHOW_BAT = !SHOW_BAT;
            break;
#endif
#ifndef AWTRIX_DISABLE_TIMER
#ifndef awtrix2_upgrade
        case 5:
#else
        case 4:
#endif
        {
            bool prev = SHOW_TIMER;
            SHOW_TIMER = !SHOW_TIMER;
            TimerManager.onShowTimerChange(prev, SHOW_TIMER);
            if (prev && !SHOW_TIMER)
                TimerHaHost.remove();
            else if (!prev && SHOW_TIMER)
                TimerHaHost.enable();
            break;
        }
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
        if (GAME_ACTIVE)
        {
            currentState = MainMenu;
            inMenu = false;
            GAME_ACTIVE = false;
            return;
        }
        switch (currentState)
        {
        case BrightnessMenu:
            // BRIGHTNESS = map(BRIGHTNESS_PERCENT, 0, 100, 0, 255);
            saveSettings();
            break;
        case ColorMenu:
            TEXTCOLOR_888 = textColors[currentColor];
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
        case VolumeMenu:
            PeripheryManager.setVolume(SOUND_VOLUME);
            saveSettings();
            break;
#ifndef AWTRIX_DISABLE_TIMER
        case TimerConfigMenu:
        {
            // Long press: in a value/read-only leaf it just steps back up to the
            // list (value already live in RAM, no commit). In the DURATION leaf it
            // commits the edited duration (run-state, separate from the config
            // batch) then returns to the list. Out of the list it is the single
            // commit seam: main menu (origin = menu) or back to the Timer app
            // (origin = app).
            TimerNavOutcome o = timerNav.back();
            if (o == TimerNavOutcome::CommitDuration)
            {
                // The normal set-duration commit path. A bare duration edit
                // propagates NOTHING: duration rides only with a start, so an
                // on-device length edit no longer moves a follower's displayed time.
                TimerManager.setDuration(timerDurationEditor.exit());
                return;                 // stay in the TIMER menu, list focus
            }
            if (o == TimerNavOutcome::BackToList)
                return;                 // stay in the TIMER menu, list focus
            commitTimerMenu();          // GoToMainMenu / ExitMenu: commit once
            if (o == TimerNavOutcome::ExitMenu)
            {
                // Entered from the Timer app: close the menu so the app reappears.
                inMenu = false;
                currentState = MainMenu;
                return;
            }
            break;                      // GoToMainMenu: falls through to MainMenu
        }
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

#ifndef AWTRIX_DISABLE_TIMER
void MenuManager_::openTimerMenuFromApp()
{
    // Open the TIMER menu directly at the top of the list, origin = App so a
    // long-press out of the list returns to the Timer app.
    inMenu = true;
    currentState = TimerConfigMenu;
    timerNav.enter(TIMER_MENU_SLOT_COUNT, TIMER_MENU_SLOT_COUNT - 1, TimerNavOrigin::App);
}
#endif
