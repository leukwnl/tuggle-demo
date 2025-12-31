//
//  F2sampler.h
//  Tuggle
//
//  Second fidgetable toy - a 2x2 grid of buttons with different haptic styles.
//

#ifndef __F2sampler_H__
#define __F2sampler_H__

#include "FidgetableView.h"
#include "HapticButton.h"
#include <array>

/**
 * Second fidgetable toy in the carousel.
 * Displays 4 buttons in a 2x2 grid, each with a different haptic style:
 * - Top-left: RATTLE (velocity-based transients)
 * - Top-right: BUZZ (soft continuous feel)
 * - Bottom-left: TICK (sharp distinct ticks)
 * - Bottom-right: THUMP (heavy distance-based thumps)
 */
class F2sampler : public FidgetableView {
protected:
  /** The four haptic buttons */
  std::array<HapticButton, 4> _buttons;

  /** Track which buttons are currently active (for multi-touch) */
  std::array<bool, 4> _buttonActive = {false, false, false, false};

  /** Button radius ratio relative to page width */
  static constexpr float BUTTON_RADIUS_RATIO = 0.12f;

  /** Spacing between buttons */
  static constexpr float BUTTON_SPACING_RATIO = 0.08f;

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
