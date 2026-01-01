//
//  F1tancho.cpp
//  Tuggle
//
//  Implementation of F1tancho - a single button toy.
//

#include "F1tancho.h"
#include "InputController.h"
#include <cmath>
#include <cugl/cugl.h>

using namespace cugl;
using namespace cugl::scene2;

// Button appearance
#define BUTTON_RADIUS_RATIO 0.25f
#define BUTTON_COLOR_NORMAL Color4(100, 149, 237, 255)   // Cornflower blue
#define BUTTON_COLOR_PRESSED Color4(70, 119, 207, 255)   // Darker blue
#define BUTTON_COLOR_INACTIVE Color4(150, 180, 220, 255) // Dimmed blue

#pragma mark -
#pragma mark Constructors

F1tancho::F1tancho()
    : FidgetableView(), _buttonRadius(100.0f), _isPressed(false),
      _currentScale(1.0f), _isDragging(false), _hapticCooldown(0.0f) {}

F1tancho::~F1tancho() { dispose(); }

bool F1tancho::init(int index, const cugl::Size &pageSize)
{
  _buttonRadius = pageSize.width * BUTTON_RADIUS_RATIO;
  return FidgetableView::init(index, pageSize);
}

std::shared_ptr<F1tancho> F1tancho::alloc(const cugl::Size &pageSize)
{
  std::shared_ptr<F1tancho> result = std::make_shared<F1tancho>();
  if (result->init(1, pageSize))
  {
    return result;
  }
  return nullptr;
}

void F1tancho::dispose()
{
  if (_button != nullptr)
  {
    _button->deactivate();
    _button->clearListeners();
    _button = nullptr;
  }
  _buttonNode = nullptr;
  FidgetableView::dispose();
}

#pragma mark -
#pragma mark Content Building

void F1tancho::buildContent()
{
  // Calculate center position
  Vec2 centerPos(_pageSize.width / 2, _pageSize.height / 2);

  // Store original position for drag reset
  _originalPos = centerPos;

  // Create button visuals
  auto normalNode = createCircle(_buttonRadius, BUTTON_COLOR_NORMAL);
  auto pressedNode = createCircle(_buttonRadius, BUTTON_COLOR_PRESSED);

  // Store reference to normal node for color changes
  _buttonNode = normalNode;

  // Create the button
  _button = Button::alloc(normalNode, pressedNode);
  _button->setAnchor(Vec2::ANCHOR_CENTER);
  _button->setPosition(centerPos);
  _button->setName("f1tancho_button");

  // Set up listener - handle both press and release
  _button->addListener([this](const std::string &name, bool down)
                       {
    if (_isActive) {
      if (down) {
        onPressed();
      } else {
        onReleased();
      }
    } });

  // Add to root node
  _rootNode->addChild(_button);
}

#pragma mark -
#pragma mark Interaction

void F1tancho::onPressed()
{
  CULog("F1tancho pressed");

  // Trigger initial haptic feedback for button press
  cugl::Haptics::medium();

  // Mark as pressed (will animate to shrunk state)
  _isPressed = true;

  // Mark as interacting (suspends carousel swiping)
  _isInteracting = true;

  // Start drag interaction
  _isDragging = true;
  _dragOffset = Vec2::ZERO;
  _dragVelocity = Vec2::ZERO;
  _dragStartPointerPos = InputController::getInstance()->getPosition();
  _lastPointerPos = _dragStartPointerPos;
  _hapticCooldown = 0.0f;
}

void F1tancho::onReleased()
{
  CULog("F1tancho released");

  // Mark as released (will animate back to normal)
  _isPressed = false;

  // End interaction (resumes carousel swiping)
  _isInteracting = false;

  // End drag - snap back instantly
  _isDragging = false;
  _dragOffset = Vec2::ZERO;
  _dragVelocity = Vec2::ZERO;

  // Reset button to original position
  if (_button != nullptr)
  {
    _button->setPosition(_originalPos);
  }
}

