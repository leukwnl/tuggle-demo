//
//  HapticButton.cpp
//  Tuggle
//
//  Implementation of reusable draggable button with haptic feedback.
//

#include "HapticButton.h"
#include "InputController.h"
#include <cugl/cugl.h>
#include <cmath>

using namespace cugl;
using namespace cugl::scene2;

std::shared_ptr<PolygonNode> HapticButton::createCircle(float radius, Color4 color)
{
  constexpr int segments = 64;
  std::vector<Vec2> vertices;
  vertices.reserve(segments + 2);
  vertices.push_back(Vec2::ZERO);

  for (int i = 0; i <= segments; i++)
  {
    float angle = (float)i / (float)segments * 2.0f * M_PI;
    vertices.push_back(Vec2(radius * cosf(angle), radius * sinf(angle)));
  }

  std::vector<Uint32> indices;
  indices.reserve(segments * 3);
  for (int i = 1; i <= segments; i++)
  {
    indices.push_back(0);
    indices.push_back(i);
    indices.push_back(i + 1);
  }

  auto node = PolygonNode::allocWithPoly(Poly2(vertices, indices));
  node->setColor(color);
  node->setAnchor(Vec2::ANCHOR_CENTER);
  return node;
}

bool HapticButton::init(Vec2 position, const Config &config)
{
  _config = config;
  _originalPos = position;
  _dragOffset = Vec2::ZERO;
  _dragVelocity = Vec2::ZERO;
  _currentScale = 1.0f;
  _isPressed = false;
  _isDragging = false;

  // Create button visuals
  auto normalNode = createCircle(_config.radius, _config.normalColor);
  auto pressedNode = createCircle(_config.radius, _config.pressedColor);
  _buttonNode = normalNode;

  // Create the button
  _button = Button::alloc(normalNode, pressedNode);
  _button->setAnchor(Vec2::ANCHOR_CENTER);
  _button->setPosition(position);

  return true;
}

void HapticButton::onPressed(Vec2 pointerPos)
{
  _isPressed = true;
  _isDragging = true;
  _dragOffset = Vec2::ZERO;
  _dragVelocity = Vec2::ZERO;
  _lastPointerPos = pointerPos;
  _hapticCooldown = 0.0f;

  // Initial press haptic based on style
  switch (_config.hapticStyle)
  {
  case HapticStyle::RATTLE:
    Haptics::medium();
    break;
  case HapticStyle::BUZZ:
    Haptics::light();
    break;
  case HapticStyle::TICK:
    Haptics::heavy();
    break;
  case HapticStyle::THUMP:
    Haptics::heavy();
    break;
  case HapticStyle::TAP:
    Haptics::tap(0.8f, 0.5f);
    break;
  case HapticStyle::BUZZ_PULSE:
    Haptics::buzz(0.7f, 0.3f, 0.15f);  // Short 150ms buzz on press
    break;
  case HapticStyle::SELECTION:
    Haptics::selection();
    break;
  }
}

void HapticButton::onReleased()
{
  _isPressed = false;
  _isDragging = false;
  _dragOffset = Vec2::ZERO;
  _dragVelocity = Vec2::ZERO;

  if (_button)
  {
    _button->setPosition(_originalPos);
  }
}

void HapticButton::update(float timestep, Vec2 currentPointerPos)
{
  if (!_button)
    return;

  // Update drag if active
  if (_isDragging)
  {
    Vec2 pointerDelta = currentPointerPos - _lastPointerPos;

    // Scale delta to scene coordinates
    Size displaySize = Application::get()->getDisplaySize();
    float scale = _config.sceneHeight / displaySize.height;
    pointerDelta = pointerDelta * scale;

    pointerDelta.y = -pointerDelta.y; // Flip Y for screen->scene coords

    _dragVelocity = pointerDelta / std::max(timestep, 0.001f);
    _lastPointerPos = currentPointerPos;

    // Apply spring physics
    Vec2 targetOffset = _dragOffset + pointerDelta;
    float targetDist = targetOffset.length();

    if (targetDist > 0.001f)
    {
      float normalizedDist = std::min(targetDist / _config.maxDragDistance, 1.0f);
      float dampening = 1.0f - (_config.springK * normalizedDist * normalizedDist);
      dampening = std::max(0.1f, dampening);

      targetOffset = _dragOffset + pointerDelta * dampening;

      float clampedDist = targetOffset.length();
      if (clampedDist > _config.maxDragDistance)
      {
        targetOffset = targetOffset.getNormalization() * _config.maxDragDistance;
      }
    }

    _dragOffset = targetOffset;
    _button->setPosition(_originalPos + _dragOffset);

    triggerHaptic(_dragVelocity.length(), timestep);
  }

  float targetScale = _isPressed ? _config.pressedScale : 1.0f;
  if (std::abs(_currentScale - targetScale) > 0.001f)
  {
    float speed = timestep / _config.animDuration;
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

void HapticButton::triggerHaptic(float velocity, float timestep)
{
  _hapticCooldown -= timestep;
  if (_hapticCooldown > 0.0f)
    return;

  float intensity = std::min(velocity, MAX_HAPTIC_VELOCITY) / MAX_HAPTIC_VELOCITY;
  if (intensity < MIN_HAPTIC_INTENSITY)
    return;

  switch (_config.hapticStyle)
  {
  case HapticStyle::RATTLE:
    // Velocity-based transient
    Haptics::transient(intensity, 0.5f);
    _hapticCooldown = 0.04f * (1.0f - intensity * 0.5f) + 0.02f;
    break;

  case HapticStyle::BUZZ:
    Haptics::transient(intensity * 0.7f, 0.2f);
    _hapticCooldown = 0.03f;
    break;

  case HapticStyle::TICK:
    if (intensity > 0.3f)
    {
      Haptics::transient(0.8f, 0.9f);
      _hapticCooldown = 0.08f; // Slower, more distinct
    }
    break;

  case HapticStyle::THUMP:
  {
    float distRatio = _dragOffset.length() / _config.maxDragDistance;
    if (distRatio > 0.5f && intensity > 0.2f)
    {
      Haptics::transient(0.9f, 0.1f); // Very low sharpness = heavy thump
      _hapticCooldown = 0.12f;
    }
  }
  break;

  case HapticStyle::TAP:
    // Uses tap() instead of transient() - approximated sharpness via generator
    Haptics::tap(intensity, 0.5f);
    _hapticCooldown = 0.05f * (1.0f - intensity * 0.3f) + 0.03f;
    break;

  case HapticStyle::BUZZ_PULSE:
    // Short continuous buzz triggered by velocity
    if (intensity > 0.25f)
    {
      float duration = 0.05f + intensity * 0.1f;  // 50-150ms based on velocity
      Haptics::buzz(intensity * 0.8f, 0.25f, duration);
      _hapticCooldown = duration + 0.05f;  // Wait for buzz to finish + small gap
    }
    break;

  case HapticStyle::SELECTION:
    // Selection tick on each movement threshold
    if (intensity > 0.15f)
    {
      Haptics::selection();
      _hapticCooldown = 0.06f;  // Quick selection ticks
    }
    break;
  }
}

void HapticButton::setActive(bool active)
{
  _isActive = active;
  if (_buttonNode)
  {
    _buttonNode->setColor(active ? _config.normalColor : _config.inactiveColor);
  }
}

void HapticButton::activate()
{
  if (_button)
    _button->activate();
}

void HapticButton::deactivate()
{
  if (_button)
    _button->deactivate();
}
