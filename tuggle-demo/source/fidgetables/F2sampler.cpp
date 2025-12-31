//
//  F2sampler.cpp
//  Tuggle
//
//  Implementation of F2sampler - four buttons with different haptic styles.
//

#include "F2sampler.h"
#include "InputController.h"

using namespace cugl;
using namespace cugl::scene2;

// Button colors - each button has a distinct color
static const Color4 COLORS[] = {
    Color4(255, 107, 107, 255), // Coral red (RATTLE)
    Color4(78, 205, 196, 255),  // Teal (BUZZ)
    Color4(255, 230, 109, 255), // Yellow (TICK)
    Color4(170, 111, 255, 255)  // Purple (THUMP)
};

static const Color4 COLORS_PRESSED[] = {
    Color4(225, 77, 77, 255), Color4(48, 175, 166, 255),
    Color4(225, 200, 79, 255), Color4(140, 81, 225, 255)};

static const Color4 COLORS_INACTIVE[] = {
    Color4(200, 150, 150, 255), Color4(150, 200, 195, 255),
    Color4(200, 195, 150, 255), Color4(180, 160, 200, 255)};

F2sampler::F2sampler() : FidgetableView() {
  for (int i = 0; i < 4; i++) {
    _buttonActive[i] = false;
  }
}

F2sampler::~F2sampler() { dispose(); }

bool F2sampler::init(int index, const Size &pageSize) {
  return FidgetableView::init(index, pageSize);
}

std::shared_ptr<F2sampler> F2sampler::alloc(const Size &pageSize) {
  auto result = std::make_shared<F2sampler>();
  if (result->init(2, pageSize)) {
    return result;
  }
  return nullptr;
}

void F2sampler::dispose() {
  for (auto &btn : _buttons) {
    btn.deactivate();
  }
  FidgetableView::dispose();
}

void F2sampler::buildContent() {
  float radius = _pageSize.width * BUTTON_RADIUS_RATIO;
  float spacing = _pageSize.width * BUTTON_SPACING_RATIO;

  // Calculate grid positions (2x2 centered)
  Vec2 center(_pageSize.width / 2, _pageSize.height / 2);
  float halfGrid = radius + spacing / 2;

  Vec2 positions[] = {
      Vec2(center.x - halfGrid, center.y + halfGrid), // Top-left
      Vec2(center.x + halfGrid, center.y + halfGrid), // Top-right
      Vec2(center.x - halfGrid, center.y - halfGrid), // Bottom-left
      Vec2(center.x + halfGrid, center.y - halfGrid)  // Bottom-right
  };

  HapticStyle styles[] = {HapticStyle::RATTLE, HapticStyle::BUZZ,
                          HapticStyle::TICK, HapticStyle::THUMP};

  for (int i = 0; i < 4; i++) {
    HapticButton::Config config;
    config.radius = radius;
    config.normalColor = COLORS[i];
    config.pressedColor = COLORS_PRESSED[i];
    config.inactiveColor = COLORS_INACTIVE[i];
    config.hapticStyle = styles[i];
    config.maxDragDistance = 10.0f;
    config.springK = 0.5f;
    config.sceneHeight = _pageSize.height;

    _buttons[i].init(positions[i], config);

    // Set up listener
    auto btnNode = _buttons[i].getNode();
    btnNode->setName("f2sampler_button_" + std::to_string(i));

    int btnIndex = i;
    btnNode->addListener([this, btnIndex](const std::string &name, bool down) {
      if (_isActive) {
        if (down) {
          _buttonActive[btnIndex] = true;
          _isInteracting = true;
          _buttons[btnIndex].onPressed(
              InputController::getInstance()->getPosition());
        } else {
          _buttons[btnIndex].onReleased();
          _buttonActive[btnIndex] = false;
          // Check if any button is still active
          _isInteracting = false;
          for (int j = 0; j < 4; j++) {
            if (_buttonActive[j]) {
              _isInteracting = true;
              break;
            }
          }
        }
      }
    });

    _rootNode->addChild(btnNode);
  }
}

void F2sampler::update(float timestep) {
  FidgetableView::update(timestep);

  Vec2 pointerPos = InputController::getInstance()->getPosition();

  for (int i = 0; i < 4; i++) {
    // Update active buttons with drag, others just animate
    if (_buttonActive[i]) {
      _buttons[i].update(timestep, pointerPos);
    } else {
      // Still need to animate scale for non-active buttons
      _buttons[i].update(timestep, Vec2::ZERO);
    }
  }
}

void F2sampler::setActive(bool active) {
  FidgetableView::setActive(active);

  for (auto &btn : _buttons) {
    btn.setActive(active);
  }
}

void F2sampler::activateInputs() {
  for (auto &btn : _buttons) {
    btn.activate();
  }
}

void F2sampler::deactivateInputs() {
  for (auto &btn : _buttons) {
    btn.deactivate();
  }
}
