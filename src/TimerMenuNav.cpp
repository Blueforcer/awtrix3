#ifndef AWTRIX_DISABLE_TIMER
#include "TimerMenuNav.h"

// Display-free TIMER-menu navigation state machine.
// See the header for the input->outcome contract. The implementation is pure logic
// over {focus, index, origin, leaf, bounds} -- no device, no display, no storage.

TimerMenuNav::TimerMenuNav()
    : _itemCount(1), _mainIndex(0), _index(0),
      _focus(TimerNavFocus::List), _origin(TimerNavOrigin::Menu),
      _leaf(TimerNavLeaf::Value) {}

TimerMenuNav::TimerMenuNav(uint8_t itemCount, uint8_t mainIndex, TimerNavOrigin origin)
    : _itemCount(itemCount ? itemCount : 1),
      _mainIndex(mainIndex),
      _index(0),
      _focus(TimerNavFocus::List),
      _origin(origin),
      _leaf(TimerNavLeaf::Value) {}

void TimerMenuNav::enter(uint8_t itemCount, uint8_t mainIndex, TimerNavOrigin origin)
{
    _itemCount = itemCount ? itemCount : 1;
    _mainIndex = mainIndex;
    _index     = 0;
    _focus     = TimerNavFocus::List;
    _origin    = origin;
    _leaf      = TimerNavLeaf::Value;
}

TimerNavOutcome TimerMenuNav::navigate(int dir)
{
    if (_focus == TimerNavFocus::Editing)
    {
        // The leaf value lives in the data model / edit engine; the caller applies
        // the step. DURATION steps the active field; a read-only leaf ignores it.
        switch (_leaf)
        {
            case TimerNavLeaf::Value:            return TimerNavOutcome::AdjustValue;
            case TimerNavLeaf::DurationEditable: return TimerNavOutcome::AdjustField;
            case TimerNavLeaf::DurationReadOnly: return TimerNavOutcome::None;
        }
        return TimerNavOutcome::None;
    }

    // List focus: move the cursor with wrap.
    if (dir > 0)
        _index = (uint8_t)((_index + 1) % _itemCount);
    else
        _index = (uint8_t)((_index == 0) ? _itemCount - 1 : _index - 1);
    return TimerNavOutcome::None;
}

TimerNavOutcome TimerMenuNav::select(TimerNavLeaf leafKind)
{
    if (_focus == TimerNavFocus::Editing)
    {
        switch (_leaf)
        {
            case TimerNavLeaf::Value:
                // Short press in a value leaf: confirm and step back up to the list.
                _focus = TimerNavFocus::List;
                return TimerNavOutcome::ConfirmBackToList;
            case TimerNavLeaf::DurationEditable:
                // Short press in the duration wheel: cycle H -> M -> S, stay editing.
                return TimerNavOutcome::CycleField;
            case TimerNavLeaf::DurationReadOnly:
                // Read-only: any press returns to the list.
                _focus = TimerNavFocus::List;
                return TimerNavOutcome::BackToList;
        }
    }

    // List focus.
    if (onMain())
        return TimerNavOutcome::GoToMainMenu;   // commit handled by the device

    _leaf  = leafKind;                          // remember how the leaf behaves
    _focus = TimerNavFocus::Editing;            // drill into the leaf
    return TimerNavOutcome::EnterLeaf;
}

TimerNavOutcome TimerMenuNav::back()
{
    if (_focus == TimerNavFocus::Editing)
    {
        TimerNavLeaf leaf = _leaf;
        _focus = TimerNavFocus::List;           // every leaf back-out returns to the list
        // The duration leaf commits its run-state value on back-out; value leaves
        // are already live in RAM (committed with the list -> main batch).
        return (leaf == TimerNavLeaf::DurationEditable)
                   ? TimerNavOutcome::CommitDuration
                   : TimerNavOutcome::BackToList;
    }

    // List focus: context-aware exit by entry origin.
    return (_origin == TimerNavOrigin::App)
               ? TimerNavOutcome::ExitMenu
               : TimerNavOutcome::GoToMainMenu;
}
#endif // AWTRIX_DISABLE_TIMER
