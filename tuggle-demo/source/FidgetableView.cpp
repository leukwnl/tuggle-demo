//
//  FidgetableView.cpp
//  Tuggle
//
//  Implementation of the abstract FidgetableView base class.
//

#include "FidgetableView.h"

using namespace cugl;
using namespace cugl::graphics;
using namespace cugl::scene2;

#pragma mark -
#pragma mark Constructors

FidgetableView::FidgetableView() : _index(0), _isActive(false), _isInteracting(false) {}

FidgetableView::~FidgetableView() { dispose(); }

bool FidgetableView::init(int index, const Size &pageSize) {
  _index = index;
  _pageSize = pageSize;
  _isActive = false;
  _name = "Fidgetable " + std::to_string(index);

  // Create the root node for this fidgetable
  _rootNode = SceneNode::alloc();
  _rootNode->setContentSize(pageSize);
  _rootNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);

  // Let the subclass build its specific content
  buildContent();

  return true;
}

void FidgetableView::dispose() {
  // Clear root node - subclasses should clean up their own members
  _rootNode = nullptr;
}

#pragma mark -
#pragma mark Utility Methods

std::shared_ptr<PolygonNode>
FidgetableView::createCircle(float radius, Color4 color, int segments) {
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
    indices.push_back(0);     // Center
    indices.push_back(i);     // Current vertex
    indices.push_back(i + 1); // Next vertex (wraps around)
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
#pragma mark Update

void FidgetableView::update(float timestep) {
  // Base implementation does nothing
  // Subclasses override for custom animation
}

void FidgetableView::setActive(bool active) {
  _isActive = active;
  // Subclasses should override to update visuals
}
