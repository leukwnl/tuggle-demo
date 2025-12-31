//
//  F6katamari.cpp
//  Tuggle
//
//  Implementation of F6katamari - a circular arena with accelerometer-controlled ball.
//

#include "F6katamari.h"
#include <cugl/cugl.h>
#include <cmath>

using namespace cugl;
using namespace cugl::scene2;

// Visual appearance
#define ARENA_RADIUS_RATIO 0.38f    // Arena radius as fraction of page width
#define BALL_RADIUS_RATIO 0.06f     // Ball radius as fraction of page width
#define ARENA_BORDER_WIDTH 8.0f     // Width of arena border ring

// Colors
#define ARENA_COLOR         Color4(80, 80, 100, 255)    // Dark blue-gray border
#define ARENA_COLOR_INACTIVE Color4(60, 60, 70, 255)    // Dimmed border
#define BALL_COLOR          Color4(255, 140, 60, 255)   // Orange ball
#define BALL_COLOR_INACTIVE Color4(180, 120, 80, 255)   // Dimmed ball

#pragma mark -
#pragma mark Constructors

F6katamari::F6katamari()
    : FidgetableView(),
      _arenaRadius(150.0f),
      _ballRadius(25.0f),
      _maxBallDistance(125.0f),
      _rollingHapticTimer(0.0f),
      _wallTickTimer(0.0f),
      _wasTouchingWall(false),
      _lastWallAngle(0.0f) {}

F6katamari::~F6katamari() {
    dispose();
}

bool F6katamari::init(int index, const cugl::Size& pageSize) {
    // Calculate dimensions based on page size
    _arenaRadius = pageSize.width * ARENA_RADIUS_RATIO;
    _ballRadius = pageSize.width * BALL_RADIUS_RATIO;
    _maxBallDistance = _arenaRadius - _ballRadius - (ARENA_BORDER_WIDTH / 2);
    
    return FidgetableView::init(index, pageSize);
}

std::shared_ptr<F6katamari> F6katamari::alloc(const cugl::Size& pageSize) {
    std::shared_ptr<F6katamari> result = std::make_shared<F6katamari>();
    if (result->init(6, pageSize)) {
        return result;
    }
    return nullptr;
}

void F6katamari::dispose() {
    _arenaNode = nullptr;
    _ballNode = nullptr;
    FidgetableView::dispose();
}

#pragma mark -
#pragma mark Content Building

std::shared_ptr<PolygonNode> F6katamari::createRing(
    float innerRadius, float outerRadius, const Color4& color) {
    
    // Create ring using two circles (outer - inner)
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
    
    // Build triangle strip for ring
    std::vector<Vec2> vertices;
    std::vector<Uint32> indices;
    
    // Combine vertices: outer first, then inner
    for (const auto& v : outerVerts) vertices.push_back(v);
    for (const auto& v : innerVerts) vertices.push_back(v);
    
    // Create indices for triangle strip between outer and inner rings
    for (int i = 0; i < segments; i++) {
        int next = (i + 1) % segments;
        // Outer triangle
        indices.push_back(i);                   // outer current
        indices.push_back(segments + i);        // inner current
        indices.push_back(next);                // outer next
        // Inner triangle
        indices.push_back(next);                // outer next
        indices.push_back(segments + i);        // inner current
        indices.push_back(segments + next);     // inner next
    }
    
    Poly2 poly(vertices, indices);
    auto node = PolygonNode::allocWithPoly(poly);
    node->setColor(color);
    return node;
}

void F6katamari::buildContent() {
    // Center of the arena
    _arenaCenter = Vec2(_pageSize.width / 2, _pageSize.height / 2);
    
    // Create arena border ring
    float outerRadius = _arenaRadius + ARENA_BORDER_WIDTH / 2;
    float innerRadius = _arenaRadius - ARENA_BORDER_WIDTH / 2;
    _arenaNode = createRing(innerRadius, outerRadius, ARENA_COLOR);
    _arenaNode->setAnchor(Vec2::ANCHOR_CENTER);
    _arenaNode->setPosition(_arenaCenter);
    _rootNode->addChild(_arenaNode);
    
    // Create the ball
    _ballNode = createCircle(_ballRadius, BALL_COLOR);
    _ballNode->setAnchor(Vec2::ANCHOR_CENTER);
    _ballNode->setPosition(_arenaCenter);
    _rootNode->addChild(_ballNode);
    
    // Initialize physics state
    _ballOffset = Vec2::ZERO;
    _ballVelocity = Vec2::ZERO;
}

#pragma mark -
#pragma mark Physics

