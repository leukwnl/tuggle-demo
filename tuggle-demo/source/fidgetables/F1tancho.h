//
//  F1tancho.h
//  Tuggle
//
// A button that can slide in any direction.
//

#ifndef __F1TANCHO_H__
#define __F1TANCHO_H__

#include "FidgetableView.h"
#include <cugl/core/input/CUHaptics.h>

class F1tancho : public FidgetableView
{
protected:
  /** The interactive button */
  std::shared_ptr<cugl::scene2::Button> _button;

  /** The button's visual node */
  std::shared_ptr<cugl::scene2::PolygonNode> _buttonNode;

  /** Button radius */
  float _buttonRadius;

  /** Whether the button is currently pressed */
  bool _isPressed;

  /** Current button scale (for animation) */
  float _currentScale;

  // Drag interaction state
  /** Whether the button is currently being dragged */
  bool _isDragging;

  /** Original center position of the button */
  cugl::Vec2 _originalPos;

  /** Current offset from original position */
  cugl::Vec2 _dragOffset;

  /** Drag velocity for haptic feedback */
  cugl::Vec2 _dragVelocity;

  /** Last pointer position for velocity calculation */
  cugl::Vec2 _lastPointerPos;

  /** Pointer position when drag started */
  cugl::Vec2 _dragStartPointerPos;

  /** Time since last haptic trigger */
  float _hapticCooldown;

  /** Animation duration in seconds */
  static constexpr float ANIM_DURATION = 0.12f;

  /** Scale when pressed (shrink amount) */
  static constexpr float PRESSED_SCALE = 0.85f;

  /** Maximum drag distance from center (pixels) */
  static constexpr float MAX_DRAG_DISTANCE = 12.0f;

  /** Spring constant (higher = stiffer, more resistance) */
  static constexpr float SPRING_K = 0.4f;

  /** Maximum velocity for haptic intensity scaling (scene units/second) */
  static constexpr float MAX_HAPTIC_VELOCITY = 1000.0f;

  /** Minimum haptic intensity to bother playing  */
  static constexpr float MIN_HAPTIC_INTENSITY = 0.05f;

  /**
   * Creates the visual content for this fidgetable.
   */
  void buildContent() override;

  /**
   * Called when the button is pressed down.
   */
  virtual void onPressed();

  /**
   * Called when the button is released.
   */
  virtual void onReleased();

  /**
   * Updates the drag interaction with spring physics.
   * @param timestep The time since last update
   */
  void updateDrag(float timestep);

  /**
   * Triggers rattling haptic feedback based on velocity.
   * @param velocity The current drag velocity magnitude
   * @param timestep The time since last update
   */
  void triggerRattleHaptic(float velocity, float timestep);

public:
  F1tancho();
  virtual ~F1tancho();

  /**
   * Initializes this fidgetable.
   */
  bool init(int index, const cugl::Size &pageSize) override;

  /**
   * Static allocator for F1tancho.
   */
  static std::shared_ptr<F1tancho> alloc(const cugl::Size &pageSize);

  /**
   * Disposes of all resources.
   */
  void dispose() override;

  /**
   * Updates this fidgetable.
   */
  void update(float timestep) override;

  /**
   * Sets whether this fidgetable is active.
   */
  void setActive(bool active) override;

  /**
   * Activates input listeners.
   */
  void activateInputs() override;

  /**
   * Deactivates input listeners.
   */
  void deactivateInputs() override;
};

#endif /* __F1TANCHO_H__ */
