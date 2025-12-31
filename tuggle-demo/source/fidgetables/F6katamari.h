//
//  F6katamari.h
//  Tuggle
//
//  Sixth fidgetable toy - a circular arena with a rolling ball
//  controlled by the device accelerometer.
//

#ifndef __F6KATAMARI_H__
#define __F6KATAMARI_H__

#include "FidgetableView.h"

/**
 * Sixth fidgetable toy - Accelerometer-controlled ball in a circular arena.
 * 
 * The ball rolls around based on device tilt. Haptic feedback includes:
 * - Friction rumble while rolling
 * - Impact haptic on wall collision
 * - Tick haptics when rolling along the wall edge
 */
class F6katamari : public FidgetableView {
protected:
    // Visual elements
    /** The arena border (ring) */
    std::shared_ptr<cugl::scene2::PolygonNode> _arenaNode;
    
    /** The rolling ball */
    std::shared_ptr<cugl::scene2::PolygonNode> _ballNode;
    
    // Arena dimensions
    /** Arena center position */
    cugl::Vec2 _arenaCenter;
    
    /** Outer radius of the arena */
    float _arenaRadius;
    
    /** Radius of the ball */
    float _ballRadius;
    
    /** Effective radius the ball center can move within */
    float _maxBallDistance;
    
    // Ball physics state
    /** Current ball position relative to arena center */
    cugl::Vec2 _ballOffset;
    
    /** Ball velocity in scene units per second */
    cugl::Vec2 _ballVelocity;
    
    // Haptic state
    /** Cooldown timer for rolling haptics */
    float _rollingHapticTimer;
    
    /** Cooldown timer for wall tick haptics */
    float _wallTickTimer;
    
    /** Whether ball was touching wall in previous frame */
    bool _wasTouchingWall;
    
    /** Last wall contact angle for tick detection */
    float _lastWallAngle;
    
    // Physics constants
    /** Acceleration factor from tilt (how responsive to tilt) */
    static constexpr float TILT_ACCELERATION = 3500.0f;
    
    /** Friction coefficient (velocity decay per second) */
    static constexpr float FRICTION = 0.97f;
    
    /** Wall bounce damping (velocity retained after bounce) */
    static constexpr float BOUNCE_DAMPING = 0.65f;
    
    /** Minimum velocity to trigger rolling haptics */
    static constexpr float MIN_ROLL_VELOCITY = 30.0f;
    
    /** Maximum velocity for haptic scaling */
    static constexpr float MAX_ROLL_VELOCITY = 1000.0f;
    
    /** Base interval for rolling haptics (seconds) */
    static constexpr float ROLL_HAPTIC_INTERVAL = 0.04f;
    
    /** Minimum wall collision velocity for impact haptic */
    static constexpr float MIN_COLLISION_VELOCITY = 60.0f;
    
    /** Angle change required for wall tick (radians) */
    static constexpr float WALL_TICK_ANGLE = 0.15f;
    
    /** Minimum interval between wall ticks (seconds) */
    static constexpr float WALL_TICK_INTERVAL = 0.05f;
    
    /**
     * Creates the visual content for this fidgetable.
     */
    void buildContent() override;
    
    /**
     * Updates ball physics based on accelerometer input.
     * @param timestep  Frame time in seconds
     */
    void updatePhysics(float timestep);
    
    /**
     * Handles haptic feedback for rolling friction.
     * @param velocity  Current ball speed
     * @param timestep  Frame time in seconds
     */
    void triggerRollingHaptic(float velocity, float timestep);
    
    /**
     * Handles haptic feedback for wall collision.
     * @param collisionVelocity  Velocity component into wall
     */
    void triggerCollisionHaptic(float collisionVelocity);
    
    /**
     * Handles tick haptics while rolling along wall.
     * @param angle     Current angle on wall
     * @param timestep  Frame time in seconds
     */
    void triggerWallTickHaptic(float angle, float timestep);
    
    /**
     * Creates a ring polygon for the arena border.
     * @param innerRadius  Inner radius of the ring
     * @param outerRadius  Outer radius of the ring
     * @param color        Fill color
     * @return The polygon node for the ring
     */
    std::shared_ptr<cugl::scene2::PolygonNode> createRing(
        float innerRadius, float outerRadius, const cugl::Color4& color);
    
public:
    F6katamari();
    virtual ~F6katamari();
    
    bool init(int index, const cugl::Size& pageSize) override;
    static std::shared_ptr<F6katamari> alloc(const cugl::Size& pageSize);
    void dispose() override;
    void update(float timestep) override;
    void setActive(bool active) override;
    void activateInputs() override;
    void deactivateInputs() override;
};

#endif /* __F6KATAMARI_H__ */
