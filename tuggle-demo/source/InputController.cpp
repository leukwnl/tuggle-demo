//
//  InputController.cpp
//
//

#include "InputController.h"
using namespace cugl;

// Define the static instance variable
InputController* InputController::_instance = nullptr;

// Define constants
/** How much time must pass for a double tap (in milliseconds) */
#define DEFAULT_DOUBLE_TAP_TIME 400
/** Minimum time for a tap-and-hold gesture (in milliseconds) */
#define DEFAULT_TAP_HOLD_TIME 500
/** Minimum distance for a swipe gesture */
#define DEFAULT_SWIPE_MIN_DISTANCE 50.0f
/** Maximum time for a swipe gesture (in milliseconds) */
#define DEFAULT_SWIPE_MAX_TIME 300
/** Minimum distance to consider a drag (in pixels) */
#define DEFAULT_DRAG_THRESHOLD 2.0f

/**
 * Creates a new input controller.
 *
 * This constructor does NOT do any initialization. It simply allocates the
 * object. All initialization is done via the init() method.
 */
InputController::InputController():
_active(false),
_mouse(nullptr),
_keyboard(nullptr),
_touch(nullptr),
_pointerDown(false),
_tapped(false),
_doubleTapped(false),
_dragging(false),
_dragStarted(false),
_dragEnded(false),
_tapHoldDetected(false),
_swipeDetected(false),
_moving(false) {
    // Initialize vectors to zero
    _currPos.setZero();
    _prevPos.setZero();
    _startPos.setZero();
    _swipeVelocity.setZero();
}

/**
 * Initializes the input controller.
 *
 * This method works like a proper constructor, initializing the input
 * controller and allocating memory. However, it does not activate the
 * listeners. You must call start() to do that.
 *
 * @return true if initialization was successful
 */
bool InputController::init() {
    _lastTapTime.mark();
    _pressStartTime.mark();
    
    bool success = true;
    
#ifndef CU_TOUCH_SCREEN
    // Initialize keyboard if available
    success = Input::activate<Keyboard>();
    if (success) {
        _keyboard = Input::get<Keyboard>();
    }
    
    // Initialize mouse if available
    success = Input::activate<Mouse>();
    if (success) {
        _mouse = Input::get<Mouse>();
        // Set pointer awareness to ALWAYS to capture all movement
        _mouse->setPointerAwareness(Mouse::PointerAwareness::ALWAYS);
    }
#else
    // Initialize touch if available
    success = Input::activate<Touchscreen>();
    if (success) {
        _touch = Input::get<Touchscreen>();
    }
#endif
    Input::activate<TextInput>();
    
    return success;
}

/**
 * Starts the input processing for this controller.
 *
 * This method activates the input devices and attaches the listeners.
 *
 * @return true if the controller was successfully started
 */
bool InputController::start() {
    if (_active) {
        return false;
    }
    
#ifndef CU_TOUCH_SCREEN
    if (_mouse != nullptr) {
        _mouse->addPressListener(LISTENER_KEY, [this](const MouseEvent& event, Uint8 clicks, bool focus) {
            this->mousePressedCB(event, clicks, focus);
        });
        
        _mouse->addReleaseListener(LISTENER_KEY, [this](const MouseEvent& event, Uint8 clicks, bool focus) {
            this->mouseReleasedCB(event, clicks, focus);
        });
        
        _mouse->addDragListener(LISTENER_KEY, [this](const MouseEvent& event, const Vec2& previous, bool focus) {
            this->mouseDraggedCB(event, previous, focus);
        });
    }
#else
    if (_touch != nullptr) {
        _touch->addBeginListener(LISTENER_KEY,
                                 [this](const TouchEvent& event, bool focus) { this->touchBeganCB(event, focus); });
        
        _touch->addEndListener(LISTENER_KEY,
                               [this](const TouchEvent& event, bool focus) { this->touchEndedCB(event, focus); });
        
        _touch->addMotionListener(LISTENER_KEY, [this](const TouchEvent& event, const Vec2& previous, bool focus) {
            this->touchMovedCB(event, previous, focus);
        });
    }
#endif
    
    _active = true;
    return true;
}