void F1tancho::updateDrag(float timestep)
{
  if (!_isDragging || _button == nullptr)
    return;

  InputController *input = InputController::getInstance();
  Vec2 currentPointerPos = input->getPosition();

  // Calculate frame-to-frame delta movement
  Vec2 pointerDelta = currentPointerPos - _lastPointerPos;

  // Scale delta to scene coordinates
  Size displaySize = Application::get()->getDisplaySize();
  float scale = _pageSize.height / displaySize.height;
  pointerDelta = pointerDelta * scale;
  pointerDelta.y = -pointerDelta.y;

  // Calculate velocity for haptics
  _dragVelocity = pointerDelta / std::max(timestep, 0.001f);
  _lastPointerPos = currentPointerPos;

  // Calculate target offset by adding frame delta to current offset
  Vec2 targetOffset = _dragOffset + pointerDelta;

  // Apply spring resistance - the further from center, the more we dampen
  float targetDist = targetOffset.length();
  if (targetDist > 0.001f)
  {
    // Non-linear spring: movement is dampened more as we approach max distance
    float normalizedDist = std::min(targetDist / MAX_DRAG_DISTANCE, 1.0f);
    float dampening = 1.0f - (SPRING_K * normalizedDist * normalizedDist);
    dampening = std::max(0.1f, dampening);

    // Apply dampening to the delta, not the total offset
    targetOffset = _dragOffset + pointerDelta * dampening;

    // Hard clamp to maximum distance
    float clampedDist = targetOffset.length();
    if (clampedDist > MAX_DRAG_DISTANCE)
    {
      targetOffset = targetOffset.getNormalization() * MAX_DRAG_DISTANCE;
    }
  }

  _dragOffset = targetOffset;

  // Update button position
  _button->setPosition(_originalPos + _dragOffset);

  // Trigger rattling haptics based on velocity
  float velocityMag = _dragVelocity.length();
  triggerRattleHaptic(velocityMag, timestep);
}

void F1tancho::triggerRattleHaptic(float velocity, float timestep)
{
  // Decrease cooldown
  _hapticCooldown -= timestep;
  if (_hapticCooldown > 0.0f)
    return;

  // Calculate intensity based on velocity
  float intensity =
      std::min(velocity, MAX_HAPTIC_VELOCITY) / MAX_HAPTIC_VELOCITY;

  // Only trigger if intensity is above threshold
  if (intensity > MIN_HAPTIC_INTENSITY)
  {
    cugl::Haptics::transient(intensity, 0.5f);
    // Set cooldown - faster velocity = shorter cooldown (more frequent taps)
    float cooldownScale = 1.0f - (intensity * 0.5f);
    _hapticCooldown = 0.04f * cooldownScale + 0.02f; // 20-60ms between taps
  }
}

void F1tancho::update(float timestep)
{
  FidgetableView::update(timestep);

  // Update drag interaction
  updateDrag(timestep);

  // Animate button scale based on pressed state
  if (_button != nullptr)
  {
    float targetScale = _isPressed ? PRESSED_SCALE : 1.0f;

    if (std::abs(_currentScale - targetScale) > 0.001f)
    {
      // Ease out interpolation toward target
      float speed = timestep / ANIM_DURATION;
      float diff = targetScale - _currentScale;
      _currentScale += diff * std::min(1.0f, speed * 8.0f);
      _button->setScale(_currentScale);
    }
    else
    {
      _currentScale = targetScale;
      _button->setScale(_currentScale);
    }
  }
}

void F1tancho::setActive(bool active)
{
  FidgetableView::setActive(active);

  // Update button appearance based on active state
  if (_buttonNode != nullptr)
  {
    if (active)
    {
      _buttonNode->setColor(BUTTON_COLOR_NORMAL);
    }
    else
    {
      _buttonNode->setColor(BUTTON_COLOR_INACTIVE);
    }
  }
}

void F1tancho::activateInputs()
{
  if (_button != nullptr)
  {
    _button->activate();
  }
}

void F1tancho::deactivateInputs()
{
  if (_button != nullptr)
  {
    _button->deactivate();
  }
}
