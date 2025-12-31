//
//  HapticButton.h
//  Tuggle
//
//  Reusable draggable button with haptic feedback.
//  Supports different haptic styles for variety.
//

#ifndef __HAPTIC_BUTTON_H__
#define __HAPTIC_BUTTON_H__

#include <cugl/cugl.h>
#include <cugl/core/input/CUHaptics.h>

/**
 * Haptic style determines how the button feels when interacted with.
 */
enum class HapticStyle {
  /** Velocity-based transient haptics (like F1tancho) */
  RATTLE,
  /** Soft continuous buzz while dragging */
  BUZZ,
  /** Sharp ticks at regular intervals */
  TICK,
  /** Heavy thumps based on distance from center */
  THUMP
};

/**
 * A reusable draggable button with spring physics and haptic feedback.
 * Can be configured with different haptic styles for variety.
 */
class HapticButton {
public:
  // Configuration
  struct Config {
    float radius;
    cugl::Color4 normalColor;
    cugl::Color4 pressedColor;
    cugl::Color4 inactiveColor;
    HapticStyle hapticStyle;
    float maxDragDistance;
    float springK;
    float pressedScale;
    float animDuration;
    float sceneHeight;  // For screen-to-scene scale calculation
    
    Config() :
      radius(50.0f),
      normalColor(100, 149, 237, 255),
      pressedColor(70, 119, 207, 255),
      inactiveColor(150, 180, 220, 255),
      hapticStyle(HapticStyle::RATTLE),
      maxDragDistance(12.0f),
      springK(0.4f),
      pressedScale(0.85f),
      animDuration(0.12f),
      sceneHeight(1024.0f) {}
  };

private:
  std::shared_ptr<cugl::scene2::Button> _button;
  std::shared_ptr<cugl::scene2::PolygonNode> _buttonNode;
  
  Config _config;
  cugl::Vec2 _originalPos;
  cugl::Vec2 _dragOffset;
  cugl::Vec2 _dragVelocity;
  cugl::Vec2 _lastPointerPos;
  
  bool _isPressed = false;
  bool _isDragging = false;
  bool _isActive = true;
  float _currentScale = 1.0f;
  float _hapticCooldown = 0.0f;
  
  // Haptic parameters (velocity in scene units/second)
  static constexpr float MAX_HAPTIC_VELOCITY = 1000.0f;
  static constexpr float MIN_HAPTIC_INTENSITY = 0.05f;

  std::shared_ptr<cugl::scene2::PolygonNode> createCircle(float radius, cugl::Color4 color);
  void triggerHaptic(float velocity, float timestep);

public:
  HapticButton() = default;
  ~HapticButton() = default;

  /**
   * Initializes the button at the given position.
   */
  bool init(cugl::Vec2 position, const Config& config = Config());

  /**
   * Returns the underlying CUGL button node.
   */
  std::shared_ptr<cugl::scene2::Button> getNode() { return _button; }

  /**
   * Called when this button is pressed.
   */
  void onPressed(cugl::Vec2 pointerPos);

  /**
   * Called when this button is released.
   */
  void onReleased();

  /**
   * Updates the button state and animations.
   */
  void update(float timestep, cugl::Vec2 currentPointerPos);

  /**
   * Returns whether this button is currently being interacted with.
   */
  bool isInteracting() const { return _isDragging; }

  /**
   * Sets whether this button is active.
   */
  void setActive(bool active);

  /**
   * Activates input for this button.
   */
  void activate();

  /**
   * Deactivates input for this button.
   */
  void deactivate();
};

#endif /* __HAPTIC_BUTTON_H__ */

