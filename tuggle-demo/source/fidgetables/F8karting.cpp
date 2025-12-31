//
//  F8karting.cpp
//  Tuggle
//
//  Implementation of F8karting - steering wheel with rotation haptics.
//

#include "F8karting.h"
#include <cugl/cugl.h>
#include <cmath>

using namespace cugl;
using namespace cugl::scene2;

// Visual appearance
#define WHEEL_RADIUS_RATIO 0.35f
#define HUB_RADIUS_RATIO 0.08f
#define INDICATOR_LENGTH_RATIO 0.28f
#define RING_WIDTH 12.0f

// Colors - racing/sporty theme
#define WHEEL_COLOR Color4(60, 60, 70, 255)           // Dark gray
#define WHEEL_COLOR_INACTIVE Color4(45, 45, 50, 255)  // Darker
#define HUB_COLOR Color4(200, 50, 50, 255)            // Red center
#define HUB_COLOR_INACTIVE Color4(120, 50, 50, 255)   // Dimmed red
#define INDICATOR_COLOR Color4(255, 200, 50, 255)     // Yellow indicator
#define INDICATOR_COLOR_INACTIVE Color4(150, 120, 40, 255)
#define TICK_COLOR Color4(100, 100, 110, 255)         // Subtle tick marks
#define TICK_COLOR_INACTIVE Color4(60, 60, 65, 255)

// Number of visual tick marks
#define NUM_TICK_MARKS 12

#pragma mark -
#pragma mark Constructors

F8karting::F8karting()
    : FidgetableView(), _wheelRadius(150.0f), _hubRadius(30.0f),
      _currentAngle(0.0f), _lastTickIndex(0), _smoothedAngle(0.0f) {}

F8karting::~F8karting() { dispose(); }

bool F8karting::init(int index, const cugl::Size &pageSize) {
  _wheelRadius = pageSize.width * WHEEL_RADIUS_RATIO;
  _hubRadius = pageSize.width * HUB_RADIUS_RATIO;
  return FidgetableView::init(index, pageSize);
}

std::shared_ptr<F8karting> F8karting::alloc(const cugl::Size &pageSize) {
  std::shared_ptr<F8karting> result = std::make_shared<F8karting>();
  if (result->init(8, pageSize)) {
    return result;
  }
  return nullptr;
}

void F8karting::dispose() {
  _wheelNode = nullptr;
  _hubNode = nullptr;
  _indicatorNode = nullptr;
  _tickMarks.clear();
  FidgetableView::dispose();
}

#pragma mark -
#pragma mark Content Building

std::shared_ptr<PolygonNode>
F8karting::createRing(float innerRadius, float outerRadius,
                      const Color4 &color) {
  const int segments = 64;
  std::vector<Vec2> outerVerts;
  std::vector<Vec2> innerVerts;

  for (int i = 0; i < segments; i++) {
    float angle = (float)i / segments * 2.0f * M_PI;
    float cos_a = cosf(angle);
    float sin_a = sinf(angle);
    outerVerts.push_back(Vec2(cos_a * outerRadius, sin_a * outerRadius));
    innerVerts.push_back(Vec2(cos_a * innerRadius, sin_a * innerRadius));
  }

  std::vector<Vec2> vertices;
  std::vector<Uint32> indices;

  for (const auto &v : outerVerts)
    vertices.push_back(v);
  for (const auto &v : innerVerts)
    vertices.push_back(v);

  for (int i = 0; i < segments; i++) {
    int next = (i + 1) % segments;
    indices.push_back(i);
    indices.push_back(segments + i);
    indices.push_back(next);
    indices.push_back(next);
    indices.push_back(segments + i);
    indices.push_back(segments + next);
  }

  Poly2 poly(vertices, indices);
  auto node = PolygonNode::allocWithPoly(poly);
  node->setColor(color);
  return node;
}

std::shared_ptr<PolygonNode> F8karting::createBar(float width, float height,
                                                   const Color4 &color) {
  std::vector<Vec2> vertices = {Vec2(-width / 2, 0), Vec2(width / 2, 0),
                                Vec2(width / 2, height),
                                Vec2(-width / 2, height)};
  std::vector<Uint32> indices = {0, 1, 2, 0, 2, 3};

  Poly2 poly(vertices, indices);
  auto node = PolygonNode::allocWithPoly(poly);
  node->setColor(color);
  return node;
}

