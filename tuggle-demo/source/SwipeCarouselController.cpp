//
//  SwipeCarouselController.cpp
//  Tuggle
//
//  Implementation of the horizontal carousel controller.
//

#include "SwipeCarouselController.h"

using namespace cugl;
using namespace cugl::graphics;
using namespace cugl::scene2;

#pragma mark -
#pragma mark Constructors

SwipeCarouselController::SwipeCarouselController()
    : _pageWidth(0), _pageHeight(0), _scrollPos(0), _targetScrollPos(0),
      _isSnapping(false), _snapProgress(0), _snapStartPos(0),
      _activePageIndex(0), _isDragging(false), _scrollStartPos(0),
      _screenToSceneScale(1.0f) {}

SwipeCarouselController::~SwipeCarouselController() { dispose(); }

bool SwipeCarouselController::init(std::shared_ptr<Scene2> scene,
                                   const Size &pageSize,
                                   float screenToSceneScale,
                                   std::shared_ptr<AssetManager> assets) {
  _scene = scene;
  _pageWidth = pageSize.width;
  _pageHeight = pageSize.height;
  _screenToSceneScale = screenToSceneScale;
  _assets = assets;

  // Create the container node that will hold all pages
  // This container scrolls horizontally
  _container = SceneNode::alloc();
  _container->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
  _container->setPosition(Vec2::ZERO);

  // Set container size to hold all pages horizontally
  float totalWidth = _pageWidth * NUM_FIDGETABLES;
  _container->setContentSize(Size(totalWidth, _pageHeight));

  // Build all fidgetable pages
  buildFidgetables();

  // Add container to scene
  _scene->addChild(_container);

  // Build pagination dots
  buildPaginationDots();

  // Set first page as active
  _activePageIndex = 0;
  _scrollPos = 0;
  _targetScrollPos = 0;
  updateActivePage();

  return true;
}

std::shared_ptr<SwipeCarouselController>
SwipeCarouselController::alloc(std::shared_ptr<Scene2> scene,
                               const Size &pageSize, float screenToSceneScale,
                               std::shared_ptr<AssetManager> assets) {

  std::shared_ptr<SwipeCarouselController> result =
      std::make_shared<SwipeCarouselController>();
  if (result->init(scene, pageSize, screenToSceneScale, assets)) {
    return result;
  }
  return nullptr;
}

void SwipeCarouselController::dispose() {
  // Deactivate all inputs
  for (auto &fidgetable : _fidgetables) {
    if (fidgetable != nullptr) {
      fidgetable->deactivateInputs();
      fidgetable->dispose();
    }
  }
  _fidgetables.clear();

  if (_paginationContainer != nullptr) {
    _paginationContainer->removeFromParent();
    _paginationContainer = nullptr;
  }
  _paginationDots.clear();

  if (_container != nullptr) {
    _container->removeFromParent();
    _container = nullptr;
  }

  _scene = nullptr;
}

#pragma mark -
#pragma mark Building Content

