//
//  InputController.h
//  Game
//
//  This input controller manages all input for the game including keyboard,
//  mouse, touch, and gestures. It provides a unified interface for handling
//  different input methods across platforms.
//
//  This class is a singleton and should only be accessed via getInstance().
//

#ifndef __INPUT_CONTROLLER_H__
#define __INPUT_CONTROLLER_H__

#include <cugl/cugl.h>

/**
 * This class represents the universal input controller for the game.
 *
 * This input handler uses the CUGL input API and manages various input
 * devices including keyboard, mouse, touch, and gestures. It uses both
 * polling and callback approaches depending on the device.
 *
 * This controller is implemented as a singleton. To access the input
 * controller, use InputController::getInstance().
 */
class InputController {
   private:
    /** The singleton instance of this class */
    static InputController* _instance;

    /** Whether or not this input controller is active */
    bool _active;

    // Input Device Singletons
    /** Mouse input device */
    cugl::Mouse* _mouse;
    /** Keyboard input device */
    cugl::Keyboard* _keyboard;
    /** Touch screen device */
    cugl::Touchscreen* _touch;

    // Mouse/Touch state
    /** Current position */
    cugl::Vec2 _currPos;
    /** Previous position */
    cugl::Vec2 _prevPos;
    /** Start position of current touch/click */
    cugl::Vec2 _startPos;
    /** Whether the button/touch is currently down */
    bool _pointerDown;

    // Interaction state
    /** Whether a tap was detected this frame */
    bool _tapped;
    /** Whether a double tap was detected this frame */
    bool _doubleTapped;
    /** Whether currently moving for this frame */
    bool _moving;
    /** Whether currently dragging */
    bool _dragging;
    /** Whether drag just started this frame */
    bool _dragStarted;
    /** Whether drag just ended this frame */
    bool _dragEnded;
    /** Whether a tap-and-hold was detected */
    bool _tapHoldDetected;
    /** Whether a swipe was detected this frame */
    bool _swipeDetected;
    /** Velocity vector of the swipe gesture */
    cugl::Vec2 _swipeVelocity;

    // Timestamps and thresholds
    /** Timestamp of the last tap event */
    cugl::Timestamp _lastTapTime;
    /** Start time of the current press */
    cugl::Timestamp _pressStartTime;
    /** Maximum time between taps to register as double tap (in milliseconds) */
    Uint32 _doubleTapTime;
    /** Minimum time required for a tap-and-hold (in milliseconds) */
    Uint32 _tapHoldTime;
    /** Minimum distance required for a swipe gesture */
    float _swipeMinDistance;
    /** Maximum time for a swipe gesture to be recognized (in milliseconds) */
    Uint32 _swipeMaxTime;
    /** Movement threshold to differentiate tap from drag (in pixels) */
    float _dragThreshold;

    /** Key for event listeners */
    static constexpr Uint32 LISTENER_KEY = 1;

   private:
#pragma mark -
#pragma mark Constructors/Destructors
    /**
     * Creates a new input controller.
     *
     * This constructor is private to prevent unwanted instantiation.
     */
    InputController();

    /**
     * Disposes of this input controller, releasing all listeners.
     */
    ~InputController() { stop(); }

   public:
    /**
     * Returns the singleton instance of this class.
     *
     * @return the singleton instance of this class.
     */
    static InputController* getInstance() {
        if (_instance == nullptr) {
            _instance = new InputController();
        }
        return _instance;
    }

