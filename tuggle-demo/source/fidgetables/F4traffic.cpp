//
//  F4traffic.cpp
//  Tuggle
//
//  Implementation of F4traffic - three hold-to-pop buttons.
//

#include "F4traffic.h"
#include "InputController.h"
#include <cmath>

using namespace cugl;
using namespace cugl::scene2;

// Button colors
static const Color4 BUTTON_COLORS[] = {
    Color4(255, 80, 80, 255),   // Red
    Color4(255, 200, 50, 255),  // Yellow
    Color4(80, 200, 80, 255)    // Green
};

static const Color4 BUTTON_COLORS_PRESSED[] = {
    Color4(225, 50, 50, 255),
    Color4(225, 170, 20, 255),
    Color4(50, 170, 50, 255)
};

static const Color4 BUTTON_COLORS_INACTIVE[] = {
    Color4(200, 140, 140, 255),
    Color4(200, 190, 130, 255),
    Color4(140, 190, 140, 255)
};

// Haptic parameters - very distinct between buttons
// Red: deep rumble, Yellow: medium thud, Green: sharp tick
static const float BUTTON_SHARPNESS[] = {0.0f, 0.25f, 1.0f};
static const float BUTTON_INTENSITY[] = {1.0f, 0.85f, 0.6f};
// Different speed curves per button
static const float BUTTON_INITIAL_INTERVAL[] = {0.10f, 0.07f, 0.05f};
static const float BUTTON_FINAL_INTERVAL[] = {0.025f, 0.018f, 0.012f};

#define BUTTON_RADIUS_RATIO 0.12f
#define SPACING_RATIO 0.08f

#pragma mark -
#pragma mark Constructors

F4traffic::F4traffic() 
    : FidgetableView(), _buttonRadius(50.0f), _heldButton(-1), 
      _holdTime(0.0f), _currentScale(1.0f), _hapticTimer(0.0f) {
  for (int i = 0; i < NUM_TRAFFIC_BUTTONS; i++) {
    _alphas[i] = 1.0f;
    _popStates[i] = PopState::NONE;
    _stateTimers[i] = 0.0f;
  }
}

F4traffic::~F4traffic() { dispose(); }

bool F4traffic::init(int index, const cugl::Size& pageSize) {
  _buttonRadius = pageSize.width * BUTTON_RADIUS_RATIO;
  return FidgetableView::init(index, pageSize);
}

std::shared_ptr<F4traffic> F4traffic::alloc(const cugl::Size& pageSize) {
  auto result = std::make_shared<F4traffic>();
  if (result->init(4, pageSize)) {
    return result;
  }
  return nullptr;
}

void F4traffic::dispose() {
  for (int i = 0; i < NUM_TRAFFIC_BUTTONS; i++) {
    if (_buttons[i] != nullptr) {
      _buttons[i]->deactivate();
      _buttons[i]->clearListeners();
      _buttons[i] = nullptr;
    }
    _normalNodes[i] = nullptr;
    _pressedNodes[i] = nullptr;
  }
  FidgetableView::dispose();
}

#pragma mark -
#pragma mark Content Building

void F4traffic::buildContent() {
  float spacing = _pageSize.width * SPACING_RATIO;
  float totalHeight = NUM_TRAFFIC_BUTTONS * (_buttonRadius * 2) + 
                      (NUM_TRAFFIC_BUTTONS - 1) * spacing;
  
  float startY = (_pageSize.height + totalHeight) / 2 - _buttonRadius;
  float centerX = _pageSize.width / 2;

  for (int i = 0; i < NUM_TRAFFIC_BUTTONS; i++) {
    Vec2 pos(centerX, startY - i * (_buttonRadius * 2 + spacing));

    auto normalNode = createCircle(_buttonRadius, BUTTON_COLORS[i]);
    auto pressedNode = createCircle(_buttonRadius, BUTTON_COLORS_PRESSED[i]);

    _normalNodes[i] = normalNode;
    _pressedNodes[i] = pressedNode;

    _buttons[i] = Button::alloc(normalNode, pressedNode);
    _buttons[i]->setAnchor(Vec2::ANCHOR_CENTER);
    _buttons[i]->setPosition(pos);
    _buttons[i]->setName("f4traffic_btn_" + std::to_string(i));

    int btnIndex = i;
    _buttons[i]->addListener(
        [this, btnIndex](const std::string& name, bool down) {
          if (_isActive) {
            if (down) {
              onPressed(btnIndex);
            } else {
              onReleased(btnIndex);
            }
          }
        });

    _rootNode->addChild(_buttons[i]);
  }
}

#pragma mark -
#pragma mark Interaction

void F4traffic::onPressed(int index) {
  // Can't press a button that's not in normal state
  if (_popStates[index] != PopState::NONE) return;
  
  _heldButton = index;
  _holdTime = 0.0f;
  _currentScale = 1.0f;
  _hapticTimer = 0.0f;
  _isInteracting = true;
}

void F4traffic::onReleased(int index) {
  if (_heldButton == index) {
    if (_buttons[index] != nullptr) {
      _buttons[index]->setScale(1.0f);
    }
    _heldButton = -1;
    _holdTime = 0.0f;
    _currentScale = 1.0f;
    _isInteracting = false;
  }
}

