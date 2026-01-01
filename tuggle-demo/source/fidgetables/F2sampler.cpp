//
//  F2sampler.cpp
//  Tuggle
//
//  Four buttons with different haptic styles.
//

#include "F2sampler.h"
#include "InputController.h"

using namespace cugl;
using namespace cugl::scene2;

// Button colors - each button has a distinct color for its haptic style
static const Color4 COLORS[] = {
    Color4(255, 107, 107, 255), // Coral red (RATTLE)
    Color4(78, 205, 196, 255),  // Teal (BUZZ)
    Color4(255, 230, 109, 255), // Yellow (TICK)
    Color4(170, 111, 255, 255), // Purple (THUMP)
    Color4(255, 165, 89, 255),  // Orange (TAP)
    Color4(89, 165, 255, 255),  // Blue (BUZZ_PULSE)
    Color4(165, 255, 165, 255)  // Mint green (SELECTION)
};

static const Color4 COLORS_PRESSED[] = {
    Color4(225, 77, 77, 255),   // RATTLE
    Color4(48, 175, 166, 255),  // BUZZ
    Color4(225, 200, 79, 255),  // TICK
    Color4(140, 81, 225, 255),  // THUMP
    Color4(225, 135, 59, 255),  // TAP
    Color4(59, 135, 225, 255),  // BUZZ_PULSE
    Color4(135, 225, 135, 255)  // SELECTION
};

static const Color4 COLORS_INACTIVE[] = {
    Color4(200, 150, 150, 255), // RATTLE
    Color4(150, 200, 195, 255), // BUZZ
    Color4(200, 195, 150, 255), // TICK
    Color4(180, 160, 200, 255), // THUMP
    Color4(200, 175, 160, 255), // TAP
    Color4(160, 175, 200, 255), // BUZZ_PULSE
    Color4(175, 200, 175, 255)  // SELECTION
};

F2sampler::F2sampler() : FidgetableView()
{
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    _buttonActive[i] = false;
  }
}

F2sampler::~F2sampler() { dispose(); }

bool F2sampler::init(int index, const Size &pageSize)
{
  return FidgetableView::init(index, pageSize);
}

std::shared_ptr<F2sampler> F2sampler::alloc(const Size &pageSize)
{
  auto result = std::make_shared<F2sampler>();
  if (result->init(2, pageSize))
  {
    return result;
  }
  return nullptr;
}

void F2sampler::dispose()
{
  for (auto &btn : _buttons)
  {
    btn.deactivate();
  }
  FidgetableView::dispose();
}

void F2sampler::buildContent()
{
  float radius = _pageSize.width * BUTTON_RADIUS_RATIO;
  float spacing = _pageSize.width * BUTTON_SPACING_RATIO;
  float cellSize = radius * 2 + spacing;

  // Calculate grid positions (3-2-2 layout centered)
  Vec2 center(_pageSize.width / 2, _pageSize.height / 2);

  // Row heights (top row higher, bottom row lower)
  float row1Y = center.y + cellSize * 1.1f;  // Top row (3 buttons)
  float row2Y = center.y;                     // Middle row (2 buttons)
  float row3Y = center.y - cellSize * 1.1f;  // Bottom row (2 buttons)

  Vec2 positions[] = {
      // Row 1: RATTLE, BUZZ, TICK (3 buttons)
      Vec2(center.x - cellSize, row1Y),
      Vec2(center.x, row1Y),
      Vec2(center.x + cellSize, row1Y),
      // Row 2: THUMP, TAP (2 buttons)
      Vec2(center.x - cellSize * 0.5f, row2Y),
      Vec2(center.x + cellSize * 0.5f, row2Y),
      // Row 3: BUZZ_PULSE, SELECTION (2 buttons)
      Vec2(center.x - cellSize * 0.5f, row3Y),
      Vec2(center.x + cellSize * 0.5f, row3Y)
  };

  HapticStyle styles[] = {
      HapticStyle::RATTLE,     // Coral red - velocity transients
      HapticStyle::BUZZ,       // Teal - soft continuous
      HapticStyle::TICK,       // Yellow - sharp ticks
      HapticStyle::THUMP,      // Purple - heavy thumps
      HapticStyle::TAP,        // Orange - tap() method
      HapticStyle::BUZZ_PULSE, // Blue - buzz() method
      HapticStyle::SELECTION   // Mint - selection() method
  };

  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    HapticButton::Config config;
    config.radius = radius;
    config.normalColor = COLORS[i];
    config.pressedColor = COLORS_PRESSED[i];
    config.inactiveColor = COLORS_INACTIVE[i];
    config.hapticStyle = styles[i];
    config.maxDragDistance = 10.0f;
    config.springK = 0.5f;
    config.sceneHeight = _pageSize.height;

    _buttons[i].init(positions[i], config);

    // Set up listener
    auto btnNode = _buttons[i].getNode();
    btnNode->setName("f2sampler_button_" + std::to_string(i));

    int btnIndex = i;
    btnNode->addListener([this, btnIndex](const std::string &name, bool down)
                         {
      if (_isActive) {
        if (down) {
          _buttonActive[btnIndex] = true;
          _isInteracting = true;
          _buttons[btnIndex].onPressed(
              InputController::getInstance()->getPosition());
        } else {
          _buttons[btnIndex].onReleased();
          _buttonActive[btnIndex] = false;
          // Check if any button is still active
          _isInteracting = false;
          for (int j = 0; j < NUM_BUTTONS; j++) {
            if (_buttonActive[j]) {
              _isInteracting = true;
              break;
            }
          }
        }
      } });

    _rootNode->addChild(btnNode);
  }
}

void F2sampler::update(float timestep)
{
  FidgetableView::update(timestep);

  Vec2 pointerPos = InputController::getInstance()->getPosition();

  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    // Update active buttons with drag, others just animate
    if (_buttonActive[i])
    {
      _buttons[i].update(timestep, pointerPos);
    }
    else
    {
      // Still need to animate scale for non-active buttons
      _buttons[i].update(timestep, Vec2::ZERO);
    }
  }
}

void F2sampler::setActive(bool active)
{
  FidgetableView::setActive(active);

  for (auto &btn : _buttons)
  {
    btn.setActive(active);
  }
}

void F2sampler::activateInputs()
{
  for (auto &btn : _buttons)
  {
    btn.activate();
  }
}

void F2sampler::deactivateInputs()
{
  for (auto &btn : _buttons)
  {
    btn.deactivate();
  }
}
