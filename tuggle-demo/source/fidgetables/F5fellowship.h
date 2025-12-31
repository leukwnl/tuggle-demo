//
//  F5fellowship.h
//  Tuggle
//
//  Fifth fidgetable toy - a circular slider with haptic ticks.
//

#ifndef __F5FELLOWSHIP_H__
#define __F5FELLOWSHIP_H__

#include "FidgetableView.h"
#include <cugl/core/input/CUHaptics.h>

/**
 * Fifth fidgetable toy in the carousel.
 * A ring-shaped slider with a draggable knob that plays haptic ticks.
 */
class F5fellowship : public FidgetableView {
protected:
  /** The ring track visual */
  std::shared_ptr<cugl::scene2::PolygonNode> _trackNode;
  
  /** The draggable knob button */
  std::shared_ptr<cugl::scene2::Button> _knobButton;
  
  /** The knob's visual node */
  std::shared_ptr<cugl::scene2::PolygonNode> _knobNode;
  
  /** Ring radius (center of track) */
  float _ringRadius;
  
  /** Track thickness */
  float _trackThickness;
  
  /** Knob radius */
  float _knobRadius;
  
  /** Current angle of knob in radians */
  float _currentAngle;
  
  /** Last tick index for haptic detection */
  int _lastTickIndex;
  
  /** Whether the knob is being dragged */
  bool _isDragging;
  
  /** Center position of the ring */
  cugl::Vec2 _center;
  
  /** Number of ticks around the circle */
  static constexpr int NUM_TICKS = 24;
  
  /** Radians per tick */
  static constexpr float RADIANS_PER_TICK = (2.0f * M_PI) / NUM_TICKS;

  void buildContent() override;
  void updateKnobPosition();
  int angleToTickIndex(float angle);
  std::shared_ptr<cugl::scene2::PolygonNode> createRing(float innerRadius, float outerRadius, 
                                                         cugl::Color4 color, int segments = 64);

public:
  F5fellowship();
  virtual ~F5fellowship();

  bool init(int index, const cugl::Size& pageSize) override;
  static std::shared_ptr<F5fellowship> alloc(const cugl::Size& pageSize);
  void dispose() override;
  void update(float timestep) override;
  void setActive(bool active) override;
  void activateInputs() override;
  void deactivateInputs() override;
};

#endif /* __F5FELLOWSHIP_H__ */