void SwipeCarouselController::buildFidgetables() {
  Size pageSize(_pageWidth, _pageHeight);

  // Fidgetable 1
  auto f1 = F1tancho::alloc(pageSize);
  f1->getNode()->setPosition(Vec2(0 * _pageWidth, 0));
  _container->addChild(f1->getNode());
  _fidgetables.push_back(f1);

  // Fidgetable 2
  auto f2 = F2sampler::alloc(pageSize);
  f2->getNode()->setPosition(Vec2(1 * _pageWidth, 0));
  _container->addChild(f2->getNode());
  _fidgetables.push_back(f2);

  // Fidgetable 3
  auto f3 = F3vibrations::alloc(pageSize);
  f3->getNode()->setPosition(Vec2(2 * _pageWidth, 0));
  _container->addChild(f3->getNode());
  _fidgetables.push_back(f3);

  // Fidgetable 4
  auto f4 = F4bubbles::alloc(pageSize);
  f4->getNode()->setPosition(Vec2(3 * _pageWidth, 0));
  _container->addChild(f4->getNode());
  _fidgetables.push_back(f4);

  // Fidgetable 5
  auto f5 = F5fellowship::alloc(pageSize);
  f5->getNode()->setPosition(Vec2(4 * _pageWidth, 0));
  _container->addChild(f5->getNode());
  _fidgetables.push_back(f5);

  // Fidgetable 6
  auto f6 = F6katamari::alloc(pageSize);
  f6->getNode()->setPosition(Vec2(5 * _pageWidth, 0));
  _container->addChild(f6->getNode());
  _fidgetables.push_back(f6);

  // Fidgetable 7
  auto f7 = F7fluid::alloc(pageSize);
  f7->getNode()->setPosition(Vec2(6 * _pageWidth, 0));
  _container->addChild(f7->getNode());
  _fidgetables.push_back(f7);

  // Fidgetable 8
  auto f8 = F8uppity::alloc(pageSize);
  f8->getNode()->setPosition(Vec2(7 * _pageWidth, 0));
  _container->addChild(f8->getNode());
  _fidgetables.push_back(f8);

  // Fidgetable 9 - Soundboard
  auto f9 = F9soundboard::alloc(pageSize);
  f9->getNode()->setPosition(Vec2(8 * _pageWidth, 0));
  _container->addChild(f9->getNode());
  _fidgetables.push_back(f9);

  // Configure F9soundboard with sounds from asset manager
  if (_assets != nullptr) {
    for (int i = 0; i < 9; i++) {
      std::string soundKey = "f9_sound_" + std::to_string(i);
      auto sound = _assets->get<audio::Sound>(soundKey);
      if (sound != nullptr) {
        f9->setSound(i, sound);
      }
    }
//    // Configure haptic files (all use the same fanfare.ahap for now)
//    for (int i = 0; i < 9; i++) {
//      f9->setHapticFile(i, "fanfare.ahap");
//    }
  }
    f9->setHapticFile(0, "fanfare.ahap");
    f9->setHapticFile(1, "gamecube.ahap");
    f9->setHapticFile(2, "counter.ahap");
    f9->setHapticFile(3, "eating.ahap");
    f9->setHapticFile(4, "kricketune.ahap");
    f9->setHapticFile(5, "lalilulelo.ahap");
    f9->setHapticFile(6, "oof.ahap");
    f9->setHapticFile(7, "trick.ahap");
    f9->setHapticFile(8, "sans.ahap");

  // Fidgetable 10 - verstappen (HapticPlayer demo)
  auto f10 = F10verstappen::alloc(pageSize);
  f10->getNode()->setPosition(Vec2(9 * _pageWidth, 0));
  _container->addChild(f10->getNode());
  _fidgetables.push_back(f10);
    _fidgetables[0]->setActive(true);
}

void SwipeCarouselController::buildPaginationDots() {
  // Create a container for the pagination dots
  _paginationContainer = SceneNode::alloc();
  _paginationContainer->setAnchor(Vec2::ANCHOR_BOTTOM_CENTER);
  
  // Position at the bottom center of the page
  float bottomMargin = 60.0f; // Distance from bottom of screen
  _paginationContainer->setPosition(Vec2(_pageWidth / 2, bottomMargin));
  
  // Add to scene (not to the scrolling container)
  _scene->addChild(_paginationContainer);
  
  // Dot parameters
  float dotRadius = 6.0f;
  float dotSpacing = 20.0f;
  float totalWidth = (NUM_FIDGETABLES - 1) * dotSpacing;
  int segments = 20; // Number of segments for circle smoothness
  
  // Create dots
  for (int i = 0; i < NUM_FIDGETABLES; i++) {
    // Create a filled circle using a polygon
    std::vector<Vec2> vertices;
    vertices.reserve(segments + 1);

    // Center vertex
    vertices.push_back(Vec2::ZERO);

    // Circle vertices
    for (int j = 0; j <= segments; j++) {
      float angle = (float)j / (float)segments * 2.0f * M_PI;
      float x = dotRadius * cosf(angle);
      float y = dotRadius * sinf(angle);
      vertices.push_back(Vec2(x, y));
    }

    // Create triangle fan indices
    std::vector<Uint32> indices;
    indices.reserve(segments * 3);
    for (int j = 1; j <= segments; j++) {
      indices.push_back(0);     // Center
      indices.push_back(j);     // Current vertex
      indices.push_back(j + 1); // Next vertex (wraps around)
    }

    // Create the polygon
    Poly2 poly(vertices, indices);

    // Create and configure the node
    auto dot = scene2::PolygonNode::allocWithPoly(poly);
    dot->setAnchor(Vec2::ANCHOR_CENTER);
    
    // Position dots horizontally centered
    float xPos = -totalWidth / 2 + i * dotSpacing;
    dot->setPosition(Vec2(xPos, 0));
    
    // Set initial color (inactive: semi-transparent white)
    dot->setColor(Color4(255, 255, 255, 100));
    
    _paginationContainer->addChild(dot);
    _paginationDots.push_back(dot);
  }
  
  // Update to highlight the first dot
  updatePaginationDots();
}