/**
 * Stops the input processing for this controller.
 *
 * This method deactivates the input devices and detaches the listeners.
 * It does not dispose the controller; the controller can be reused.
 */
void InputController::stop() {
    if (!_active) {
        return;
    }
#ifndef CU_TOUCH_SCREEN
    if (_mouse != nullptr) {
        _mouse->removePressListener(LISTENER_KEY);
        _mouse->removeReleaseListener(LISTENER_KEY);
        _mouse->removeDragListener(LISTENER_KEY);
    }
    Input::deactivate<Mouse>();
    Input::deactivate<Keyboard>();
#else
    if (_touch != nullptr) {
        _touch->removeBeginListener(LISTENER_KEY);
        _touch->removeEndListener(LISTENER_KEY);
        _touch->removeMotionListener(LISTENER_KEY);
    }
    Input::deactivate<Touchscreen>();
#endif
    Input::deactivate<TextInput>();
    
    _active = false;
}

/**
 * Updates the input state for this controller.
 *
 * This method should be called at the start of each animation frame,
 * before any input is processed. Make sure to clear the one-frame state
 * flags with clearInteractionFlags after calling this update and all
 * input processing is complete for the current frame.
 *
 * @param dt    The elapsed time since the last frame
 */
void InputController::update(float dt) {
    if (!_dragging || !_moving) {
        // basically so that when not dragging the delta will be zero,
        // even if the mouse is moving (because for touch this won't matter i
        // think)
        _prevPos = _currPos;
        // proper previous position stored when listener even is activated
    }
    
#ifndef CU_TOUCH_SCREEN
    if (_mouse != nullptr) {
        // Update mouse position
        // updated in all the listeners so I think its okay not being here???
        //_currPos = _mouse->pointerPosition();
        
        // Check for tap-and-hold
        if (_pointerDown && !_dragging && !_tapHoldDetected) {
            Timestamp now;
            now.mark();
            if (now.ellapsedMillis(_pressStartTime) >= DEFAULT_TAP_HOLD_TIME) {
                _tapHoldDetected = true;
            }
        }
        
        // Check for drag starting (if button is down and moved beyond
        // threshold)
        float moveDistance = (_currPos - _startPos).length();
        if (_pointerDown && !_dragging && moveDistance > DEFAULT_DRAG_THRESHOLD) {
            _dragging = true;
            _dragStarted = true;
        }
    }
#endif // !CU_TOUCH_SCREEN
}

/**
 * Clears any buffered inputs so that we may start fresh.
 */
void InputController::clear() {
    // Reset all state values
    _pointerDown = false;
    _tapped = false;
    _doubleTapped = false;
    _dragging = false;
    _dragStarted = false;
    _dragEnded = false;
    _tapHoldDetected = false;
    _swipeDetected = false;
    _moving = false;
    
    // Reset all position values
    _currPos.setZero();
    _prevPos.setZero();
    _startPos.setZero();
    _swipeVelocity.setZero();
    
    // Reset timestamps
    _lastTapTime.mark();
    _pressStartTime.mark();
}

/**
 * Clears all interaction flags at the end of the update cycle.
 *
 * Resets all the one-frame state flags. This method should be called
 * after all input processing is complete for the current frame.
 */
void InputController::clearInteractionFlags() {
    _tapped = false;
    _doubleTapped = false;
    _dragStarted = false;
    _dragEnded = false;
    _tapHoldDetected = false;
    _swipeDetected = false;
    _moving = false;
    _swipeVelocity.setZero();
}

#pragma mark -
#pragma mark Shared Callbacks

/**
 * Shared callback for when a touch/click begins
 *
 * @param pos      The position of the event
 * @param stamp    The timestamp of the event
 */
void InputController::pointerBeganCB(const Vec2& pos, const Timestamp& stamp) {
    // Check for double tap
    if (_lastTapTime.ellapsedMillis(stamp) <= DEFAULT_DOUBLE_TAP_TIME) {
        _doubleTapped = true;
    }
    
    // Record the start of interaction
    _pointerDown = true;
    _pressStartTime = stamp;
    _startPos = pos;
    _prevPos = _currPos;
    _currPos = pos;
}

