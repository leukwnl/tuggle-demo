//
//  AudioController.cpp
//  Tuggle
//
//  Implementation of minimal audio controller for soundboard functionality.
//

#include "AudioController.h"

using namespace cugl;
using namespace cugl::audio;

// Initialize static instance
AudioController *AudioController::_instance = nullptr;

AudioController::AudioController() : _sfxVolume(1.0f) {}

AudioController *AudioController::get() {
  if (_instance == nullptr) {
    _instance = new AudioController();
  }
  return _instance;
}

void AudioController::start() {
  // Ensure instance exists
  get();
}

void AudioController::stop() {
  if (_instance != nullptr) {
    // Clear all sounds
    _instance->_sounds.clear();
    delete _instance;
    _instance = nullptr;
  }
}

void AudioController::registerSound(
    const std::string &key, const std::shared_ptr<cugl::audio::Sound> &sound) {
  if (sound != nullptr) {
    _sounds[key] = sound;
  }
}

bool AudioController::play(const std::string &key, bool loop, bool force) {
  auto it = _sounds.find(key);
  if (it == _sounds.end() || it->second == nullptr) {
    return false;
  }

  auto *engine = AudioEngine::get();
  if (engine == nullptr) {
    return false;
  }

  // Stop any existing sound with this key first
  if (engine->isActive(key)) {
    engine->clear(key);
  }

  return engine->play(key, it->second, loop, _sfxVolume, force);
}

void AudioController::stopSound(const std::string &key) {
  auto *engine = AudioEngine::get();
  if (engine != nullptr && engine->isActive(key)) {
    engine->clear(key);  // AudioEngine uses clear() to stop sounds
  }
}

void AudioController::clear(const std::string &key) {
  auto *engine = AudioEngine::get();
  if (engine != nullptr && engine->isActive(key)) {
    engine->clear(key);
  }
}

bool AudioController::isPlaying(const std::string &key) {
  auto *engine = AudioEngine::get();
  if (engine == nullptr) {
    return false;
  }
  return engine->isActive(key) &&
         engine->getState(key) == AudioEngine::State::PLAYING;
}

void AudioController::setVolume(float volume) {
  _sfxVolume = std::max(0.0f, std::min(1.0f, volume));
}