void SwipeCarouselController::updatePaginationDots() {
  for (int i = 0; i < (int)_paginationDots.size(); i++) {
    if (i == _activePageIndex) {
      // Active dot: fully opaque white
      _paginationDots[i]->setColor(Color4(255, 255, 255, 255));
    } else {
      // Inactive dots: semi-transparent white
      _paginationDots[i]->setColor(Color4(255, 255, 255, 100));
    }
  }
}

#pragma mark -
#pragma mark Coordinate Conversion

Vec2 SwipeCarouselController::screenToScene(const Vec2 &screenPos) const {
  // Convert screen coordinates to scene coordinates
  float sceneX = screenPos.x * _screenToSceneScale;
  float sceneY = screenPos.y * _screenToSceneScale;
  return Vec2(sceneX, sceneY);
}

#pragma mark -
#pragma mark Update

void SwipeCarouselController::update(float timestep) {
  // Get the input controller
  InputController *input = InputController::getInstance();

  // Check if active fidgetable is being interacted with
  bool fidgetableInteracting = false;
  auto activeFidgetable = getActiveFidgetable();
  if (activeFidgetable != nullptr) {
    fidgetableInteracting = activeFidgetable->isInteracting();
  }

  // Handle drag start (only if fidgetable is not being interacted with)
  if (input->didDragStart() && !_isDragging && !fidgetableInteracting) {
    _isDragging = true;
    _isSnapping = false; // Cancel any ongoing snap animation
    _scrollStartPos = _scrollPos;
    _dragStartPos = screenToScene(input->getStartPosition());
  }

  // Handle ongoing drag (skip if fidgetable took over interaction)
  if (_isDragging && input->isDragging() && !fidgetableInteracting) {
    Vec2 currentPos = screenToScene(input->getPosition());

    // Calculate drag delta from start position
    float deltaX = _dragStartPos.x - currentPos.x;

    // Update scroll position
    float newScrollPos = _scrollStartPos + deltaX;
    _scrollPos = clampScrollPos(newScrollPos);

    // Update container position
    _container->setPositionX(-_scrollPos);

    // Update active page during drag
    updateActivePage();
  }

  // Cancel carousel drag if fidgetable took over interaction
  if (_isDragging && fidgetableInteracting) {
    _isDragging = false;
    // Snap back to nearest page since we were interrupted
    int targetPage = calculateSnapTarget(0);
    startSnapAnimation(targetPage);
  }

  // Handle drag end
  if (input->didDragEnd() && _isDragging) {
    _isDragging = false;

    // Get swipe velocity from InputController (already in screen coords)
    Vec2 swipeVelocity = input->getSwipeVelocity();
    // Scale velocity to scene coordinates
    float velocityX = -swipeVelocity.x * _screenToSceneScale;

    // Calculate target page based on velocity
    int targetPage = calculateSnapTarget(velocityX);

    // Start snap animation
    startSnapAnimation(targetPage);
  }

  // Update snap animation if active (and not dragging)
  if (_isSnapping && !_isDragging) {
    _snapProgress += timestep / SNAP_DURATION;

    if (_snapProgress >= 1.0f) {
      // Animation complete
      _snapProgress = 1.0f;
      _isSnapping = false;
      _scrollPos = _targetScrollPos;
    } else {
      // Interpolate with easing
      float easedProgress = easeOutCubic(_snapProgress);
      _scrollPos =
          _snapStartPos + (_targetScrollPos - _snapStartPos) * easedProgress;
    }

    // Update container position
    _container->setPositionX(-_scrollPos);
    updateActivePage();
  }

  // Update all fidgetables
  for (auto &fidgetable : _fidgetables) {
    fidgetable->update(timestep);
  }
}

