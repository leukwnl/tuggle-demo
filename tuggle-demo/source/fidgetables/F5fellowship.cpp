//
//  F5fellowship.cpp
//  Tuggle
//
//  Implementation of F5fellowship - a circular slider with haptic ticks.
//

#include "F5fellowship.h"
#include "InputController.h"
#include <cmath>
#include <cugl/cugl.h>

using namespace cugl;
using namespace cugl::scene2;

// Visual appearance
#define RING_RADIUS_RATIO 0.28f
#define TRACK_THICKNESS_RATIO 0.04f
#define KNOB_RADIUS_RATIO 0.07f

static const Color4 TRACK_COLOR(80, 80, 100, 255);
static const Color4 TRACK_COLOR_INACTIVE(60, 60, 70, 255);
static const Color4 KNOB_COLOR(180, 140, 255, 255);
static const Color4 KNOB_COLOR_PRESSED(220, 180, 255, 255);
static const Color4 KNOB_COLOR_INACTIVE(140, 130, 160, 255);

#pragma mark -
#pragma mark Constructors

F5fellowship::F5fellowship()
    : FidgetableView(), _ringRadius(100.0f), _trackThickness(20.0f),
      _knobRadius(30.0f), _currentAngle(0.0f), _lastTickIndex(0),
      _isDragging(false) {}

F5fellowship::~F5fellowship() { dispose(); }

bool F5fellowship::init(int index, const cugl::Size &pageSize) {
  _ringRadius = pageSize.width * RING_RADIUS_RATIO;
  _trackThickness = pageSize.width * TRACK_THICKNESS_RATIO;
  _knobRadius = pageSize.width * KNOB_RADIUS_RATIO;
  return FidgetableView::init(index, pageSize);
}

std::shared_ptr<F5fellowship> F5fellowship::alloc(const cugl::Size &pageSize) {
  auto result = std::make_shared<F5fellowship>();
  if (result->init(5, pageSize)) {
    return result;
  }
  return nullptr;
}

void F5fellowship::dispose() {
  if (_knobButton != nullptr) {
    _knobButton->deactivate();
    _knobButton->clearListeners();
    _knobButton = nullptr;
  }
  _knobNode = nullptr;
  _trackNode = nullptr;
  FidgetableView::dispose();
}

#pragma mark -
#pragma mark Content Building

std::shared_ptr<PolygonNode> F5fellowship::createRing(float innerRadius,
                                                      float outerRadius,
                                                      Color4 color,
                                                      int segments) {
  std::vector<Vec2> vertices;
  std::vector<Uint32> indices;

  for (int i = 0; i <= segments; i++) {
    float angle = (float)i / (float)segments * 2.0f * M_PI;
    float cosA = cosf(angle);
    float sinA = sinf(angle);
    vertices.push_back(Vec2(innerRadius * cosA, innerRadius * sinA));
    vertices.push_back(Vec2(outerRadius * cosA, outerRadius * sinA));
  }

  for (int i = 0; i < segments; i++) {
    int base = i * 2;
    indices.push_back(base);
    indices.push_back(base + 1);
    indices.push_back(base + 2);
    indices.push_back(base + 1);
    indices.push_back(base + 3);
    indices.push_back(base + 2);
  }

  Poly2 poly(vertices, indices);
  auto node = PolygonNode::allocWithPoly(poly);
  node->setColor(color);
  node->setAnchor(Vec2::ANCHOR_CENTER);
  return node;
}

void F5fellowship::buildContent() {
  _center = Vec2(_pageSize.width / 2, _pageSize.height / 2);

  // Create the ring track
  float innerRadius = _ringRadius - _trackThickness / 2;
  float outerRadius = _ringRadius + _trackThickness / 2;
  _trackNode = createRing(innerRadius, outerRadius, TRACK_COLOR);
  _trackNode->setPosition(_center);
  _rootNode->addChild(_trackNode);

  // Create the knob as a Button for proper touch detection
  auto normalNode = createCircle(_knobRadius, KNOB_COLOR);
  auto pressedNode = createCircle(_knobRadius, KNOB_COLOR_PRESSED);
  _knobNode = normalNode;

  _knobButton = Button::alloc(normalNode, pressedNode);
  _knobButton->setAnchor(Vec2::ANCHOR_CENTER);
  _knobButton->setName("f5_knob");

  // Button listener for press/release
  _knobButton->addListener([this](const std::string &name, bool down) {
    if (_isActive) {
      if (down) {
        _isDragging = true;
        _isInteracting = true;
        Haptics::heavy();
      } else {
        _isDragging = false;
        _isInteracting = false;
      }
    }
  });

  _rootNode->addChild(_knobButton);

  // Initialize knob position
  _currentAngle = M_PI / 2; // Start at top
  _lastTickIndex = angleToTickIndex(_currentAngle);
  updateKnobPosition();
}

void F5fellowship::updateKnobPosition() {
  if (_knobButton == nullptr)
    return;

  float x = _center.x + _ringRadius * cosf(_currentAngle);
  float y = _center.y + _ringRadius * sinf(_currentAngle);
  _knobButton->setPosition(Vec2(x, y));
}

int F5fellowship::angleToTickIndex(float angle) {
  while (angle < 0)
    angle += 2.0f * M_PI;
  while (angle >= 2.0f * M_PI)
    angle -= 2.0f * M_PI;
  return (int)(angle / RADIANS_PER_TICK) % NUM_TICKS;
}

#pragma mark -
#pragma mark Update

void F5fellowship::update(float timestep) {
  FidgetableView::update(timestep);

  if (!_isActive)
    return;

  // Check if knob is being held (using button's isDown state)
  if (_knobButton != nullptr && _knobButton->isDown()) {
    _isDragging = true;
    _isInteracting = true;
  }

  if (!_isDragging)
    return;

  InputController *input = InputController::getInstance();
  Vec2 screenPos = input->getPosition();

  // Convert screen position to scene coordinates
  Size displaySize = Application::get()->getDisplaySize();
  float scale = _pageSize.height / displaySize.height;

  // Scale and flip Y
  Vec2 scenePos;
  scenePos.x = screenPos.x * scale;
  scenePos.y = _pageSize.height - (screenPos.y * scale);

  // Calculate angle directly from pointer position to center
  // This makes the knob snap to the closest point on the ring to the finger
  Vec2 toPointer = scenePos - _center;
  float newAngle = atan2f(toPointer.y, toPointer.x);

  // Check for tick crossing before updating
  int newTickIndex = angleToTickIndex(newAngle);
  if (newTickIndex != _lastTickIndex) {
    Haptics::heavy();
    _lastTickIndex = newTickIndex;
  }

  _currentAngle = newAngle;
  updateKnobPosition();
}

void F5fellowship::setActive(bool active) {
  FidgetableView::setActive(active);

  if (_trackNode != nullptr) {
    _trackNode->setColor(active ? TRACK_COLOR : TRACK_COLOR_INACTIVE);
  }
  if (_knobNode != nullptr) {
    _knobNode->setColor(active ? KNOB_COLOR : KNOB_COLOR_INACTIVE);
  }
}

void F5fellowship::activateInputs() {
  if (_knobButton != nullptr) {
    _knobButton->activate();
  }
}

void F5fellowship::deactivateInputs() {
  if (_knobButton != nullptr) {
    _knobButton->deactivate();
  }
  _isDragging = false;
  _isInteracting = false;
}
