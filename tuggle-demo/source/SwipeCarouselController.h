//
//  SwipeCarouselController.h
//  Tuggle
//
//  Controls the horizontal carousel of fidgetables.
//
//  - Manages a container node that holds all fidgetable pages
//  - Pages are arranged horizontally: |Page1|Page2|...|PageN|
//  - User drags to scroll through pages
//  - On release, snaps to nearest page with easing animation
//  - Only the centered page is "active" for interaction
//

#ifndef __SWIPE_CAROUSEL_CONTROLLER_H__
#define __SWIPE_CAROUSEL_CONTROLLER_H__

#include "FidgetableView.h"
#include "InputController.h"
#include <cugl/cugl.h>

// Forward declarations for all fidgetable types
#include "fidgetables/F1tancho.h"
#include "fidgetables/F2sampler.h"
#include "fidgetables/F3snakeyes.h"
#include "fidgetables/F4traffic.h"
#include "fidgetables/F5fellowship.h"
#include "fidgetables/F6katamari.h"
#include "fidgetables/F7samba.h"
#include "fidgetables/F8karting.h"

/** Number of fidgetables in the carousel */
#define NUM_FIDGETABLES 8

/** Snap animation duration in seconds */
#define SNAP_DURATION 0.3f

/** Minimum swipe velocity to trigger page change */
#define SWIPE_VELOCITY_THRESHOLD 500.0f

/**
 * Controller class for the horizontal carousel of fidgetables.
 *
 * This class manages:
 * - Layout of fidgetable pages in a horizontal strip
 * - Polling InputController for drag/swipe input
 * - Scroll position tracking and animation
 * - Snap-to-page behavior with easing
 * - Active page management (only centered page receives input)
 *
 * SCROLL POSITION:
 * scrollPos represents the X offset of the container.
 * scrollPos = 0 shows page 0 (first page)
 * scrollPos = pageWidth shows page 1
 * scrollPos = pageIndex * pageWidth shows page N
 */
class SwipeCarouselController {
protected:
  /** The scene this controller belongs to */
  std::shared_ptr<cugl::scene2::Scene2> _scene;

  /** Container node that holds all fidgetable pages (scrolls horizontally) */
  std::shared_ptr<cugl::scene2::SceneNode> _container;

  /** Array of all fidgetable views */
  std::vector<std::shared_ptr<FidgetableView>> _fidgetables;

  /** The width of each page (typically screen width) */
  float _pageWidth;

  /** The height of each page (typically screen height) */
  float _pageHeight;

  /** Current scroll position (0 = first page, pageWidth = second page, etc.) */
  float _scrollPos;

  /** Target scroll position for snap animation */
  float _targetScrollPos;

  /** Whether we're currently animating a snap */
  bool _isSnapping;

  /** Animation progress (0 to 1) */
  float _snapProgress;

  /** Starting scroll position for snap animation */
  float _snapStartPos;

  /** Currently active page index (0-7) */
  int _activePageIndex;

  // Drag tracking state
  /** Whether we're currently dragging the carousel */
  bool _isDragging;

  /** Scroll position when drag began */
  float _scrollStartPos;

  /** Position where drag started (for delta calculation) */
  cugl::Vec2 _dragStartPos;

  /** Scale factor from screen to scene coordinates */
  float _screenToSceneScale;

  /**
   * Creates all fidgetable pages and adds them to the container.
   */
  void buildFidgetables();

  /**
   * Updates the active page based on current scroll position.
   * Activates/deactivates fidgetables as needed.
   */
  void updateActivePage();

  /**
   * Calculates the nearest page index to snap to.
   *
   * @param velocity  The swipe velocity (used to bias direction)
   * @return The page index to snap to (0-7)
   */
  int calculateSnapTarget(float velocity);

  /**
   * Starts the snap animation to the target page.
   *
   * @param targetPage    The page index to snap to
   */
  void startSnapAnimation(int targetPage);

  /**
   * Easing function for snap animation.
   * Uses ease-out cubic for smooth deceleration.
   *
   * @param t Progress from 0 to 1
   * @return Eased value from 0 to 1
   */
  float easeOutCubic(float t);

  /**
   * Clamps scroll position to valid range.
   *
   * @param pos   The scroll position to clamp
   * @return Clamped scroll position
   */
  float clampScrollPos(float pos);

  /**
   * Converts screen coordinates to scene coordinates.
   *
   * @param screenPos The position in screen coordinates
   * @return The position in scene coordinates
   */
  cugl::Vec2 screenToScene(const cugl::Vec2 &screenPos) const;

public:
  /**
   * Creates an uninitialized SwipeCarouselController.
   */
  SwipeCarouselController();

  /**
   * Destroys the controller.
   */
  ~SwipeCarouselController();

  /**
   * Initializes the carousel controller.
   *
   * @param scene     The scene to add the carousel to
   * @param pageSize  The size of each page
   * @param screenToSceneScale  Scale factor for coordinate conversion
   * @return true if initialization succeeded
   */
  bool init(std::shared_ptr<cugl::scene2::Scene2> scene,
            const cugl::Size &pageSize, float screenToSceneScale = 1.0f);

  /**
   * Static allocator for SwipeCarouselController.
   */
  static std::shared_ptr<SwipeCarouselController>
  alloc(std::shared_ptr<cugl::scene2::Scene2> scene, const cugl::Size &pageSize,
        float screenToSceneScale = 1.0f);

  /**
   * Disposes of all resources.
   */
  void dispose();

  /**
   * Updates the carousel state.
   * Polls InputController for drag input and updates scroll position.
   * Call this every frame.
   *
   * @param timestep  Time since last frame in seconds
   */
  void update(float timestep);

  /**
   * Returns the container node for the carousel.
   *
   * @return The container node
   */
  std::shared_ptr<cugl::scene2::SceneNode> getContainer() const {
    return _container;
  }

  /**
   * Returns the currently active fidgetable.
   *
   * @return The active fidgetable view
   */
  std::shared_ptr<FidgetableView> getActiveFidgetable() const;

  /**
   * Returns the current active page index.
   *
   * @return Page index (0-7)
   */
  int getActivePageIndex() const { return _activePageIndex; }

  /**
   * Programmatically scrolls to a specific page.
   *
   * @param pageIndex The page to scroll to (0-7)
   * @param animated  Whether to animate the scroll
   */
  void scrollToPage(int pageIndex, bool animated = true);

  /**
   * Returns whether a drag is currently in progress.
   *
   * @return true if user is currently dragging
   */
  bool isDragging() const { return _isDragging; }

  /**
   * Returns whether a snap animation is in progress.
   *
   * @return true if snapping
   */
  bool isSnapping() const { return _isSnapping; }

  /**
   * Activates all fidgetable inputs in the carousel.
   * Call after adding to scene.
   */
  void activateInputs();

  /**
   * Deactivates all fidgetable inputs in the carousel.
   */
  void deactivateInputs();
};

#endif /* __SWIPE_CAROUSEL_CONTROLLER_H__ */
