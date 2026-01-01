//
//  F9soundboard.h
//  Tuggle
//
//  Ninth fidgetable toy - a 3x3 grid of soundboard buttons.
//  Each button plays synchronized audio + AHAP haptics.
//

#ifndef __F9SOUNDBOARD_H__
#define __F9SOUNDBOARD_H__

#include "FidgetableView.h"
#include <cugl/audio/CUAudioEngine.h>
#include <cugl/core/input/CUHaptics.h>
#include <array>

#define NUM_BUTTONS 9


/**
 * Ninth fidgetable toy in the carousel.
 * Displays 9 buttons in a 3x3 grid, each playing a different sound + haptic combo.
 *
 * Features:
 * - Each button plays an AHAP haptic pattern synchronized with audio
 * - Pressing any button stops the previous sound and starts the new one
 * - Visual feedback shows which button is currently playing
 * - Uses Haptics::play() for AHAP file playback
 */
class F9soundboard : public FidgetableView {
protected:
  /** The button scene nodes */
  std::array<std::shared_ptr<cugl::scene2::Button>, NUM_BUTTONS> _buttons;

  /** The button visual circles */
  std::array<std::shared_ptr<cugl::scene2::PolygonNode>, NUM_BUTTONS> _buttonNodes;

  /** Track which button is currently playing (-1 if none) */
  int _activeButton;

  /** Current playback time (for visual feedback animation) */
  float _playbackTime;

  /** Whether playback is active */
  bool _isPlaybackActive;

  /** Sound assets for each button */
  std::array<std::shared_ptr<cugl::audio::Sound>, NUM_BUTTONS> _sounds;

  /** AHAP filenames for each button */
  std::array<std::string, NUM_BUTTONS> _ahapFiles;

  /** Audio key used for current playback */
  std::string _currentAudioKey;

  /** Button radius ratio relative to page width */
  static constexpr float BUTTON_RADIUS_RATIO = 0.10f;

  /** Spacing between buttons */
  static constexpr float BUTTON_SPACING_RATIO = 0.04f;

  /** Colors for each button (vibrant soundboard aesthetic) */
  static const cugl::Color4 BUTTON_COLORS[NUM_BUTTONS];
  static const cugl::Color4 BUTTON_COLORS_PRESSED[NUM_BUTTONS];
  static const cugl::Color4 BUTTON_COLORS_PLAYING[NUM_BUTTONS];
  static const cugl::Color4 BUTTON_COLORS_INACTIVE[NUM_BUTTONS];

  /**
   * Creates the visual content for this fidgetable.
   */
  void buildContent() override;

  /**
   * Creates a filled circle polygon node.
   */
  std::shared_ptr<cugl::scene2::PolygonNode>
  createCircle(float radius, cugl::Color4 color);

  /**
   * Triggers playback of sound and haptics for the given button index.
   * Stops any currently playing sound first.
   */
  void triggerButton(int index);

  /**
   * Stops current playback.
   */
  void stopPlayback();

  /**
   * Updates visual feedback for the playing button.
   */
  void updatePlaybackVisuals(float timestep);

public:
  F9soundboard();
  virtual ~F9soundboard();

  bool init(int index, const cugl::Size &pageSize) override;
  static std::shared_ptr<F9soundboard> alloc(const cugl::Size &pageSize);
  void dispose() override;
  void update(float timestep) override;
  void setActive(bool active) override;
  void activateInputs() override;
  void deactivateInputs() override;

  /**
   * Sets the sound asset for a specific button.
   *
   * @param index The button index (0-8)
   * @param sound The sound asset to play
   */
  void setSound(int index, const std::shared_ptr<cugl::audio::Sound> &sound);

  /**
   * Sets the AHAP haptic file for a specific button.
   *
   * @param index    The button index (0-8)
   * @param filename The AHAP file path (relative to bundle)
   */
  void setHapticFile(int index, const std::string &filename);
};

#endif /* __F9SOUNDBOARD_H__ */

