//
//  F4traffic.h
//  Tuggle
//
//  Fourth fidgetable toy - three hold-to-pop buttons.
//

#ifndef __F4TRAFFIC_H__
#define __F4TRAFFIC_H__

#include "FidgetableView.h"
#include <cugl/core/input/CUHaptics.h>
#include <array>

#define NUM_TRAFFIC_BUTTONS 3

/** Button state after popping */
enum class PopState {
  NONE,       // Normal, interactive
  FADING_OUT, // Just popped, fading out
  WAITING,    // Invisible, waiting to respawn
  FADING_IN   // Respawning, fading in
};

/**
 * Fourth fidgetable toy in the carousel.
 * Three buttons in a column that grow when held and pop after 3 seconds.
 * Each has different haptic sharpness, with accelerating vibrations.
 */
class F4traffic : public FidgetableView {
protected:
  std::array<std::shared_ptr<cugl::scene2::Button>, NUM_TRAFFIC_BUTTONS> _buttons;
  std::array<std::shared_ptr<cugl::scene2::PolygonNode>, NUM_TRAFFIC_BUTTONS> _normalNodes;
  std::array<std::shared_ptr<cugl::scene2::PolygonNode>, NUM_TRAFFIC_BUTTONS> _pressedNodes;
  
  /** Which button is currently being held (-1 if none) */
  int _heldButton;
  
  /** How long the button has been held */
  float _holdTime;
  
  /** Current scale of the held button */
  float _currentScale;
  
  /** Current alpha for fading buttons */
  std::array<float, NUM_TRAFFIC_BUTTONS> _alphas;
  
  /** Pop state for each button */
  std::array<PopState, NUM_TRAFFIC_BUTTONS> _popStates;
  
  /** Timer for respawn delay */
  std::array<float, NUM_TRAFFIC_BUTTONS> _stateTimers;
  
  /** Haptic timer for accelerating pulses */
  float _hapticTimer;
  
  float _buttonRadius;
  
  // Timing constants
  static constexpr float POP_TIME = 2.0f;
  static constexpr float MAX_SCALE = 1.25f;
  static constexpr float FADE_OUT_DURATION = 0.2f;
  static constexpr float RESPAWN_DELAY = 0.5f;
  static constexpr float FADE_IN_DURATION = 0.3f;
  
  // Haptic timing (per-button values defined in .cpp)

  void buildContent() override;
  void onPressed(int index);
  void onReleased(int index);
  void popButton(int index);
  void applyButtonAlpha(int index);

public:
  F4traffic();
  virtual ~F4traffic();

  bool init(int index, const cugl::Size& pageSize) override;
  static std::shared_ptr<F4traffic> alloc(const cugl::Size& pageSize);
  void dispose() override;
  void update(float timestep) override;
  void setActive(bool active) override;
  void activateInputs() override;
  void deactivateInputs() override;
};

#endif /* __F4TRAFFIC_H__ */
