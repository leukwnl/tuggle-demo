//
//  F7samba.cpp
//  Tuggle
//
//  Implementation of F7samba - maracas simulation with particle physics.
//

#include "F7samba.h"
#include <cmath>
#include <cstdlib>
#include <cugl/cugl.h>

using namespace cugl;
using namespace cugl::scene2;

// Visual appearance
#define CONTAINER_RADIUS_RATIO 0.38f
#define PARTICLE_RADIUS_RATIO 0.025f
#define CONTAINER_BORDER_WIDTH 6.0f

// Colors - warm maraca-inspired palette
#define CONTAINER_COLOR Color4(139, 90, 43, 255)          // Saddle brown
#define CONTAINER_COLOR_INACTIVE Color4(100, 70, 40, 255) // Darker brown
#define PARTICLE_COLORS_COUNT 5

static const Color4 PARTICLE_COLORS[] = {
    Color4(255, 200, 100, 255), // Golden yellow
    Color4(255, 150, 80, 255),  // Orange
    Color4(255, 100, 100, 255), // Coral red
    Color4(200, 255, 150, 255), // Light green
    Color4(150, 200, 255, 255)  // Light blue
};

static const Color4 PARTICLE_COLORS_INACTIVE[] = {
    Color4(180, 160, 120, 255), Color4(180, 130, 100, 255),
    Color4(180, 110, 110, 255), Color4(160, 180, 130, 255),
    Color4(130, 160, 180, 255)};

#pragma mark -
#pragma mark Constructors

F7samba::F7samba()
    : FidgetableView(), _containerRadius(150.0f), _particleRadius(12.0f),
      _maxParticleDistance(130.0f), _wallCollisionIntensity(0.0f),
      _wallCollisionCount(0), _ballCollisionIntensity(0.0f),
      _ballCollisionCount(0), _hapticCooldown(0.0f) {}

F7samba::~F7samba() { dispose(); }

bool F7samba::init(int index, const cugl::Size &pageSize) {
  // Calculate dimensions based on page size
  _containerRadius = pageSize.width * CONTAINER_RADIUS_RATIO;
  _particleRadius = pageSize.width * PARTICLE_RADIUS_RATIO;
  _maxParticleDistance =
      _containerRadius - _particleRadius - (CONTAINER_BORDER_WIDTH / 2);

  return FidgetableView::init(index, pageSize);
}

std::shared_ptr<F7samba> F7samba::alloc(const cugl::Size &pageSize) {
  std::shared_ptr<F7samba> result = std::make_shared<F7samba>();
  if (result->init(7, pageSize)) {
    return result;
  }
  return nullptr;
}

void F7samba::dispose() {
  _containerNode = nullptr;
  for (int i = 0; i < NUM_PARTICLES; i++) {
    _particleNodes[i] = nullptr;
  }
  FidgetableView::dispose();
}

#pragma mark -
#pragma mark Content Building

