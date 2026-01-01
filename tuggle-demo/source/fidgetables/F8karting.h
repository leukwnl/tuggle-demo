//
//  F8karting.h
//  Tuggle
//
//  Eighth fidgetable toy - steering wheel with rotation-based haptic ticks.
//

#ifndef __F8KARTING_H__
#define __F8KARTING_H__

#include "FidgetableView.h"

/**
 * Eighth fidgetable toy - Steering Wheel
 *
 * The phone acts as a steering wheel. Rotating the phone triggers
 * haptic ticks at regular angle intervals, like notches on a wheel.
 */
class F8karting : public FidgetableView
{
protected:
  // Visual elements
  /** The steering wheel outer ring */
  std::shared_ptr<cugl::scene2::PolygonNode> _wheelNode;

  /** The center hub */
  std::shared_ptr<cugl::scene2::PolygonNode> _hubNode;

  /** Position indicator (shows current rotation) */
  std::shared_ptr<cugl::scene2::PolygonNode> _indicatorNode;

  /** Tick marks around the wheel */
  std::vector<std::shared_ptr<cugl::scene2::PolygonNode>> _tickMarks;

  // Dimensions
  /** Wheel center position */
  cugl::Vec2 _wheelCenter;

  /** Outer radius of the wheel */
  float _wheelRadius;

  /** Hub radius */
  float _hubRadius;

  // Rotation state
  /** Current steering angle in radians (full 360 range) */
  float _currentAngle;

  /** Last tick index that triggered haptic */
  int _lastTickIndex;

  /** Smoothed angle for stable reading */
  float _smoothedAngle;

  // Constants
  /** Angle between haptic ticks (in radians) - ~10 degrees */
  static constexpr float TICK_ANGLE = 0.175f;

  /** Smoothing factor for accelerometer (0-1, lower = smoother) */
  static constexpr float SMOOTHING = 0.2f;

  /**
   * Creates the visual content for this fidgetable.
   */
  void buildContent() override;

  /**
   * Creates a ring polygon.
   */
  std::shared_ptr<cugl::scene2::PolygonNode>
  createRing(float innerRadius, float outerRadius, const cugl::Color4 &color);

  /**
   * Creates a rectangular bar.
   */
  std::shared_ptr<cugl::scene2::PolygonNode>
  createBar(float width, float height, const cugl::Color4 &color);

  /**
   * Gets the tick index for a given angle.
   */
  int angleToTickIndex(float angle);

public:
  F8karting();
  virtual ~F8karting();

  bool init(int index, const cugl::Size &pageSize) override;
  static std::shared_ptr<F8karting> alloc(const cugl::Size &pageSize);
  void dispose() override;
  void update(float timestep) override;
  void setActive(bool active) override;
  void activateInputs() override;
  void deactivateInputs() override;
};

#endif /* __F8KARTING_H__ */
