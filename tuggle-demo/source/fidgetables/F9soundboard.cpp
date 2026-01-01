//
//  F9soundboard.cpp
//  Tuggle
//
//  Implementation of F9soundboard - a 3x3 soundboard with synchronized
//  audio and AHAP haptic playback.
//

#include "F9soundboard.h"
#include "../AudioController.h"
#include "InputController.h"
#include <cmath>

using namespace cugl;
using namespace cugl::scene2;
using namespace cugl::audio;

// Vibrant soundboard colors - each button has a distinct character
const Color4 F9soundboard::BUTTON_COLORS[NUM_BUTTONS] = {
    Color4(255, 82, 82, 255),   // Red
    Color4(255, 177, 66, 255),  // Orange
    Color4(255, 241, 118, 255), // Yellow
    Color4(129, 236, 129, 255), // Green
    Color4(100, 221, 255, 255), // Cyan
    Color4(130, 177, 255, 255), // Blue
    Color4(179, 136, 255, 255), // Purple
    Color4(255, 145, 200, 255), // Pink
    Color4(200, 200, 200, 255)  // Silver/White
};

const Color4 F9soundboard::BUTTON_COLORS_PRESSED[NUM_BUTTONS] = {
    Color4(200, 50, 50, 255),   // Red
    Color4(200, 140, 40, 255),  // Orange
    Color4(200, 190, 80, 255),  // Yellow
    Color4(90, 180, 90, 255),   // Green
    Color4(60, 170, 200, 255),  // Cyan
    Color4(90, 130, 200, 255),  // Blue
    Color4(140, 100, 200, 255), // Purple
    Color4(200, 110, 160, 255), // Pink
    Color4(150, 150, 150, 255)  // Silver/White
};

// Brighter/glowing colors for active playback
const Color4 F9soundboard::BUTTON_COLORS_PLAYING[NUM_BUTTONS] = {
    Color4(255, 120, 120, 255), // Red glow
    Color4(255, 200, 100, 255), // Orange glow
    Color4(255, 250, 160, 255), // Yellow glow
    Color4(160, 255, 160, 255), // Green glow
    Color4(140, 240, 255, 255), // Cyan glow
    Color4(170, 210, 255, 255), // Blue glow
    Color4(210, 180, 255, 255), // Purple glow
    Color4(255, 185, 230, 255), // Pink glow
    Color4(240, 240, 240, 255)  // Silver glow
};

const Color4 F9soundboard::BUTTON_COLORS_INACTIVE[NUM_BUTTONS] = {
    Color4(180, 120, 120, 255), // Red dimmed
    Color4(180, 155, 115, 255), // Orange dimmed
    Color4(180, 175, 135, 255), // Yellow dimmed
    Color4(135, 170, 135, 255), // Green dimmed
    Color4(130, 165, 180, 255), // Cyan dimmed
    Color4(135, 150, 180, 255), // Blue dimmed
    Color4(155, 140, 180, 255), // Purple dimmed
    Color4(180, 145, 165, 255), // Pink dimmed
    Color4(160, 160, 160, 255)  // Silver dimmed
};

F9soundboard::F9soundboard()
    : FidgetableView(), _activeButton(-1), _playbackTime(0.0f),
      _isPlaybackActive(false), _currentAudioKey("f9_sound") {
  // Initialize AHAP files to empty (user will set these)
  for (int i = 0; i < NUM_BUTTONS; i++) {
    _ahapFiles[i] = "";
    _sounds[i] = nullptr;
  }
}

F9soundboard::~F9soundboard() { dispose(); }

bool F9soundboard::init(int index, const Size &pageSize) {
  return FidgetableView::init(index, pageSize);
}

std::shared_ptr<F9soundboard> F9soundboard::alloc(const Size &pageSize) {
  auto result = std::make_shared<F9soundboard>();
  if (result->init(9, pageSize)) {
    return result;
  }
  return nullptr;
}

void F9soundboard::dispose() {
  stopPlayback();
  for (auto &btn : _buttons) {
    if (btn) {
      btn->deactivate();
    }
  }
  FidgetableView::dispose();
}

std::shared_ptr<PolygonNode> F9soundboard::createCircle(float radius,
                                                         Color4 color) {
  constexpr int segments = 64;
  std::vector<Vec2> vertices;
  vertices.reserve(segments + 2);
  vertices.push_back(Vec2::ZERO);

  for (int i = 0; i <= segments; i++) {
    float angle = (float)i / (float)segments * 2.0f * M_PI;
    vertices.push_back(Vec2(radius * cosf(angle), radius * sinf(angle)));
  }

  std::vector<Uint32> indices;
  indices.reserve(segments * 3);
  for (int i = 1; i <= segments; i++) {
    indices.push_back(0);
    indices.push_back(i);
    indices.push_back(i + 1);
  }

  auto node = PolygonNode::allocWithPoly(Poly2(vertices, indices));
  node->setColor(color);
  node->setAnchor(Vec2::ANCHOR_CENTER);
  return node;
}