std::shared_ptr<PolygonNode>
F7samba::createRing(float innerRadius, float outerRadius, const Color4 &color) {

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

void F7samba::buildContent() {
  _containerCenter = Vec2(_pageSize.width / 2, _pageSize.height / 2);

  // Create container border
  float outerRadius = _containerRadius + CONTAINER_BORDER_WIDTH / 2;
  float innerRadius = _containerRadius - CONTAINER_BORDER_WIDTH / 2;
  _containerNode = createRing(innerRadius, outerRadius, CONTAINER_COLOR);
  _containerNode->setAnchor(Vec2::ANCHOR_CENTER);
  _containerNode->setPosition(_containerCenter);
  _rootNode->addChild(_containerNode);

  // Initialize and create particles - start at bottom of container
  for (int i = 0; i < NUM_PARTICLES; i++) {
    // Position particles at the bottom, spread horizontally
    // Random x position within bottom arc, y near bottom
    float xSpread =
        ((float)rand() / RAND_MAX - 0.5f) * _maxParticleDistance * 1.5f;
    float yOffset = -_maxParticleDistance * 0.7f +
                    ((float)rand() / RAND_MAX) * _maxParticleDistance * 0.3f;

    _particles[i].position = Vec2(xSpread, yOffset);

    // Clamp to container
    float dist = _particles[i].position.length();
    if (dist > _maxParticleDistance) {
      _particles[i].position =
          _particles[i].position.getNormalization() * _maxParticleDistance;
    }

    _particles[i].velocity = Vec2::ZERO;
    _particles[i].radius = _particleRadius;

    // Create visual node with cycling colors
    Color4 color = PARTICLE_COLORS[i % PARTICLE_COLORS_COUNT];
    _particleNodes[i] = createCircle(_particleRadius, color);
    _particleNodes[i]->setAnchor(Vec2::ANCHOR_CENTER);
    _particleNodes[i]->setPosition(_containerCenter + _particles[i].position);
    _rootNode->addChild(_particleNodes[i]);
  }

  _prevAcceleration = Vec3::ZERO;
}

#pragma mark -
#pragma mark Physics

void F7samba::updatePhysics(float timestep) {
  // Get accelerometer input
  Accelerometer *accel = Input::get<Accelerometer>();
  if (accel == nullptr)
    return;

  Vec3 acceleration = accel->getAcceleration();

  // Detect shake: change in acceleration
  Vec3 accelDelta = acceleration - _prevAcceleration;
  float shakeIntensity = accelDelta.length();
  _prevAcceleration = acceleration;

  // Gravity pulls particles to the "bottom" based on phone orientation
  Vec2 gravity(acceleration.x, acceleration.y);

  // Check if we're shaking
  bool isShaking = shakeIntensity > SHAKE_THRESHOLD;

  // Reset frame collision tracking
  _wallCollisionIntensity = 0.0f;
  _wallCollisionCount = 0;
  _ballCollisionIntensity = 0.0f;
  _ballCollisionCount = 0;

  // Update each particle
  for (int i = 0; i < NUM_PARTICLES; i++) {
    // Always apply gravity - this pulls particles to the bottom
    _particles[i].velocity += gravity * GRAVITY_SCALE * timestep;

    // On shake, add burst of energy in the shake direction
    if (isShaking) {
      // Add randomized shake impulse
      float randomAngle = ((float)rand() / RAND_MAX - 0.5f) * 0.6f;
      float cosR = cosf(randomAngle);
      float sinR = sinf(randomAngle);
      Vec2 shakeDir(accelDelta.x, accelDelta.y);
      Vec2 randomizedForce(shakeDir.x * cosR - shakeDir.y * sinR,
                           shakeDir.x * sinR + shakeDir.y * cosR);
      _particles[i].velocity += randomizedForce * SHAKE_ENERGY * timestep;
    }

    // Apply velocity decay (energy dissipates)
    _particles[i].velocity *= VELOCITY_DECAY;

    // Sleep check: stop if moving very slowly AND not being shaken
    float speed = _particles[i].velocity.length();
    if (speed < SLEEP_VELOCITY && !isShaking) {
      _particles[i].velocity = Vec2::ZERO;
    }

    // Update position
    _particles[i].position += _particles[i].velocity * timestep;
  }

  // Resolve collisions with multiple passes to prevent clipping
  for (int pass = 0; pass < 4; pass++) {
    // Wall collisions
    for (int i = 0; i < NUM_PARTICLES; i++) {
      resolveWallCollision(i);
    }

    // Particle-particle collisions
    for (int i = 0; i < NUM_PARTICLES; i++) {
      for (int j = i + 1; j < NUM_PARTICLES; j++) {
        resolveParticleCollision(i, j);
      }
    }
  }

  // Update visual positions
  for (int i = 0; i < NUM_PARTICLES; i++) {
    if (_particleNodes[i] != nullptr) {
      _particleNodes[i]->setPosition(_containerCenter + _particles[i].position);
    }
  }

  // Trigger haptic feedback if there were collisions
  if (_wallCollisionCount > 0 || _ballCollisionCount > 0) {
    triggerCollisionHaptic(timestep);
  }
}

void F7samba::resolveParticleCollision(int a, int b) {
  Vec2 delta = _particles[b].position - _particles[a].position;
  float dist = delta.length();
  float minDist = _particles[a].radius + _particles[b].radius;

  // Add small buffer to prevent touching
  float safeMinDist = minDist + 0.5f;

  if (dist < safeMinDist && dist > 0.001f) {
    // Collision detected
    Vec2 normal = delta / dist;
    float overlap = safeMinDist - dist;

    // Aggressively separate - push completely apart
    Vec2 separation = normal * (overlap * 0.52f);
    _particles[a].position -= separation;
    _particles[b].position += separation;

    // Check if particles are actually moving fast
    float speedA = _particles[a].velocity.length();
    float speedB = _particles[b].velocity.length();
    float maxSpeed = std::max(speedA, speedB);

    // Only do velocity resolution if particles have real speed
    if (maxSpeed > SLEEP_VELOCITY * 3.0f) {
      Vec2 relVel = _particles[b].velocity - _particles[a].velocity;
      float velAlongNormal = relVel.dot(normal);

      // Only resolve if moving toward each other
      if (velAlongNormal < 0) {
        float impulse = -(1.0f + BALL_RESTITUTION) * velAlongNormal / 2.0f;
        Vec2 impulseVec = normal * impulse;

        _particles[a].velocity -= impulseVec;
        _particles[b].velocity += impulseVec;

        // Track ball-ball collision for haptics - only high velocity
        float collisionForce = fabsf(velAlongNormal);
        if (collisionForce > 200.0f && maxSpeed > 150.0f) {
          _ballCollisionIntensity += collisionForce;
          _ballCollisionCount++;
        }
      }
    }
  }
}

void F7samba::resolveWallCollision(int index) {
  float dist = _particles[index].position.length();

  // Small buffer to keep particles slightly inside
  float safeDistance = _maxParticleDistance - 0.5f;

  if (dist > safeDistance) {
    // Collision with wall
    Vec2 normal = _particles[index].position / dist;

    // Push back inside with margin
    _particles[index].position = normal * safeDistance;

    // Check velocity into wall
    float velIntoWall = _particles[index].velocity.dot(normal);
    float totalSpeed = _particles[index].velocity.length();

    if (velIntoWall > 0) {
      // Reflect with restitution
      _particles[index].velocity -=
          normal * velIntoWall * (1.0f + WALL_RESTITUTION);

      // Track wall collision for haptics - needs high velocity
      if (velIntoWall > 150.0f && totalSpeed > 200.0f) {
        _wallCollisionIntensity += velIntoWall;
        _wallCollisionCount++;
      }
    }
  }
}

#pragma mark -
#pragma mark Haptics

void F7samba::triggerCollisionHaptic(float timestep) {
  _hapticCooldown -= timestep;
  if (_hapticCooldown > 0.0f)
    return;

  // Wall collisions: sharp, scales with velocity and count
  if (_wallCollisionCount > 0) {
    float avgWallForce = _wallCollisionIntensity / (float)_wallCollisionCount;

    // High velocity (>400) = stronger, medium (200-400) = light
    float forceScale;
    if (avgWallForce > 500.0f) {
      forceScale = 0.6f + std::min((avgWallForce - 500.0f) / 500.0f, 0.4f);
    } else {
      forceScale = 0.2f + (avgWallForce - 150.0f) / 350.0f * 0.4f;
    }

    // More wall hits = more intense
    float countScale = std::min((float)_wallCollisionCount / 6.0f, 1.0f);

    float intensity = forceScale * (0.7f + countScale * 0.3f);
    intensity = std::min(intensity, 1.0f);

    // High sharpness for crisp wall impact
    Haptics::transient(intensity, 0.95f);
    _hapticCooldown = HAPTIC_COOLDOWN;
    return;
  }

  // Ball-ball collisions: very light, only when many collide at high speed
  if (_ballCollisionCount > 0) {
    float avgForce = _ballCollisionIntensity / (float)_ballCollisionCount;

    // Only noticeable if force is high
    if (avgForce > 300.0f) {
      float countScale = std::min((float)_ballCollisionCount / 10.0f, 1.0f);
      float intensity = 0.15f + countScale * 0.15f;
      intensity = std::min(intensity, 0.35f);

      Haptics::transient(intensity, 0.5f);
      _hapticCooldown = HAPTIC_COOLDOWN * 2.0f;
    }
  }
}

#pragma mark -
#pragma mark Lifecycle

void F7samba::update(float timestep) {
  FidgetableView::update(timestep);

  if (_isActive) {
    updatePhysics(timestep);
  }
}

void F7samba::setActive(bool active) {
  FidgetableView::setActive(active);

  // Update container appearance
  if (_containerNode != nullptr) {
    _containerNode->setColor(active ? CONTAINER_COLOR
                                    : CONTAINER_COLOR_INACTIVE);
  }

  // Update particle colors
  for (int i = 0; i < NUM_PARTICLES; i++) {
    if (_particleNodes[i] != nullptr) {
      Color4 color = active
                         ? PARTICLE_COLORS[i % PARTICLE_COLORS_COUNT]
                         : PARTICLE_COLORS_INACTIVE[i % PARTICLE_COLORS_COUNT];
      _particleNodes[i]->setColor(color);
    }
  }

  // Reset physics when becoming active
  if (active) {
    for (int i = 0; i < NUM_PARTICLES; i++) {
      _particles[i].velocity = Vec2::ZERO;
    }
    _hapticCooldown = 0.0f;
    _prevAcceleration = Vec3::ZERO;
  }
}

void F7samba::activateInputs() {
  // No touch inputs - uses accelerometer only
}

void F7samba::deactivateInputs() {
  // No touch inputs to deactivate
}
