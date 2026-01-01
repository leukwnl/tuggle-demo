//
//  AudioController.h
//  Tuggle
//
//  Minimal audio controller for soundboard functionality.
//  Singleton pattern for easy access from fidgetables.
//

#ifndef __AUDIO_CONTROLLER_H__
#define __AUDIO_CONTROLLER_H__

#include <cugl/cugl.h>
#include <map>
#include <string>

/**
 * Minimal audio controller for the Tuggle demo.
 *
 * Provides simple sound effect playback through a singleton interface.
 * Sounds are registered by key and can be played/stopped on demand.
 */
class AudioController {
private:
  /** The singleton instance */
  static AudioController *_instance;

  /** Sound effect volume (0.0 - 1.0) */
  float _sfxVolume;

  /** Dictionary of loaded sounds */
  std::map<std::string, std::shared_ptr<cugl::audio::Sound>> _sounds;

  /** Private constructor for singleton */
  AudioController();

public:
  /**
   * Returns the singleton instance of the AudioController.
   * Creates it if it doesn't exist.
   *
   * @return The singleton AudioController
   */
  static AudioController *get();

  /**
   * Initializes the audio controller.
   * Must be called after AudioEngine::start().
   */
  static void start();

  /**
   * Shuts down the audio controller.
   * Call before AudioEngine::stop().
   */
  static void stop();

  /**
   * Registers a sound with the given key.
   *
   * @param key   The key to identify this sound
   * @param sound The sound asset to register
   */
  void registerSound(const std::string &key,
                     const std::shared_ptr<cugl::audio::Sound> &sound);

  /**
   * Plays the sound registered with the given key.
   *
   * @param key   The key of the sound to play
   * @param loop  Whether to loop the sound (default false)
   * @param force Whether to force playback even if no slots (default true)
   * @return true if the sound was played
   */
  bool play(const std::string &key, bool loop = false, bool force = true);

  /**
   * Stops a currently playing sound (with fade).
   *
   * @param key The key of the sound to stop
   */
  void stopSound(const std::string &key);

  /**
   * Clears (immediately stops) a currently playing sound.
   *
   * @param key The key of the sound to clear
   */
  void clear(const std::string &key);

  /**
   * Checks if a sound is currently playing.
   *
   * @param key The key of the sound to check
   * @return true if the sound is playing
   */
  bool isPlaying(const std::string &key);

  /**
   * Sets the sound effect volume.
   *
   * @param volume The volume (0.0 - 1.0)
   */
  void setVolume(float volume);

  /**
   * Gets the current sound effect volume.
   *
   * @return The volume (0.0 - 1.0)
   */
  float getVolume() const { return _sfxVolume; }
};

#endif /* __AUDIO_CONTROLLER_H__ */

