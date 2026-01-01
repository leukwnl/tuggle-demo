//
//  F3snakeyes.cpp
//  Tuggle
//
//  Six toggle buttons with continuous haptics.
//  3 rows x 2 columns:
//    Left = rumble (low sharpness), Right = tick (high sharpness)
//    Top to bottom = slow, medium, fast interval speeds
//

#include "F3snakeyes.h"

using namespace cugl;
using namespace cugl::scene2;

// Button colors - Left column (warm/rumble), Right column (cool/tick)
// Layout: [0]=rumble-slow, [1]=tick-slow, [2]=rumble-med, [3]=tick-med,
//         [4]=rumble-fast, [5]=tick-fast
static const Color4 BUTTON_COLORS[] = {
    Color4(255, 200, 180, 255), // Peach (rumble-slow)
    Color4(180, 200, 255, 255), // Periwinkle (tick-slow)
    Color4(255, 140, 100, 255), // Salmon (rumble-medium)
    Color4(100, 150, 255, 255), // Cornflower (tick-medium)
    Color4(255, 80, 60, 255),   // Red-orange (rumble-fast)
    Color4(60, 100, 255, 255)   // Blue (tick-fast)
};

static const Color4 BUTTON_COLORS_PRESSED[] = {
    Color4(225, 170, 150, 255), Color4(150, 170, 225, 255),
    Color4(225, 110, 70, 255), Color4(70, 120, 225, 255),
    Color4(225, 50, 30, 255), Color4(30, 70, 225, 255)};

static const Color4 BUTTON_COLORS_INACTIVE[] = {
    Color4(200, 185, 175, 255), Color4(175, 185, 200, 255),
    Color4(200, 160, 150, 255), Color4(150, 165, 200, 255),
    Color4(200, 140, 135, 255), Color4(135, 150, 200, 255)};

// Haptic parameters - all heavy intensity, varying sharpness and speed
// Left column (0,2,4): low sharpness (rumble)
// Right column (1,3,5): high sharpness (tick)
static const float BUTTON_SHARPNESS[] = {0.1f, 0.9f, 0.1f, 0.9f, 0.1f, 0.9f};

// Intervals: top=slow (120ms), middle=medium (60ms), bottom=fast (30ms)
static const float BUTTON_INTERVALS[] = {0.12f, 0.12f, 0.06f,
                                         0.06f, 0.03f, 0.03f};

// Grid configuration - 3 rows, 2 columns
#define BUTTON_RADIUS_RATIO 0.10f
#define GRID_COLS 2
#define GRID_ROWS 3
#define SPACING_RATIO 0.08f

#pragma mark -
#pragma mark Constructors

F3snakeyes::F3snakeyes() : FidgetableView(), _buttonRadius(50.0f)
{
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    _toggleStates[i] = false;
    _currentScales[i] = 1.0f;
    _hapticTimers[i] = 0.0f;
  }
}

F3snakeyes::~F3snakeyes() { dispose(); }

bool F3snakeyes::init(int index, const cugl::Size &pageSize)
{
  _buttonRadius = pageSize.width * BUTTON_RADIUS_RATIO;
  return FidgetableView::init(index, pageSize);
}

std::shared_ptr<F3snakeyes> F3snakeyes::alloc(const cugl::Size &pageSize)
{
  auto result = std::make_shared<F3snakeyes>();
  if (result->init(3, pageSize))
  {
    return result;
  }
  return nullptr;
}

void F3snakeyes::dispose()
{
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    if (_buttons[i] != nullptr)
    {
      _buttons[i]->deactivate();
      _buttons[i]->clearListeners();
      _buttons[i] = nullptr;
    }
    _buttonNodes[i] = nullptr;
  }
  FidgetableView::dispose();
}

#pragma mark -
#pragma mark Content Building

