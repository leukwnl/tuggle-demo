//
//  SwipeCarouselController.cpp
//  Tuggle
//
//  Implementation of the horizontal carousel controller.
//  Uses InputController for unified pointer input.
//

#include "SwipeCarouselController.h"

using namespace cugl;
using namespace cugl::graphics;
using namespace cugl::scene2;

#pragma mark -
#pragma mark Constructors

SwipeCarouselController::SwipeCarouselController() :
    _pageWidth(0),
    _pageHeight(0),
    _scrollPos(0),
    _targetScrollPos(0),
    _isSnapping(false),
    _snapProgress(0),
    _snapStartPos(0),
    _activePageIndex(0),
    _isDragging(false),
    _scrollStartPos(0),
    _screenToSceneScale(1.0f) {
}

SwipeCarouselController::~SwipeCarouselController() {
    dispose();
}

bool SwipeCarouselController::init(std::shared_ptr<Scene2> scene,
                                    const Size& pageSize,
                                    std::shared_ptr<Font> font,
                                    float screenToSceneScale) {
    _scene = scene;
    _pageWidth = pageSize.width;
    _pageHeight = pageSize.height;
    _font = font;
    _screenToSceneScale = screenToSceneScale;
    
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
    
    // Set first page as active
    _activePageIndex = 0;
    _scrollPos = 0;
    _targetScrollPos = 0;
    updateActivePage();
    
    return true;
}

std::shared_ptr<SwipeCarouselController> SwipeCarouselController::alloc(
    std::shared_ptr<Scene2> scene,
    const Size& pageSize,
    std::shared_ptr<Font> font,
    float screenToSceneScale) {
    
    std::shared_ptr<SwipeCarouselController> result = 
        std::make_shared<SwipeCarouselController>();
    if (result->init(scene, pageSize, font, screenToSceneScale)) {
        return result;
    }
    return nullptr;
}

void SwipeCarouselController::dispose() {
    // Deactivate all buttons
    for (auto& fidgetable : _fidgetables) {
        if (fidgetable != nullptr) {
            fidgetable->deactivateButton();
            fidgetable->dispose();
        }
    }
    _fidgetables.clear();
    
    if (_container != nullptr) {
        _container->removeFromParent();
        _container = nullptr;
    }
    
    _scene = nullptr;
    _font = nullptr;
}

#pragma mark -
#pragma mark Building Content

void SwipeCarouselController::buildFidgetables() {
    Size pageSize(_pageWidth, _pageHeight);
    
    // Create each fidgetable as its own class type
    // This allows for future customization of each toy
    
    // Fidgetable 1
    auto f1 = Fidgetable1::alloc(pageSize, _font);
    f1->getNode()->setPosition(Vec2(0 * _pageWidth, 0));
    _container->addChild(f1->getNode());
    _fidgetables.push_back(f1);
    
    // Fidgetable 2
    auto f2 = Fidgetable2::alloc(pageSize, _font);
    f2->getNode()->setPosition(Vec2(1 * _pageWidth, 0));
    _container->addChild(f2->getNode());
    _fidgetables.push_back(f2);
    
    // Fidgetable 3
    auto f3 = Fidgetable3::alloc(pageSize, _font);
    f3->getNode()->setPosition(Vec2(2 * _pageWidth, 0));
    _container->addChild(f3->getNode());
    _fidgetables.push_back(f3);
    
    // Fidgetable 4
    auto f4 = Fidgetable4::alloc(pageSize, _font);
    f4->getNode()->setPosition(Vec2(3 * _pageWidth, 0));
    _container->addChild(f4->getNode());
    _fidgetables.push_back(f4);
    
    // Fidgetable 5
    auto f5 = Fidgetable5::alloc(pageSize, _font);
    f5->getNode()->setPosition(Vec2(4 * _pageWidth, 0));
    _container->addChild(f5->getNode());
    _fidgetables.push_back(f5);
    
    // Fidgetable 6
    auto f6 = Fidgetable6::alloc(pageSize, _font);
    f6->getNode()->setPosition(Vec2(5 * _pageWidth, 0));
    _container->addChild(f6->getNode());
    _fidgetables.push_back(f6);
    
    // Fidgetable 7
    auto f7 = Fidgetable7::alloc(pageSize, _font);
    f7->getNode()->setPosition(Vec2(6 * _pageWidth, 0));
    _container->addChild(f7->getNode());
    _fidgetables.push_back(f7);
    
    // Fidgetable 8
    auto f8 = Fidgetable8::alloc(pageSize, _font);
    f8->getNode()->setPosition(Vec2(7 * _pageWidth, 0));
    _container->addChild(f8->getNode());
    _fidgetables.push_back(f8);
}

#pragma mark -
#pragma mark Coordinate Conversion

Vec2 SwipeCarouselController::screenToScene(const Vec2& screenPos) const {
    // Convert screen coordinates to scene coordinates
    float sceneX = screenPos.x * _screenToSceneScale;
    float sceneY = screenPos.y * _screenToSceneScale;
    return Vec2(sceneX, sceneY);
}

#pragma mark -
#pragma mark Update

void SwipeCarouselController::update(float timestep) {
    // Get the input controller
    InputController* input = InputController::getInstance();
    
    // Handle drag start
    if (input->didDragStart() && !_isDragging) {
        _isDragging = true;
        _isSnapping = false;  // Cancel any ongoing snap animation
        _scrollStartPos = _scrollPos;
        _dragStartPos = screenToScene(input->getStartPosition());
    }
    
    // Handle ongoing drag
    if (_isDragging && input->isDragging()) {
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
            _scrollPos = _snapStartPos + (_targetScrollPos - _snapStartPos) * easedProgress;
        }
        
        // Update container position
        _container->setPositionX(-_scrollPos);
        updateActivePage();
    }
    
    // Update all fidgetables
    for (auto& fidgetable : _fidgetables) {
        fidgetable->update(timestep);
    }
}

void SwipeCarouselController::updateActivePage() {
    // Calculate which page is currently centered
    int newActiveIndex = (int)roundf(_scrollPos / _pageWidth);
    newActiveIndex = std::max(0, std::min(newActiveIndex, NUM_FIDGETABLES - 1));
    
    // Update active state if changed
    if (newActiveIndex != _activePageIndex || _activePageIndex == 0) {
        _activePageIndex = newActiveIndex;
        
        // Update all fidgetables' active state
        for (int i = 0; i < (int)_fidgetables.size(); i++) {
            _fidgetables[i]->setActive(i == _activePageIndex);
        }
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
    // Ease out cubic: f(t) = 1 - (1-t)^3
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

std::shared_ptr<FidgetableView> SwipeCarouselController::getActiveFidgetable() const {
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
    for (auto& fidgetable : _fidgetables) {
        fidgetable->activateButton();
    }
}

void SwipeCarouselController::deactivateInputs() {
    for (auto& fidgetable : _fidgetables) {
        fidgetable->deactivateButton();
    }
}
