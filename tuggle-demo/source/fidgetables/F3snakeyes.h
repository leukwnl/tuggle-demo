//
//  F3snakeyes.h
//  Tuggle
//
//  Third fidgetable toy - six toggle buttons with continuous haptics.
//

#ifndef __F3SNAKEYES_H__
#define __F3SNAKEYES_H__

#include "FidgetableView.h"
#include <cugl/core/input/CUHaptics.h>
#include <array>

/** Number of buttons in the grid */
#define NUM_BUTTONS 6

/**
 * Third fidgetable toy in the carousel.
 * Displays 6 toggle buttons in a 3 row x 2 column grid.
 * Left column: Rumbles (low sharpness) at slow/medium/fast speeds
 * Right column: Ticks (high sharpness) at slow/medium/fast speeds
 * All at heavy intensity. Each button toggles on/off.
 */
class F3snakeyes : public FidgetableView {
protected:
  /** The interactive buttons */
  std::array<std::shared_ptr<cugl::scene2::Button>, NUM_BUTTONS> _buttons;
  
  /** The buttons' visual nodes */
  std::array<std::shared_ptr<cugl::scene2::PolygonNode>, NUM_BUTTONS> _buttonNodes;
  
  /** Toggle state for each button */
  std::array<bool, NUM_BUTTONS> _toggleStates;
  
  /** Current scale for each button (for animation) */
  std::array<float, NUM_BUTTONS> _currentScales;
  
  /** Per-button haptic timers */
  std::array<float, NUM_BUTTONS> _hapticTimers;
  
  /** Button radius */
  float _buttonRadius;
  
  /** Animation duration */
  static constexpr float ANIM_DURATION = 0.15f;
  
  /** Scale when toggled on */
  static constexpr float TOGGLED_SCALE = 1.2f;

  void buildContent() override;
  void onToggle(int index);

public:
  F3snakeyes();
  virtual ~F3snakeyes();

  bool init(int index, const cugl::Size& pageSize) override;
  static std::shared_ptr<F3snakeyes> alloc(const cugl::Size& pageSize);
  void dispose() override;
  void update(float timestep) override;
  void setActive(bool active) override;
  void activateInputs() override;
  void deactivateInputs() override;
};

#endif /* __F3SNAKEYES_H__ */