void F3snakeyes::buildContent()
{
  float spacing = _pageSize.width * SPACING_RATIO;
  float gridWidth = GRID_COLS * (_buttonRadius * 2) + (GRID_COLS - 1) * spacing;
  float gridHeight =
      GRID_ROWS * (_buttonRadius * 2) + (GRID_ROWS - 1) * spacing;

  Vec2 gridOrigin((_pageSize.width - gridWidth) / 2 + _buttonRadius,
                  (_pageSize.height - gridHeight) / 2 + _buttonRadius);

  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    int col = i % GRID_COLS;
    int row = (GRID_ROWS - 1) - (i / GRID_COLS);

    Vec2 pos(gridOrigin.x + col * (_buttonRadius * 2 + spacing),
             gridOrigin.y + row * (_buttonRadius * 2 + spacing));

    auto normalNode = createCircle(_buttonRadius, BUTTON_COLORS[i]);
    auto pressedNode = createCircle(_buttonRadius, BUTTON_COLORS_PRESSED[i]);

    _buttonNodes[i] = normalNode;

    _buttons[i] = Button::alloc(normalNode, pressedNode);
    _buttons[i]->setAnchor(Vec2::ANCHOR_CENTER);
    _buttons[i]->setPosition(pos);
    _buttons[i]->setName("f3snakeyes_btn_" + std::to_string(i));

    int btnIndex = i;
    _buttons[i]->addListener(
        [this, btnIndex](const std::string &name, bool down)
        {
          if (!down && _isActive)
          {
            onToggle(btnIndex);
          }
        });

    _rootNode->addChild(_buttons[i]);
  }
}

#pragma mark -
#pragma mark Interaction

void F3snakeyes::onToggle(int index)
{
  _toggleStates[index] = !_toggleStates[index];
  _hapticTimers[index] = 0.0f; // Reset timer to play immediately

  CULog("F3snakeyes button %d toggled %s (sharpness=%.1f, interval=%.0fms)",
        index, _toggleStates[index] ? "ON" : "OFF", BUTTON_SHARPNESS[index],
        BUTTON_INTERVALS[index] * 1000);

  // Play feedback on toggle
  if (_toggleStates[index])
  {
    Haptics::transient(1.0f, BUTTON_SHARPNESS[index]);
  }
  else
  {
    Haptics::light();
  }
}

void F3snakeyes::update(float timestep)
{
  FidgetableView::update(timestep);

  // Animate button scales
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    if (_buttons[i] == nullptr)
      continue;

    float targetScale = _toggleStates[i] ? TOGGLED_SCALE : 1.0f;

    if (std::abs(_currentScales[i] - targetScale) > 0.001f)
    {
      float speed = timestep / ANIM_DURATION;
      float diff = targetScale - _currentScales[i];
      _currentScales[i] += diff * std::min(1.0f, speed * 6.0f);
      _buttons[i]->setScale(_currentScales[i]);
    }
    else
    {
      _currentScales[i] = targetScale;
      _buttons[i]->setScale(_currentScales[i]);
    }
  }

  // Per-button haptic timers
  if (_isActive)
  {
    for (int i = 0; i < NUM_BUTTONS; i++)
    {
      if (_toggleStates[i])
      {
        _hapticTimers[i] -= timestep;
        if (_hapticTimers[i] <= 0.0f)
        {
          Haptics::transient(1.0f, BUTTON_SHARPNESS[i]);
          _hapticTimers[i] = BUTTON_INTERVALS[i];
        }
      }
    }
  }
}

void F3snakeyes::setActive(bool active)
{
  FidgetableView::setActive(active);

  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    if (_buttonNodes[i] != nullptr)
    {
      if (active)
      {
        _buttonNodes[i]->setColor(BUTTON_COLORS[i]);
      }
      else
      {
        _buttonNodes[i]->setColor(BUTTON_COLORS_INACTIVE[i]);
      }
    }
  }
}

void F3snakeyes::activateInputs()
{
  for (auto &btn : _buttons)
  {
    if (btn != nullptr)
      btn->activate();
  }
}

void F3snakeyes::deactivateInputs()
{
  for (auto &btn : _buttons)
  {
    if (btn != nullptr)
      btn->deactivate();
  }
}