void F6katamari::updatePhysics(float timestep) {
    // Get accelerometer input
    Accelerometer* accel = Input::get<Accelerometer>();
    if (accel == nullptr) {
        return; // Accelerometer not available
    }
    
    Vec3 acceleration = accel->getAcceleration();
    
    // Map accelerometer axes to screen movement
    // In portrait mode: x tilts left/right, y tilts forward/back
    // Note: accelerometer returns values roughly in range [-1, 1] representing g-force
    Vec2 tilt(acceleration.x, acceleration.y);
    
    // Apply acceleration to velocity
    _ballVelocity += tilt * TILT_ACCELERATION * timestep;
    
    // Apply friction
    _ballVelocity *= powf(FRICTION, timestep * 60.0f); // Normalize to 60fps
    
    // Update position
    _ballOffset += _ballVelocity * timestep;
    
    // Check for wall collision
    float dist = _ballOffset.length();
    bool touchingWall = false;
    
    if (dist > _maxBallDistance) {
        // Ball hit the wall
        touchingWall = true;
        
        // Calculate collision response
        Vec2 normal = _ballOffset.getNormalization();
        float velocityIntoWall = _ballVelocity.dot(normal);
        
        if (velocityIntoWall > 0) {
            // Ball moving into wall - bounce
            
            // Trigger collision haptic
            if (velocityIntoWall > MIN_COLLISION_VELOCITY) {
                triggerCollisionHaptic(velocityIntoWall);
            }
            
            // Reflect velocity with damping
            _ballVelocity -= normal * velocityIntoWall * (1.0f + BOUNCE_DAMPING);
        }
        
        // Clamp position to wall
        _ballOffset = normal * _maxBallDistance;
        
        // Wall tick haptics
        float currentAngle = atan2f(_ballOffset.y, _ballOffset.x);
        triggerWallTickHaptic(currentAngle, timestep);
    }
    
    // Track wall touching state
    if (!touchingWall && _wasTouchingWall) {
        // Just left the wall - reset tick tracking
        _lastWallAngle = 0.0f;
    }
    _wasTouchingWall = touchingWall;
    
    // Update ball visual position
    if (_ballNode != nullptr) {
        _ballNode->setPosition(_arenaCenter + _ballOffset);
    }
    
    // Trigger rolling haptics based on velocity
    float speed = _ballVelocity.length();
    triggerRollingHaptic(speed, timestep);
}

#pragma mark -
#pragma mark Haptics

void F6katamari::triggerRollingHaptic(float velocity, float timestep) {
    _rollingHapticTimer -= timestep;
    if (_rollingHapticTimer > 0.0f) return;
    
    // Only trigger if moving fast enough
    if (velocity < MIN_ROLL_VELOCITY) return;
    
    // Calculate intensity based on velocity (0.0 to 1.0)
    float normalizedVel = std::min(velocity, MAX_ROLL_VELOCITY) / MAX_ROLL_VELOCITY;
    float intensity = 0.3f + normalizedVel * 0.7f; // Range 0.5 to 1.0 (stronger)
    
    // Low sharpness for a rumble feel
    Haptics::transient(intensity, 0.10f);
    
    // Faster movement = shorter interval between haptics
    float intervalScale = 1.0f - (normalizedVel * 0.7f);
    _rollingHapticTimer = ROLL_HAPTIC_INTERVAL * intervalScale;
}

void F6katamari::triggerCollisionHaptic(float collisionVelocity) {
    // Scale intensity based on collision force
    float normalizedVel = std::min(collisionVelocity, MAX_ROLL_VELOCITY) / MAX_ROLL_VELOCITY;
    float intensity = 0.7f + normalizedVel * 0.3f; // Range 0.7 to 1.0 (stronger base)
    
    // High sharpness for impact feel
    Haptics::transient(intensity, 0.9f);
    
    // Also trigger heavy haptic for strong collisions
    if (normalizedVel > 0.5f) {
        Haptics::heavy();
    }
}

void F6katamari::triggerWallTickHaptic(float angle, float timestep) {
    _wallTickTimer -= timestep;
    
    // Initialize last angle if just started touching wall
    if (!_wasTouchingWall) {
        _lastWallAngle = angle;
        return;
    }
    
    // Check if we've moved enough along the wall for a tick
    float angleDiff = fabsf(angle - _lastWallAngle);
    // Handle wraparound
    if (angleDiff > M_PI) {
        angleDiff = 2.0f * M_PI - angleDiff;
    }
    
    if (angleDiff >= WALL_TICK_ANGLE && _wallTickTimer <= 0.0f) {
        // Tick! Ball is rolling along the wall
        Haptics::transient(0.6f, 0.9f); // Sharp tick
        
        _lastWallAngle = angle;
        _wallTickTimer = WALL_TICK_INTERVAL;
    }
}

#pragma mark -
#pragma mark Lifecycle

void F6katamari::update(float timestep) {
    FidgetableView::update(timestep);
    
    if (_isActive) {
        updatePhysics(timestep);
    }
}

void F6katamari::setActive(bool active) {
    FidgetableView::setActive(active);
    
    // Update visual appearance
    if (_arenaNode != nullptr) {
        _arenaNode->setColor(active ? ARENA_COLOR : ARENA_COLOR_INACTIVE);
    }
    if (_ballNode != nullptr) {
        _ballNode->setColor(active ? BALL_COLOR : BALL_COLOR_INACTIVE);
    }
    
    // Reset physics when becoming active
    if (active) {
        _ballVelocity = Vec2::ZERO;
        _rollingHapticTimer = 0.0f;
        _wallTickTimer = 0.0f;
        _wasTouchingWall = false;
    }
}

void F6katamari::activateInputs() {
    // No touch inputs needed - uses accelerometer
}

void F6katamari::deactivateInputs() {
    // No touch inputs to deactivate
}