void F8karting::buildContent() {
  _wheelCenter = Vec2(_pageSize.width / 2, _pageSize.height / 2);

  // Create the wheel ring
  float innerRadius = _wheelRadius - RING_WIDTH;
  _wheelNode = createRing(innerRadius, _wheelRadius, WHEEL_COLOR);
  _wheelNode->setAnchor(Vec2::ANCHOR_CENTER);
  _wheelNode->setPosition(_wheelCenter);
  _rootNode->addChild(_wheelNode);

  // Create tick marks around the wheel
  float tickLength = 15.0f;
  float tickWidth = 3.0f;
  for (int i = 0; i < NUM_TICK_MARKS; i++) {
    float angle = (float)i / NUM_TICK_MARKS * 2.0f * M_PI;

    auto tick = createBar(tickWidth, tickLength, TICK_COLOR);
    tick->setAnchor(Vec2(0.5f, 0.0f));

    // Position at the inner edge of the wheel
    float tickDist = innerRadius - tickLength - 5.0f;
    Vec2 tickPos = _wheelCenter + Vec2(cosf(angle) * tickDist, sinf(angle) * tickDist);
    tick->setPosition(tickPos);

    // Rotate to point outward
    tick->setAngle(angle - M_PI / 2);

    _tickMarks.push_back(tick);
    _rootNode->addChild(tick);
  }

  // Create the center hub
  _hubNode = createCircle(_hubRadius, HUB_COLOR);
  _hubNode->setAnchor(Vec2::ANCHOR_CENTER);
  _hubNode->setPosition(_wheelCenter);
  _rootNode->addChild(_hubNode);

  // Create the position indicator (a bar pointing up from center)
  float indicatorLength = _pageSize.width * INDICATOR_LENGTH_RATIO;
  float indicatorWidth = 8.0f;
  _indicatorNode = createBar(indicatorWidth, indicatorLength, INDICATOR_COLOR);
  _indicatorNode->setAnchor(Vec2(0.5f, 0.0f)); // Anchor at bottom center
  _indicatorNode->setPosition(_wheelCenter);
  _rootNode->addChild(_indicatorNode);

  // Initialize state
  _currentAngle = 0.0f;
  _lastTickIndex = 0;
  _smoothedAngle = 0.0f;
}

#pragma mark -
#pragma mark Rotation Logic

int F8karting::angleToTickIndex(float angle) {
  // Convert angle to tick index (works for full 360 degrees)
  // Each tick is TICK_ANGLE radians apart
  return (int)std::floor(angle / TICK_ANGLE);
}

void F8karting::update(float timestep) {
  FidgetableView::update(timestep);

  if (!_isActive)
    return;

  // Get accelerometer
  Accelerometer *accel = Input::get<Accelerometer>();
  if (accel == nullptr)
    return;

  Vec3 acceleration = accel->getAcceleration();

  // Calculate rotation angle from gravity vector using atan2
  // This gives full 360 degree rotation based on phone orientation
  // When phone is level (screen up): x≈0, y≈0, z≈-1
  // When tilted: x and y indicate direction of tilt
  // atan2(x, -y) gives the angle of rotation
  float rawAngle = atan2f(acceleration.x, -acceleration.y);

  // Smooth the angle reading (handle wraparound at ±π)
  float angleDiff = rawAngle - _smoothedAngle;
  // Normalize angle difference to [-π, π]
  while (angleDiff > M_PI)
    angleDiff -= 2.0f * M_PI;
  while (angleDiff < -M_PI)
    angleDiff += 2.0f * M_PI;

  _smoothedAngle += angleDiff * SMOOTHING;

  // Normalize smoothed angle to [-π, π]
  while (_smoothedAngle > M_PI)
    _smoothedAngle -= 2.0f * M_PI;
  while (_smoothedAngle < -M_PI)
    _smoothedAngle += 2.0f * M_PI;

  // Update current angle
  _currentAngle = _smoothedAngle;

  // Check for tick crossing
  int newTickIndex = angleToTickIndex(_currentAngle);
  if (newTickIndex != _lastTickIndex) {
    // Crossed a tick - trigger haptic
    Haptics::transient(0.6f, 0.85f);
    _lastTickIndex = newTickIndex;
  }

  // Update visual rotation - indicator rotates fully with phone
  if (_indicatorNode != nullptr) {
    _indicatorNode->setAngle(_currentAngle);
  }

  // Rotate the entire wheel with phone rotation
  if (_wheelNode != nullptr) {
    _wheelNode->setAngle(_currentAngle);
  }

  // Rotate tick marks with wheel
  for (int i = 0; i < (int)_tickMarks.size(); i++) {
    if (_tickMarks[i] != nullptr) {
      float baseAngle = (float)i / NUM_TICK_MARKS * 2.0f * M_PI;
      float rotatedAngle = baseAngle + _currentAngle;

      float tickDist = (_wheelRadius - RING_WIDTH) - 15.0f - 5.0f;
      Vec2 tickPos =
          _wheelCenter +
          Vec2(cosf(rotatedAngle) * tickDist, sinf(rotatedAngle) * tickDist);
      _tickMarks[i]->setPosition(tickPos);
      _tickMarks[i]->setAngle(rotatedAngle - M_PI / 2);
    }
  }
}

void F8karting::setActive(bool active) {
  FidgetableView::setActive(active);

  if (_wheelNode != nullptr) {
    _wheelNode->setColor(active ? WHEEL_COLOR : WHEEL_COLOR_INACTIVE);
  }
  if (_hubNode != nullptr) {
    _hubNode->setColor(active ? HUB_COLOR : HUB_COLOR_INACTIVE);
  }
  if (_indicatorNode != nullptr) {
    _indicatorNode->setColor(active ? INDICATOR_COLOR
                                    : INDICATOR_COLOR_INACTIVE);
  }
  for (auto &tick : _tickMarks) {
    if (tick != nullptr) {
      tick->setColor(active ? TICK_COLOR : TICK_COLOR_INACTIVE);
    }
  }

  // Reset angle when becoming active
  if (active) {
    _currentAngle = 0.0f;
    _smoothedAngle = 0.0f;
    _lastTickIndex = 0;
  }
}

void F8karting::activateInputs() {
  // No touch inputs - uses accelerometer
}

void F8karting::deactivateInputs() {
  // No touch inputs to deactivate
}