void SwipeCarouselController::updateActivePage() {
  // Calculate which page is currently centered
  int newActiveIndex = (int)roundf(_scrollPos / _pageWidth);
  newActiveIndex = std::max(0, std::min(newActiveIndex, NUM_FIDGETABLES - 1));

  // Update active state if changed
  if (newActiveIndex != _activePageIndex) {
    _activePageIndex = newActiveIndex;

    // Update all fidgetables' active state
    for (int i = 0; i < (int)_fidgetables.size(); i++) {
      _fidgetables[i]->setActive(i == _activePageIndex);
    }
    
    // Update pagination dots to reflect new active page
    updatePaginationDots();
    // Play a medium haptic
    Haptics::medium();
  }
}

#pragma mark -
#pragma mark Snap Animation

int SwipeCarouselController::calculateSnapTarget(float velocity) {
  // Current fractional page
  float currentPage = _scrollPos / _pageWidth;

  // If velocity is high enough, bias toward that direction
  if (fabsf(velocity) > SWIPE_VELOCITY_THRESHOLD) {
    if (velocity > 0) {
      // Swiping left (next page)
      currentPage = ceilf(currentPage);
    } else {
      // Swiping right (previous page)
      currentPage = floorf(currentPage);
    }
  } else {
    // No significant velocity - snap to nearest
    currentPage = roundf(currentPage);
  }

  // Clamp to valid range
  int targetPage = (int)currentPage;
  targetPage = std::max(0, std::min(targetPage, NUM_FIDGETABLES - 1));

  return targetPage;
}

void SwipeCarouselController::startSnapAnimation(int targetPage) {
  _targetScrollPos = targetPage * _pageWidth;
  _snapStartPos = _scrollPos;
  _snapProgress = 0;
  _isSnapping = true;
}

float SwipeCarouselController::easeOutCubic(float t) {
  float inv = 1.0f - t;
  return 1.0f - (inv * inv * inv);
}

float SwipeCarouselController::clampScrollPos(float pos) {
  // Clamp to valid scroll range
  float minScroll = 0;
  float maxScroll = (_pageWidth * (NUM_FIDGETABLES - 1));

  return std::max(minScroll, std::min(pos, maxScroll));
}

#pragma mark -
#pragma mark Public Interface

std::shared_ptr<FidgetableView>
SwipeCarouselController::getActiveFidgetable() const {
  if (_activePageIndex >= 0 && _activePageIndex < (int)_fidgetables.size()) {
    return _fidgetables[_activePageIndex];
  }
  return nullptr;
}

void SwipeCarouselController::scrollToPage(int pageIndex, bool animated) {
  // Clamp to valid range
  pageIndex = std::max(0, std::min(pageIndex, NUM_FIDGETABLES - 1));

  if (animated) {
    startSnapAnimation(pageIndex);
  } else {
    _scrollPos = pageIndex * _pageWidth;
    _targetScrollPos = _scrollPos;
    _isSnapping = false;
    _container->setPositionX(-_scrollPos);
    updateActivePage();
  }
}

void SwipeCarouselController::activateInputs() {
  for (auto &fidgetable : _fidgetables) {
    fidgetable->activateInputs();
  }
}

void SwipeCarouselController::deactivateInputs() {
  for (auto &fidgetable : _fidgetables) {
    fidgetable->deactivateInputs();
  }
}
