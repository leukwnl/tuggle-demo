//
//  F7samba.h
//  Tuggle
//
//  Seventh fidgetable toy - maracas simulation with particle physics.
//

#ifndef __F7SAMBA_H__
#define __F7SAMBA_H__

#include "FidgetableView.h"
#include <array>

// Number of particles in the maracas
#define NUM_PARTICLES 30

/**
 * Seventh fidgetable toy - Maracas Simulation
 *
 * Simulates the inside of a maraca with many small balls that
 * respond to gravity (accelerometer) and collide with each other
 * and the container walls. Shaking triggers collision haptics.
 */
class F7samba : public FidgetableView {
protected:
  /**
   * Represents a single particle (ball) in the maracas.
   */
  struct Particle {
    cugl::Vec2 position; // Position relative to container center
    cugl::Vec2 velocity; // Velocity in scene units per second
    float radius;        // Particle radius

    Particle() : radius(10.0f) {}
  };

  // Visual elements
  /** Container border (maraca shell) */
  std::shared_ptr<cugl::scene2::PolygonNode> _containerNode;

  /** Visual nodes for each particle */
  std::array<std::shared_ptr<cugl::scene2::PolygonNode>, NUM_PARTICLES>
      _particleNodes;

  // Particles
  /** All particles in the simulation */
  std::array<Particle, NUM_PARTICLES> _particles;

  // Container dimensions
  /** Container center position */
  cugl::Vec2 _containerCenter;

  /** Container radius */
  float _containerRadius;

  /** Particle radius (all same size) */
  float _particleRadius;

  /** Maximum distance particle center can be from container center */
  float _maxParticleDistance;

  // Haptic state
  /** Accumulated wall collision intensity this frame */
  float _wallCollisionIntensity;

  /** Number of wall collisions this frame */
  int _wallCollisionCount;

  /** Accumulated ball-ball collision intensity this frame */
  float _ballCollisionIntensity;

  /** Number of ball-ball collisions this frame */
  int _ballCollisionCount;

  /** Cooldown timer for haptic feedback */
  float _hapticCooldown;

  /** Previous frame's accelerometer reading for shake detection */
  cugl::Vec3 _prevAcceleration;

  /** Gravity scale - strong pull to bottom */
  static constexpr float GRAVITY_SCALE = 2000.0f;

  /** Shake detection threshold - very sensitive */
  static constexpr float SHAKE_THRESHOLD = 0.05f;

  /** Energy multiplier when shake is detected - very high for crossing
   * container */
  static constexpr float SHAKE_ENERGY = 80000.0f;

  /** Velocity decay per frame - moderate decay */
  static constexpr float VELOCITY_DECAY = 0.90f;

  /** Wall bounce coefficient - very low, almost no bounce */
  static constexpr float WALL_RESTITUTION = 0.8f;

  /** Ball-ball bounce coefficient - low bounce */
  static constexpr float BALL_RESTITUTION = 0.8f;

  /** Minimum collision velocity for haptic */
  static constexpr float MIN_COLLISION_VELOCITY = 40.0f;

  /** Minimum interval between haptic events */
  static constexpr float HAPTIC_COOLDOWN = 0.008f;

  /** Velocity threshold below which particles stop */
  static constexpr float SLEEP_VELOCITY = 10.0f;

  /**
   * Creates the visual content for this fidgetable.
   */
  void buildContent() override;

  /**
   * Updates particle physics.
   * @param timestep  Frame time in seconds
   */
  void updatePhysics(float timestep);

  /**
   * Resolves collision between two particles.
   * @param a  First particle index
   * @param b  Second particle index
   */
  void resolveParticleCollision(int a, int b);

  /**
   * Resolves collision between particle and container wall.
   * @param index  Particle index
   */
  void resolveWallCollision(int index);

  /**
   * Triggers haptic feedback based on accumulated collisions.
   * @param timestep  Frame time in seconds
   */
  void triggerCollisionHaptic(float timestep);

  /**
   * Creates a ring polygon for the container border.
   */
  std::shared_ptr<cugl::scene2::PolygonNode>
  createRing(float innerRadius, float outerRadius, const cugl::Color4 &color);

public:
  F7samba();
  virtual ~F7samba();

  bool init(int index, const cugl::Size &pageSize) override;
  static std::shared_ptr<F7samba> alloc(const cugl::Size &pageSize);
  void dispose() override;
  void update(float timestep) override;
  void setActive(bool active) override;
  void activateInputs() override;
  void deactivateInputs() override;
};

#endif /* __F7SAMBA_H__ */