    /**
     * Releases the singleton instance of this class.
     */
    static void release() {
        if (_instance != nullptr) {
            delete _instance;
            _instance = nullptr;
        }
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
    bool init();

    /**
     * Starts the input processing for this controller.
     *
     * This method activates the input devices and attaches the listeners.
     *
     * @return true if the controller was successfully started
     */
    bool start();

    /**
     * Stops the input processing for this controller.
     *
     * This method deactivates the input devices and detaches the listeners.
     * It does not dispose the controller; the controller can be reused.
     */
    void stop();

#pragma mark -
#pragma mark Update Methods
    /**
     * Updates the input state for this controller.
     *
     * This method should be called at the start of each animation frame,
     * before any input is processed.
     *
     * @param dt    The elapsed time since the last frame
     */
    void update(float dt);

    /**
     * Clears any buffered inputs so that we may start fresh.
     */
    void clear();

    /**
     * Clears all interaction flags at the end of the update cycle.
     *
     * This method should be called after all input processing is complete
     * for the current frame.
     */
    void clearInteractionFlags();

#pragma mark -
#pragma mark Interaction Methods
    /**
     * Returns the current pointer position (mouse or touch).
     *
     * @return the current pointer position
     */
    const cugl::Vec2 getPosition() const { return _currPos; }

    /**
     * Returns the change in pointer position since the last animation frame.
     *
     * Change will be zero when the input is not dragging
     *
     * @return the change in pointer position
     */
    cugl::Vec2 getDelta() const { return _currPos - _prevPos; }

    /**
     * Returns true if the pointer button/touch is currently down.
     *
     * @return true if the pointer button/touch is currently down
     */
    bool isDown() const { return _pointerDown; }

    /**
     * Returns true if a single tap was detected this animation frame.
     *
     * @return true if a single tap was detected
     */
    bool didTap() const { return _tapped; }

    /**
     * Returns true if a double tap was detected this animation frame.
     *
     * @return true if a double tap was detected
     */
    bool didDoubleTap() const { return _doubleTapped; }

    /**
     * Returns true if a tap-and-hold was detected this animation frame.
     *
     * @return true if a tap-and-hold was detected
     */
    bool didTapHold() const { return _tapHoldDetected; }

    /**
     * Returns true if the user is currently dragging.
     *
     * @return true if the user is dragging
     */
    bool isDragging() const { return _dragging; }

    /**
     * Returns true if a drag operation started this animation frame.
     *
     * @return true if a drag operation just started
     */
    bool didDragStart() const { return _dragStarted; }

    /**
     * Returns true if a drag operation ended this animation frame.
     *
     * @return true if a drag operation just ended
     */
    bool didDragEnd() const { return _dragEnded; }

    /**
     * Returns true if a swipe gesture was detected this animation frame.
     *
     * @return true if a swipe was detected
     */
    bool didSwipe() const { return _swipeDetected; }

    /**
     * Returns the starting position of the current or most recent interaction.
     *
     * @return the starting position of the interaction
     */
    const cugl::Vec2& getStartPosition() const { return _startPos; }

    /**
     * Returns the velocity vector of the most recent swipe gesture.
     *
     * @return the velocity vector of the swipe
     */
    const cugl::Vec2& getSwipeVelocity() const { return _swipeVelocity; }

   private:
#pragma mark -
#pragma mark Callbacks
    /**
     * Shared callback for when a touch/click begins
     *
     * @param pos      The position of the event
     * @param stamp    The timestamp of the event
     */
    void pointerBeganCB(const cugl::Vec2& pos, const cugl::Timestamp& stamp);

    /**
     * Shared callback for when a touch/click ends
     *
     * @param pos      The position of the event
     * @param stamp    The timestamp of the event
     */
    void pointerEndedCB(const cugl::Vec2& pos, const cugl::Timestamp& stamp);

    /**
     * Shared callback for when a touch/mouse moves
     *
     * @param pos      The position of the event
     * @param previous The previous position
     */
    void pointerMovedCB(const cugl::Vec2& pos, const cugl::Vec2& previous);

    /**
     * Callback for the beginning of a touch event
     *
     * @param event    The touch information
     * @param focus    Whether the listener has focus
     */
    void touchBeganCB(const cugl::TouchEvent& event, bool focus);

    /**
     * Callback for the end of a touch event
     *
     * @param event    The touch information
     * @param focus    Whether the listener has focus
     */
    void touchEndedCB(const cugl::TouchEvent& event, bool focus);

    /**
     * Callback for when a touch moves
     *
     * @param event    The touch information
     * @param previous The previous position
     * @param focus    Whether the listener has focus
     */
    void touchMovedCB(const cugl::TouchEvent& event, const cugl::Vec2& previous, bool focus);

    /**
     * Callback for when a mouse button is pressed
     *
     * @param event    The mouse information
     * @param clicks   The number of clicks
     * @param focus    Whether the listener has focus
     */
    void mousePressedCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus);

    /**
     * Callback for when a mouse button is released
     *
     * @param event    The mouse information
     * @param clicks   The number of clicks
     * @param focus    Whether the listener has focus
     */
    void mouseReleasedCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus);

    /**
     * Callback for when a mouse is dragged
     *
     * @param event    The mouse information
     * @param previous The previous position
     * @param focus    Whether the listener has focus
     */
    void mouseDraggedCB(const cugl::MouseEvent& event, const cugl::Vec2& previous, bool focus);
};

#endif /* __INPUT_CONTROLLER_H__ */
