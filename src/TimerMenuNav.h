#ifndef TimerMenuNav_h
#define TimerMenuNav_h

#include <Arduino.h>

// Display-free navigation state machine for the TIMER global menu.
// It owns the whole interaction model behind a small interface:
// list/leaf focus, the selected index, and the entry origin. Button inputs map
// to OUTCOMES that the device layer (MenuManager) acts on; the device keeps only
// drawing (the list indicator / the leaf value) and the single commit. Because
// it depends on nothing but Arduino.h, the interaction model is testable
// without a device.
//
// The list is the TIMER menu's slot rows. Value rows (enum / number / bool) drill
// into a leaf editor; the lone navigation row (MAIN) returns to the main menu.
// MAIN's index is supplied by the device from the slot table, so the state
// machine never hard-codes which row is MAIN.

enum class TimerNavFocus  : uint8_t { List, Editing };
enum class TimerNavOrigin : uint8_t { Menu, App };

// The kind of leaf a value row drills into. Most rows are a plain Value leaf
// (cycle/step/toggle). DURATION is a field editor (HH/MM/SS wheel) when the timer
// is Idle, and read-only otherwise -- the device maps slot+timer-state to this via
// timerMenuLeafKind(), so the gating decision lives in display-free code.
enum class TimerNavLeaf : uint8_t { Value, DurationEditable, DurationReadOnly };

// Outcomes the device layer acts on. Anything not listed (plain list movement) is
// reported as None; the caller reads focus()/index() for the new cursor position.
enum class TimerNavOutcome : uint8_t
{
    None,              // handled internally (list cursor moved / read-only no-op)
    AdjustValue,       // Editing, value leaf: caller applies timerMenuAdjust(index(), dir)
    AdjustField,       // Editing, DURATION leaf: caller steps the active H/M/S field
    CycleField,        // Editing, DURATION leaf, short press: cycle the H/M/S field
    EnterLeaf,         // List focus, value/duration row selected: drilled into its leaf
    ConfirmBackToList, // Editing, value leaf, short press: confirm, back to the list
    BackToList,        // Editing, long press (or read-only press): back to the list
    CommitDuration,    // Editing, DURATION leaf, long press: commit duration, back to list
    GoToMainMenu,      // commit + return to the main menu
    ExitMenu,          // commit + return to the Timer app (origin == App)
};

class TimerMenuNav
{
public:
    // Default: an empty single-item list in List focus. Use enter() before driving.
    TimerMenuNav();
    // itemCount = number of list rows; mainIndex = the MAIN (navigation) row.
    TimerMenuNav(uint8_t itemCount, uint8_t mainIndex,
                 TimerNavOrigin origin = TimerNavOrigin::Menu);

    // (Re)enter the list: reset to List focus, index 0, with fresh bounds + origin.
    // Called by the device whenever the TIMER menu is opened.
    void enter(uint8_t itemCount, uint8_t mainIndex, TimerNavOrigin origin);

    TimerNavFocus  focus() const { return _focus; }
    uint8_t        index() const { return _index; }
    TimerNavOrigin origin() const { return _origin; }
    bool           onMain() const { return _index == _mainIndex; }

    // left/right. List focus: move the cursor with wrap (-> None). Editing focus:
    // value leaf -> AdjustValue; DurationEditable -> AdjustField; DurationReadOnly
    // -> None (no-op).
    TimerNavOutcome navigate(int dir);

    // short press (middle button).
    //   List focus, value/duration row -> EnterLeaf (focus becomes Editing); pass
    //     the target row's leaf kind so the leaf behaves correctly once editing.
    //   List focus, MAIN  -> GoToMainMenu.
    //   Editing, value leaf            -> ConfirmBackToList (focus becomes List).
    //   Editing, DurationEditable leaf -> CycleField (stays Editing).
    //   Editing, DurationReadOnly leaf -> BackToList (any press returns to the list).
    TimerNavOutcome select(TimerNavLeaf leafKind = TimerNavLeaf::Value);

    // long press (middle button, held).
    //   List focus    -> context-aware exit: GoToMainMenu (origin Menu) or
    //                    ExitMenu (origin App).
    //   Editing, value leaf            -> BackToList (value already live in RAM).
    //   Editing, DurationEditable leaf -> CommitDuration (then back to the list).
    //   Editing, DurationReadOnly leaf -> BackToList.
    TimerNavOutcome back();

private:
    uint8_t        _itemCount;
    uint8_t        _mainIndex;
    uint8_t        _index;
    TimerNavFocus  _focus;
    TimerNavOrigin _origin;
    TimerNavLeaf   _leaf;   // kind of the leaf currently being edited (Editing focus)
};

#endif
