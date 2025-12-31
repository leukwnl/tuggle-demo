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

std::shared_ptr<PolygonNode> HapticButton::createCircle(float radius, Color4 color) {
  // Create a filled circle using a triangle fan (matching FidgetableView)
  constexpr int segments = 64;
  std::vector<Vec2> vertices;
  vertices.reserve(segments + 2);
  vertices.push_back(Vec2::ZERO); // Center
  
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

bool HapticButton::init(Vec2 position, const Config& config) {
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

void HapticButton::onPressed(Vec2 pointerPos) {
  _isPressed = true;
  _isDragging = true;
  _dragOffset = Vec2::ZERO;
  _dragVelocity = Vec2::ZERO;
  _lastPointerPos = pointerPos;
  _hapticCooldown = 0.0f;
  
  // Initial press haptic based on style
  switch (_config.hapticStyle) {
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
  }
}

void HapticButton::onReleased() {
  _isPressed = false;
  _isDragging = false;
  _dragOffset = Vec2::ZERO;
  _dragVelocity = Vec2::ZERO;
  
  if (_button) {
    _button->setPosition(_originalPos);
  }
}

void HapticButton::update(float timestep, Vec2 currentPointerPos) {
  if (!_button) return;
  
  // Update drag if active
  if (_isDragging) {
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
    
    if (targetDist > 0.001f) {
      float normalizedDist = std::min(targetDist / _config.maxDragDistance, 1.0f);
      float dampening = 1.0f - (_config.springK * normalizedDist * normalizedDist);
      dampening = std::max(0.1f, dampening);
      
      targetOffset = _dragOffset + pointerDelta * dampening;
      
      float clampedDist = targetOffset.length();
      if (clampedDist > _config.maxDragDistance) {
        targetOffset = targetOffset.getNormalization() * _config.maxDragDistance;
      }
    }
    
    _dragOffset = targetOffset;
    _button->setPosition(_originalPos + _dragOffset);
    
    // Trigger style-specific haptics
    triggerHaptic(_dragVelocity.length(), timestep);
  }
  
  // Animate scale
  float targetScale = _isPressed ? _config.pressedScale : 1.0f;
  if (std::abs(_currentScale - targetScale) > 0.001f) {
    float speed = timestep / _config.animDuration;
    float diff = targetScale - _currentScale;
    _currentScale += diff * std::min(1.0f, speed * 8.0f);
    _button->setScale(_currentScale);
  } else {
    _currentScale = targetScale;
    _button->setScale(_currentScale);
  }
}

void HapticButton::triggerHaptic(float velocity, float timestep) {
  _hapticCooldown -= timestep;
  if (_hapticCooldown > 0.0f) return;
  
  float intensity = std::min(velocity, MAX_HAPTIC_VELOCITY) / MAX_HAPTIC_VELOCITY;
  if (intensity < MIN_HAPTIC_INTENSITY) return;
  
  switch (_config.hapticStyle) {
    case HapticStyle::RATTLE:
      // Velocity-based transient (like F1tancho)
      Haptics::transient(intensity, 0.5f);
      _hapticCooldown = 0.04f * (1.0f - intensity * 0.5f) + 0.02f;
      break;
      
    case HapticStyle::BUZZ:
      // Soft continuous-feeling buzz (lower sharpness)
      Haptics::transient(intensity * 0.7f, 0.2f);
      _hapticCooldown = 0.03f;
      break;
      
    case HapticStyle::TICK:
      // Sharp, distinct ticks (high sharpness, fixed intensity)
      if (intensity > 0.3f) {
        Haptics::transient(0.8f, 0.9f);
        _hapticCooldown = 0.08f; // Slower, more distinct
      }
      break;
      
    case HapticStyle::THUMP:
      // Heavy thumps based on drag distance
      {
        float distRatio = _dragOffset.length() / _config.maxDragDistance;
        if (distRatio > 0.5f && intensity > 0.2f) {
          Haptics::transient(0.9f, 0.1f); // Very low sharpness = heavy thump
          _hapticCooldown = 0.12f;
        }
      }
      break;
  }
}

void HapticButton::setActive(bool active) {
  _isActive = active;
  if (_buttonNode) {
    _buttonNode->setColor(active ? _config.normalColor : _config.inactiveColor);
  }
}

void HapticButton::activate() {
  if (_button) _button->activate();
}

void HapticButton::deactivate() {
  if (_button) _button->deactivate();
}