void F4traffic::popButton(int index) {
  CULog("F4traffic button %d POPPED!", index);
  
  // Distinct pop haptic - heavy impact then opposite sharpness for contrast
  // This creates a "thunk-tick" or "tick-thunk" feel depending on button
  Haptics::heavy();  // Strong base impact
  
  // Start fade out
  _popStates[index] = PopState::FADING_OUT;
  _stateTimers[index] = FADE_OUT_DURATION;
  _alphas[index] = 1.0f;
  
  _heldButton = -1;
  _holdTime = 0.0f;
  _isInteracting = false;
}

void F4traffic::applyButtonAlpha(int index) {
  if (_normalNodes[index] == nullptr || _pressedNodes[index] == nullptr) return;
  
  Uint8 alpha = (Uint8)(255 * std::max(0.0f, std::min(1.0f, _alphas[index])));
  
  Color4 normalColor = _isActive ? BUTTON_COLORS[index] : BUTTON_COLORS_INACTIVE[index];
  normalColor.a = alpha;
  _normalNodes[index]->setColor(normalColor);
  
  Color4 pressedColor = BUTTON_COLORS_PRESSED[index];
  pressedColor.a = alpha;
  _pressedNodes[index]->setColor(pressedColor);
}

void F4traffic::update(float timestep) {
  FidgetableView::update(timestep);

  // Handle held button growing
  if (_heldButton >= 0 && _isActive) {
    _holdTime += timestep;
    
    float progress = std::min(_holdTime / POP_TIME, 1.0f);
    float easedProgress = 1.0f - (1.0f - progress) * (1.0f - progress);
    _currentScale = 1.0f + (MAX_SCALE - 1.0f) * easedProgress;
    
    if (_buttons[_heldButton] != nullptr) {
      _buttons[_heldButton]->setScale(_currentScale);
    }
    
    // Accelerating haptics - each button has its own speed curve
    _hapticTimer -= timestep;
    if (_hapticTimer <= 0.0f) {
      float initInterval = BUTTON_INITIAL_INTERVAL[_heldButton];
      float finalInterval = BUTTON_FINAL_INTERVAL[_heldButton];
      float interval = initInterval + (finalInterval - initInterval) * progress;
      
      float baseIntensity = BUTTON_INTENSITY[_heldButton];
      float intensity = baseIntensity + (1.0f - baseIntensity) * progress;
      Haptics::transient(intensity, BUTTON_SHARPNESS[_heldButton]);
      
      _hapticTimer = interval;
    }
    
    // Check for pop
    if (_holdTime >= POP_TIME) {
      popButton(_heldButton);
    }
  }
  
  // Handle pop state machine for each button
  for (int i = 0; i < NUM_TRAFFIC_BUTTONS; i++) {
    if (_buttons[i] == nullptr) continue;
    
    switch (_popStates[i]) {
      case PopState::NONE:
        // Normal state, nothing to do
        break;
        
      case PopState::FADING_OUT:
        _stateTimers[i] -= timestep;
        _alphas[i] = std::max(0.0f, _stateTimers[i] / FADE_OUT_DURATION);
        applyButtonAlpha(i);
        
        if (_stateTimers[i] <= 0.0f) {
          _alphas[i] = 0.0f;
          _buttons[i]->setScale(1.0f);
          _popStates[i] = PopState::WAITING;
          _stateTimers[i] = RESPAWN_DELAY;
          applyButtonAlpha(i);
        }
        break;
        
      case PopState::WAITING:
        _stateTimers[i] -= timestep;
        if (_stateTimers[i] <= 0.0f) {
          _popStates[i] = PopState::FADING_IN;
          _stateTimers[i] = FADE_IN_DURATION;
          _alphas[i] = 0.0f;
        }
        break;
        
      case PopState::FADING_IN:
        _stateTimers[i] -= timestep;
        _alphas[i] = 1.0f - std::max(0.0f, _stateTimers[i] / FADE_IN_DURATION);
        applyButtonAlpha(i);
        
        if (_stateTimers[i] <= 0.0f) {
          _alphas[i] = 1.0f;
          _popStates[i] = PopState::NONE;
          applyButtonAlpha(i);
          CULog("F4traffic button %d respawned", i);
        }
        break;
    }
  }
}

void F4traffic::setActive(bool active) {
  FidgetableView::setActive(active);

  for (int i = 0; i < NUM_TRAFFIC_BUTTONS; i++) {
    if (_normalNodes[i] != nullptr && _popStates[i] == PopState::NONE) {
      if (active) {
        _normalNodes[i]->setColor(BUTTON_COLORS[i]);
      } else {
        _normalNodes[i]->setColor(BUTTON_COLORS_INACTIVE[i]);
      }
    }
  }
}

void F4traffic::activateInputs() {
  for (auto& btn : _buttons) {
    if (btn != nullptr) btn->activate();
  }
}

void F4traffic::deactivateInputs() {
  for (auto& btn : _buttons) {
    if (btn != nullptr) btn->deactivate();
  }
}