/**
 * Shared callback for when a touch/click ends
 *
 * @param pos      The position of the event
 * @param stamp    The timestamp of the event
 */
void InputController::pointerEndedCB(const Vec2& pos, const Timestamp& stamp) {
    _pointerDown = false;
    _currPos = pos;
    _prevPos = _currPos;
    // _prevPos and _currPos are the same because we only care about position
    // change when dragging
    
    // Check for single tap
    float moveDistance = (pos - _startPos).length();
    if (!_dragging && !_tapHoldDetected && moveDistance < DEFAULT_DRAG_THRESHOLD &&
        stamp.ellapsedMillis(_pressStartTime) < DEFAULT_TAP_HOLD_TIME) {
        _tapped = true;
        _lastTapTime = stamp;
    }
    
    // Check for swipe
    if (moveDistance >= DEFAULT_SWIPE_MIN_DISTANCE && stamp.ellapsedMillis(_pressStartTime) <= DEFAULT_SWIPE_MAX_TIME) {
        _swipeDetected = true;
        
        // Calculate velocity (pixels per second)
        float elapsedTime = stamp.ellapsedMillis(_pressStartTime) / 1000.0f;
        if (elapsedTime > 0) {
            _swipeVelocity = (pos - _startPos) / elapsedTime;
        }
    }
    
    // Handle drag end if we were dragging
    if (_dragging) {
        _dragging = false;
        _dragEnded = true;
    }
}

/**
 * Shared callback for when a touch/mouse moves
 *
 * @param pos      The position of the event
 * @param previous The previous position
 */
void InputController::pointerMovedCB(const Vec2& pos, const Vec2& previous) {
    _prevPos = _currPos;
    _currPos = pos;
    _moving = true;
    
    // Check for drag starting
    if (_pointerDown && !_dragging) {
        float moveDistance = (pos - _startPos).length();
        if (moveDistance > DEFAULT_DRAG_THRESHOLD) {
            _dragging = true;
            _dragStarted = true;
        }
    }
}

#pragma mark -
#pragma mark Device-Specific Callbacks

/**
 * Callback for the beginning of a touch event
 *
 * @param event    The touch information
 * @param focus    Whether the listener has focus
 */
void InputController::touchBeganCB(const TouchEvent& event, bool focus) {
    pointerBeganCB(event.position, event.timestamp);
}

/**
 * Callback for the end of a touch event
 *
 * @param event    The touch information
 * @param focus    Whether the listener has focus
 */
void InputController::touchEndedCB(const TouchEvent& event, bool focus) {
    pointerEndedCB(event.position, event.timestamp);
}

/**
 * Callback for when a touch moves
 *
 * @param event    The touch information
 * @param previous The previous position
 * @param focus    Whether the listener has focus
 */
void InputController::touchMovedCB(const TouchEvent& event, const Vec2& previous, bool focus) {
    pointerMovedCB(event.position, previous);
}

/**
 * Callback for when a mouse button is pressed
 *
 * @param event    The mouse information
 * @param clicks   The number of clicks
 * @param focus    Whether the listener has focus
 */
void InputController::mousePressedCB(const MouseEvent& event, Uint8 clicks, bool focus) {
    pointerBeganCB(event.position, event.timestamp);
}

/**
 * Callback for when a mouse button is released
 *
 * @param event    The mouse information
 * @param clicks   The number of clicks
 * @param focus    Whether the listener has focus
 */
void InputController::mouseReleasedCB(const MouseEvent& event, Uint8 clicks, bool focus) {
    pointerEndedCB(event.position, event.timestamp);
}

/**
 * Callback for when a mouse is dragged
 *
 * @param event    The mouse information
 * @param previous The previous position
 * @param focus    Whether the listener has focus
 */
void InputController::mouseDraggedCB(const MouseEvent& event, const Vec2& previous, bool focus) {
    pointerMovedCB(event.position, previous);
}