void F9soundboard::buildContent() {
  float radius = _pageSize.width * BUTTON_RADIUS_RATIO;
  float spacing = _pageSize.width * BUTTON_SPACING_RATIO;
  float cellSize = radius * 2 + spacing;

  // Calculate grid positions (3x3 centered)
  Vec2 center(_pageSize.width / 2, _pageSize.height / 2);

  // Row heights
  float row1Y = center.y + cellSize;  // Top row
  float row2Y = center.y;             // Middle row
  float row3Y = center.y - cellSize;  // Bottom row

  // Column positions
  float col1X = center.x - cellSize;
  float col2X = center.x;
  float col3X = center.x + cellSize;

  Vec2 positions[] = {
      // Row 1
      Vec2(col1X, row1Y), Vec2(col2X, row1Y), Vec2(col3X, row1Y),
      // Row 2
      Vec2(col1X, row2Y), Vec2(col2X, row2Y), Vec2(col3X, row2Y),
      // Row 3
      Vec2(col1X, row3Y), Vec2(col2X, row3Y), Vec2(col3X, row3Y)};

  for (int i = 0; i < NUM_BUTTONS; i++) {
    // Create button visuals
    auto normalNode = createCircle(radius, BUTTON_COLORS[i]);
    auto pressedNode = createCircle(radius, BUTTON_COLORS_PRESSED[i]);
    _buttonNodes[i] = normalNode;

    // Create the button
    _buttons[i] = Button::alloc(normalNode, pressedNode);
    _buttons[i]->setAnchor(Vec2::ANCHOR_CENTER);
    _buttons[i]->setPosition(positions[i]);
    _buttons[i]->setName("f9soundboard_button_" + std::to_string(i));

    // Set up listener
    int btnIndex = i;
    _buttons[i]->addListener(
        [this, btnIndex](const std::string &name, bool down) {
          if (_isActive && down) {
            triggerButton(btnIndex);
          }
        });

    _rootNode->addChild(_buttons[i]);
  }
}

void F9soundboard::triggerButton(int index) {
  if (index < 0 || index >= NUM_BUTTONS)
    return;

  // Stop any current playback
  stopPlayback();

  // Set this button as active
  _activeButton = index;
  _playbackTime = 0.0f;
  _isPlaybackActive = true;
  _isInteracting = true;

  // Play haptic feedback (AHAP file if set, otherwise default heavy)
  if (!_ahapFiles[index].empty()) {
    Haptics::play(_ahapFiles[index]);
  } else {
    // Default haptic: heavy impact for press
    Haptics::heavy();
  }

  // Play audio using AudioController with button-specific key
  std::string soundKey = "f9_sound_" + std::to_string(index);
  _currentAudioKey = soundKey;
  AudioController::get()->play(soundKey);

  // Update visual to show playing state
  if (_buttonNodes[index]) {
    _buttonNodes[index]->setColor(BUTTON_COLORS_PLAYING[index]);
  }
}

void F9soundboard::stopPlayback() {
  if (_isPlaybackActive) {
    // Stop audio using AudioController
    AudioController::get()->clear(_currentAudioKey);

    // Reset active button visual
    if (_activeButton >= 0 && _activeButton < NUM_BUTTONS) {
      if (_buttonNodes[_activeButton]) {
        _buttonNodes[_activeButton]->setColor(
            _isActive ? BUTTON_COLORS[_activeButton]
                      : BUTTON_COLORS_INACTIVE[_activeButton]);
      }
      // Reset scale
      if (_buttons[_activeButton]) {
        _buttons[_activeButton]->setScale(1.0f);
      }
    }

    _activeButton = -1;
    _isPlaybackActive = false;
    _isInteracting = false;
  }
}

void F9soundboard::updatePlaybackVisuals(float timestep) {
  if (!_isPlaybackActive || _activeButton < 0)
    return;

  _playbackTime += timestep;

  // Check if audio has finished playing
  if (!AudioController::get()->isPlaying(_currentAudioKey)) {
    // Audio finished, reset state
    stopPlayback();
    return;
  }

  // Pulse animation for active button
  float pulse = 0.95f + 0.05f * sinf(_playbackTime * 8.0f);
  if (_buttons[_activeButton]) {
    _buttons[_activeButton]->setScale(pulse);
  }
}

void F9soundboard::update(float timestep) {
  FidgetableView::update(timestep);
  updatePlaybackVisuals(timestep);
}

void F9soundboard::setActive(bool active) {
  FidgetableView::setActive(active);

  for (int i = 0; i < NUM_BUTTONS; i++) {
    if (_buttonNodes[i]) {
      if (i == _activeButton && _isPlaybackActive) {
        // Keep playing color if this button is active
        _buttonNodes[i]->setColor(BUTTON_COLORS_PLAYING[i]);
      } else {
        _buttonNodes[i]->setColor(active ? BUTTON_COLORS[i]
                                         : BUTTON_COLORS_INACTIVE[i]);
      }
    }
  }
}

void F9soundboard::activateInputs() {
  for (auto &btn : _buttons) {
    if (btn)
      btn->activate();
  }
}

void F9soundboard::deactivateInputs() {
  for (auto &btn : _buttons) {
    if (btn)
      btn->deactivate();
  }
}

void F9soundboard::setSound(int index,
                             const std::shared_ptr<cugl::audio::Sound> &sound) {
  if (index >= 0 && index < NUM_BUTTONS) {
    _sounds[index] = sound;
    // Register with AudioController using button-specific key
    std::string soundKey = "f9_sound_" + std::to_string(index);
    AudioController::get()->registerSound(soundKey, sound);
  }
}

void F9soundboard::setHapticFile(int index, const std::string &filename) {
  if (index >= 0 && index < NUM_BUTTONS) {
    _ahapFiles[index] = filename;
  }
}

