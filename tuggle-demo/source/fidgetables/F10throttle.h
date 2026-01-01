//
//  F10throttle.h
//  Tuggle
//
//  Tenth fidgetable toy - realistic car simulator with button-based shifting.
//  Demonstrates all HapticPlayer features with realistic gear/RPM mechanics.
//

#ifndef __F10THROTTLE_H__
#define __F10THROTTLE_H__

#include "FidgetableView.h"
#include <cugl/core/input/CUHaptics.h>

/**
 * Gear levels (1-5 plus neutral and reverse).
 */
enum class Gear {
  NEUTRAL = 0,
  GEAR_1,
  GEAR_2,
  GEAR_3,
  GEAR_4,
  GEAR_5,
  REVERSE
};

/**
 * Tenth fidgetable toy - realistic car simulator.
 *
 * Features:
 * - Button-based gear shifting (up/down buttons)
 * - RPM gauge with needle (0-100% of current gear's max)
 * - Realistic gear mechanics with shift windows
 * - Pedal-style throttle button
 * - Continuous engine haptics via HapticPlayer
 */
class F10throttle : public FidgetableView {
protected:
  // === Visual Elements ===

  /** The RPM needle */
  std::shared_ptr<cugl::scene2::PolygonNode> _needle;

  /** Tick marks around the gauge */
  std::vector<std::shared_ptr<cugl::scene2::PolygonNode>> _gaugeTicks;

  /** Gear indicator (colored bar showing current gear) */
  std::shared_ptr<cugl::scene2::PolygonNode> _gearIndicator;

  /** Gear level indicators (5 bars) */
  std::array<std::shared_ptr<cugl::scene2::PolygonNode>, 5> _gearBars;

  /** Shift up button */
  std::shared_ptr<cugl::scene2::Button> _shiftUpButton;

  /** Shift down button */
  std::shared_ptr<cugl::scene2::Button> _shiftDownButton;

  /** The throttle pedal button */
  std::shared_ptr<cugl::scene2::Button> _throttleButton;
  std::shared_ptr<cugl::scene2::PolygonNode> _throttleNode;

  // === State ===

  /** Current gear */
  Gear _currentGear;

  /** Current RPM (0 to max for current gear) */
  float _currentRPM;

  /** Whether throttle is currently pressed */
  bool _isThrottling;

  /** Whether the engine is running */
  bool _engineRunning;

  /** Whether the engine has stalled */
  bool _isStalled;

  /** The HapticPlayer for continuous engine rumble */
  cugl::HapticPlayer _enginePlayer;

  // === Constants ===

  /** Idle RPM */
  static constexpr float IDLE_RPM = 500.0f;

  /** RPM acceleration rate when throttling (RPM/second) */
  static constexpr float RPM_ACCEL_RATE = 2500.0f;

  /** RPM deceleration rate when not throttling */
  static constexpr float RPM_DECEL_RATE = 2000.0f;

  /** Shift window percentage (must be within this % of max to shift up) */
  static constexpr float SHIFT_WINDOW_PERCENT = 0.15f;

  /** Gauge center Y ratio */
  static constexpr float GAUGE_CENTER_Y_RATIO = 0.65f;

  /** Gauge radius ratio */
  static constexpr float GAUGE_RADIUS_RATIO = 0.5f;

  // === Helper Methods ===

  void buildContent() override;
  void buildGauge();
  void buildGearControls();
  void buildThrottlePedal();

  /** Gets the max RPM for a given gear */
  float getMaxRPMForGear(Gear gear);

  /** Gets the minimum RPM required to shift UP to next gear */
  float getShiftWindowMinRPM();

  /** Checks if current RPM is in the shift window */
  bool isInShiftWindow();

  /** Attempts to shift up */
  void shiftUp();

  /** Attempts to shift down */
  void shiftDown();

  /** Causes the engine to stall */
  void stallEngine();

  /** Updates the RPM gauge visuals */
  void updateGaugeVisuals();

  /** Updates gear indicator visuals */
  void updateGearIndicator();

  /** Updates haptic feedback based on current state */
  void updateHaptics();

  std::shared_ptr<cugl::scene2::PolygonNode> createCircle(float radius,
                                                          cugl::Color4 color);
  std::shared_ptr<cugl::scene2::PolygonNode>
  createRect(float width, float height, cugl::Color4 color);

public:
  F10throttle();
  virtual ~F10throttle();

  bool init(int index, const cugl::Size &pageSize) override;
  static std::shared_ptr<F10throttle> alloc(const cugl::Size &pageSize);
  void dispose() override;
  void update(float timestep) override;
  void setActive(bool active) override;
  void activateInputs() override;
  void deactivateInputs() override;

  /** Starts the engine */
  void startEngine();

  /** Stops the engine */
  void stopEngine();
};

#endif /* __F10THROTTLE_H__ */
