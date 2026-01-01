//
//  F2sampler.h
//  Tuggle
//
//  Second fidgetable toy - a grid of buttons demonstrating all haptic styles.
//

#ifndef __F2sampler_H__
#define __F2sampler_H__

#include "FidgetableView.h"
#include "HapticButton.h"
#include <array>

/**
 * Second fidgetable toy in the carousel.
 * Displays 7 buttons showcasing all available haptic styles:
 * - Row 1: RATTLE, BUZZ, TICK
 * - Row 2: THUMP, TAP
 * - Row 3: BUZZ_PULSE, SELECTION
 *
 * Each button demonstrates a different haptic method from the Haptics class.
 */
class F2sampler : public FidgetableView {
protected:
  /** Number of haptic style buttons */
  static constexpr int NUM_BUTTONS = 7;

  /** The haptic buttons - one for each style */
  std::array<HapticButton, NUM_BUTTONS> _buttons;

  /** Track which buttons are currently active (for multi-touch) */
  std::array<bool, NUM_BUTTONS> _buttonActive = {};

  /** Button radius ratio relative to page width */
  static constexpr float BUTTON_RADIUS_RATIO = 0.09f;

  /** Spacing between buttons */
  static constexpr float BUTTON_SPACING_RATIO = 0.06f;

  void buildContent() override;

public:
  F2sampler();
  virtual ~F2sampler();

  bool init(int index, const cugl::Size &pageSize) override;
  static std::shared_ptr<F2sampler> alloc(const cugl::Size &pageSize);
  void dispose() override;
  void update(float timestep) override;
  void setActive(bool active) override;
  void activateInputs() override;
  void deactivateInputs() override;
};

#endif /* __F2sampler_H__ */
