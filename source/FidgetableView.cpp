//
//  FidgetableView.cpp
//  Tuggle
//
//  Implementation of the base FidgetableView class.
//

#include "FidgetableView.h"

using namespace cugl;
using namespace cugl::graphics;
using namespace cugl::scene2;

// Default circle radius as fraction of page width
#define CIRCLE_RADIUS_RATIO 0.25f
// Circle button colors
#define CIRCLE_COLOR_NORMAL     Color4(100, 149, 237, 255)  // Cornflower blue
#define CIRCLE_COLOR_PRESSED    Color4(70, 119, 207, 255)   // Darker blue
#define LABEL_COLOR             Color4(50, 50, 50, 255)     // Dark gray

#pragma mark -
#pragma mark Constructors

FidgetableView::FidgetableView() :
    _index(0),
    _isActive(false),
    _circleRadius(100.0f) {
}

FidgetableView::~FidgetableView() {
    dispose();
}

bool FidgetableView::init(int index, const Size& pageSize, 
                          std::shared_ptr<Font> font) {
    _index = index;
    _pageSize = pageSize;
    _font = font;
    _name = "Fidgetable " + std::to_string(index);
    _circleRadius = pageSize.width * CIRCLE_RADIUS_RATIO;
    _isActive = false;
    
    // Create the root node for this fidgetable
    _rootNode = SceneNode::alloc();
    _rootNode->setContentSize(pageSize);
    _rootNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    
    // Build the visual content
    buildContent();
    
    return true;
}

std::shared_ptr<FidgetableView> FidgetableView::alloc(int index, 
                                                       const Size& pageSize,
                                                       std::shared_ptr<Font> font) {
    std::shared_ptr<FidgetableView> result = std::make_shared<FidgetableView>();
    if (result->init(index, pageSize, font)) {
        return result;
    }
    return nullptr;
}

void FidgetableView::dispose() {
    if (_circleButton != nullptr) {
        _circleButton->deactivate();
        _circleButton->clearListeners();
        _circleButton = nullptr;
    }
    _circleNode = nullptr;
    _label = nullptr;
    _rootNode = nullptr;
    _font = nullptr;
}

#pragma mark -
#pragma mark Visual Construction

void FidgetableView::buildContent() {
    // Calculate center position for the circle
    Vec2 centerPos(_pageSize.width / 2, _pageSize.height / 2 + 50);
    
    // Create the circle visuals for normal and pressed states
    auto circleNormal = createCircle(_circleRadius, CIRCLE_COLOR_NORMAL);
    auto circlePressed = createCircle(_circleRadius, CIRCLE_COLOR_PRESSED);
    
    // Store reference to the normal circle node
    _circleNode = circleNormal;
    
    // Create the button with up/down states
    _circleButton = Button::alloc(circleNormal, circlePressed);
    _circleButton->setAnchor(Vec2::ANCHOR_CENTER);
    _circleButton->setPosition(centerPos);
    _circleButton->setName("fidgetable_" + std::to_string(_index));
    
    // Set up the button listener
    // Capture 'this' and _index by value for the lambda
    int idx = _index;
    _circleButton->addListener([this, idx](const std::string& name, bool down) {
        if (!down && _isActive) {
            // Only respond when active and on button release
            onPressed();
        }
    });
    
    // Add button to root node
    _rootNode->addChild(_circleButton);
    
    // Create the label below the circle
    if (_font != nullptr) {
        _label = Label::allocWithText(_name, _font);
        _label->setAnchor(Vec2::ANCHOR_CENTER);
        _label->setForeground(LABEL_COLOR);
        
        // Position label below the circle with some padding
        float labelY = centerPos.y - _circleRadius - 60;
        _label->setPosition(Vec2(_pageSize.width / 2, labelY));
        
        _rootNode->addChild(_label);
    }
}

std::shared_ptr<PolygonNode> FidgetableView::createCircle(float radius, 
                                                           Color4 color, 
                                                           int segments) {
    // Create a filled circle using a polygon
    std::vector<Vec2> vertices;
    vertices.reserve(segments + 1);
    
    // Center vertex
    vertices.push_back(Vec2::ZERO);
    
    // Circle vertices
    for (int i = 0; i <= segments; i++) {
        float angle = (float)i / (float)segments * 2.0f * M_PI;
        float x = radius * cosf(angle);
        float y = radius * sinf(angle);
        vertices.push_back(Vec2(x, y));
    }
    
    // Create triangle fan indices
    std::vector<Uint32> indices;
    indices.reserve(segments * 3);
    for (int i = 1; i <= segments; i++) {
        indices.push_back(0);           // Center
        indices.push_back(i);           // Current vertex
        indices.push_back(i + 1);       // Next vertex (wraps around)
    }
    
    // Create the polygon
    Poly2 poly(vertices, indices);
    
    // Create and configure the node
    auto node = PolygonNode::allocWithPoly(poly);
    node->setColor(color);
    node->setAnchor(Vec2::ANCHOR_CENTER);
    
    return node;
}

#pragma mark -
#pragma mark Update and Input

void FidgetableView::update(float timestep) {
    // Base implementation does nothing
    // Override in subclasses for custom animation
    
    // EXPANSION: Add animation here, e.g.:
    // - Idle animations when not interacting
    // - Response animations after press
    // - State-based visual changes
}

void FidgetableView::onPressed() {
    // Print to console as specified
    CULog("Fidgetable %d pressed", _index);
    
    // EXPANSION: Override this method to add custom behavior:
    // - Play sounds
    // - Trigger animations
    // - Update game state
    // - Show visual feedback
}

void FidgetableView::setActive(bool active) {
    _isActive = active;
    
    // Visual feedback for active state could be added here
    // EXPANSION: Change appearance when active/inactive
    // e.g., dim inactive toys, highlight active toy
    
    if (_circleNode != nullptr) {
        // Slightly dim inactive fidgetables
        if (active) {
            _circleNode->setColor(CIRCLE_COLOR_NORMAL);
        } else {
            // Dimmed color for inactive state
            _circleNode->setColor(Color4(150, 180, 220, 255));
        }
    }
}

void FidgetableView::activateButton() {
    if (_circleButton != nullptr) {
        _circleButton->activate();
    }
}

void FidgetableView::deactivateButton() {
    if (_circleButton != nullptr) {
        _circleButton->deactivate();
    }
}

