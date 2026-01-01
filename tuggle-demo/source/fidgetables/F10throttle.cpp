//
//  F10throttle.cpp
//  Tuggle
//
//  Realistic car simulator with button-based shifting and pedal throttle.
//  Demonstrates HapticPlayer for continuous haptic feedback.
//

#include "F10throttle.h"
#include "InputController.h"
#include <cmath>

using namespace cugl;
using namespace cugl::scene2;

// Colors
static const Color4 TICK_COLOR(180, 180, 180, 255);
static const Color4 NEEDLE_COLOR(255, 40, 40, 255);
static const Color4 GEAR_INACTIVE_COLOR(60, 60, 70, 255);
static const Color4 GEAR_ACTIVE_COLOR(100, 200, 255, 255);
static const Color4 BUTTON_COLOR(70, 70, 80, 255);
static const Color4 BUTTON_PRESSED_COLOR(100, 160, 100, 255);
static const Color4 SHIFT_UP_COLOR(80, 80, 90, 255);
static const Color4 SHIFT_DOWN_COLOR(80, 80, 90, 255);
static const Color4 THROTTLE_COLOR(60, 60, 65, 255);
static const Color4 THROTTLE_PRESSED_COLOR(80, 160, 80, 255);
static const Color4 STALL_COLOR(255, 60, 60, 255);
static const Color4 HUB_COLOR(50, 50, 55, 255);

F10throttle::F10throttle()
    : FidgetableView(), _currentGear(Gear::NEUTRAL), _currentRPM(0.0f),
      _isThrottling(false), _engineRunning(false), _isStalled(false) {}

F10throttle::~F10throttle() { dispose(); }

bool F10throttle::init(int index, const Size &pageSize) {
  return FidgetableView::init(index, pageSize);
}

std::shared_ptr<F10throttle> F10throttle::alloc(const Size &pageSize) {
  auto result = std::make_shared<F10throttle>();
  if (result->init(10, pageSize)) {
    return result;
  }
  return nullptr;
}

void F10throttle::dispose() {
  stopEngine();
  if (_shiftUpButton)
    _shiftUpButton->deactivate();
  if (_shiftDownButton)
    _shiftDownButton->deactivate();
  if (_throttleButton)
    _throttleButton->deactivate();
  FidgetableView::dispose();
}

std::shared_ptr<PolygonNode> F10throttle::createCircle(float radius,
                                                       Color4 color) {
  constexpr int segments = 48;
  std::vector<Vec2> vertices;
  vertices.reserve(segments + 2);
  vertices.push_back(Vec2::ZERO);

  for (int i = 0; i <= segments; i++) {
    float angle = (float)i / (float)segments * 2.0f * M_PI;
    vertices.push_back(Vec2(radius * cosf(angle), radius * sinf(angle)));
  }

  std::vector<Uint32> indices;
  indices.reserve(segments * 3);
  for (int i = 1; i <= segments; i++) {
    indices.push_back(0);
    indices.push_back(i);
    indices.push_back(i + 1);
  }

  auto node = PolygonNode::allocWithPoly(Poly2(vertices, indices));
  node->setColor(color);
  node->setAnchor(Vec2::ANCHOR_CENTER);
  return node;
}

std::shared_ptr<PolygonNode> F10throttle::createRect(float width, float height,
                                                     Color4 color) {
  std::vector<Vec2> vertices = {Vec2(0, 0), Vec2(width, 0), Vec2(width, height),
                                Vec2(0, height)};
  std::vector<Uint32> indices = {0, 1, 2, 0, 2, 3};

  auto node = PolygonNode::allocWithPoly(Poly2(vertices, indices));
  node->setColor(color);
  node->setAnchor(Vec2::ANCHOR_CENTER);
  return node;
}

void F10throttle::buildContent() {
  buildGauge();
  buildGearControls();
  buildThrottlePedal();

  _currentRPM = 0.0f;
  _engineRunning = false;
  _isStalled = false;
}

void F10throttle::buildGauge() {
  Vec2 center(_pageSize.width / 2, _pageSize.height * GAUGE_CENTER_Y_RATIO);
  float radius = _pageSize.width * GAUGE_RADIUS_RATIO;

  // Gauge arc: left to right (180° to 0°)
  float startAngle = M_PI; // Left (0%)
  float angleRange = M_PI;

  // Create tick marks only
  int numTicks = 11; // 0%, 10%, 20%, ... 100%
  float tickInnerRadius = radius * 0.75f;

  for (int i = 0; i <= numTicks; i++) {
    float t = (float)i / (float)numTicks;
    float angle = startAngle - t * angleRange;

    bool isMajor = (i % 2 == 0);
    float tickLength = isMajor ? 25.0f : 12.0f;
    float tickWidth = isMajor ? 4.0f : 2.0f;

    auto tick = createRect(tickWidth, tickLength, TICK_COLOR);
    tick->setAnchor(Vec2(0.5f, 0.0f));

    Vec2 tickPos = center + Vec2(tickInnerRadius * cosf(angle),
                                 tickInnerRadius * sinf(angle));
    tick->setPosition(tickPos);
    tick->setAngle(angle - M_PI / 2);

    _gaugeTicks.push_back(tick);
    _rootNode->addChild(tick);
  }

  // Needle
  float needleLength = radius * 0.95f;
  float needleWidth = 6.0f;
  _needle = createRect(needleWidth, needleLength, NEEDLE_COLOR);
  _needle->setAnchor(Vec2(0.5f, 0.08f));
  _needle->setPosition(center);
  _needle->setAngle(startAngle - M_PI / 2); // Start pointing left
  _rootNode->addChild(_needle);

  // Center hub
  auto hub = createCircle(radius * 0.12f, HUB_COLOR);
  hub->setPosition(center);
  _rootNode->addChild(hub);
}

void F10throttle::buildGearControls() {
  float centerX = _pageSize.width / 2;
  float controlsY = _pageSize.height * 0.30f;

  // Gear level indicator bars (5 bars for gears 1-5)
  float barWidth = _pageSize.width * 0.08f;
  float barHeight = _pageSize.height * 0.04f;
  float barSpacing = barWidth * 1.4f;
  float totalWidth = 5 * barWidth + 4 * (barSpacing - barWidth);
  float startX = centerX - totalWidth / 2 + barWidth / 2;

  for (int i = 0; i < 5; i++) {
    auto bar = createRect(barWidth, barHeight, GEAR_INACTIVE_COLOR);
    bar->setPosition(Vec2(startX + i * barSpacing, controlsY + barHeight * 2));
    _gearBars[i] = bar;
    _rootNode->addChild(bar);
  }

  // Gear indicator (larger bar showing current gear)
  float indicatorWidth = _pageSize.width * 0.15f;
  float indicatorHeight = _pageSize.height * 0.06f;
  _gearIndicator =
      createRect(indicatorWidth, indicatorHeight, GEAR_INACTIVE_COLOR);
  _gearIndicator->setPosition(Vec2(centerX, controlsY - barHeight));
  _rootNode->addChild(_gearIndicator);

  // Shift buttons
  float buttonWidth = _pageSize.width * 0.18f;
  float buttonHeight = _pageSize.height * 0.08f;
  float buttonSpacing = _pageSize.width * 0.25f;

  // Shift DOWN button (left)
  auto downNormal = createRect(buttonWidth, buttonHeight, SHIFT_DOWN_COLOR);
  auto downPressed =
      createRect(buttonWidth, buttonHeight, BUTTON_PRESSED_COLOR);

  _shiftDownButton = Button::alloc(downNormal, downPressed);
  _shiftDownButton->setAnchor(Vec2::ANCHOR_CENTER);
  _shiftDownButton->setPosition(
      Vec2(centerX - buttonSpacing, controlsY - barHeight * 3.5f));
  _shiftDownButton->setName("f10_shift_down");

  _shiftDownButton->addListener([this](const std::string &name, bool down) {
    if (_isActive && down) {
      shiftDown();
    }
  });
  _rootNode->addChild(_shiftDownButton);

  // Down arrow on button
  float arrowSize = buttonHeight * 0.35f;
  std::vector<Vec2> downArrowVerts = {Vec2(0, -arrowSize),
                                      Vec2(-arrowSize * 0.7f, arrowSize * 0.4f),
                                      Vec2(arrowSize * 0.7f, arrowSize * 0.4f)};
  std::vector<Uint32> arrowIndices = {0, 1, 2};
  auto downArrow =
      PolygonNode::allocWithPoly(Poly2(downArrowVerts, arrowIndices));
  downArrow->setColor(Color4(200, 200, 200, 255));
  downArrow->setAnchor(Vec2::ANCHOR_CENTER);
  downArrow->setPosition(
      Vec2(centerX - buttonSpacing, controlsY - barHeight * 3.5f));
  _rootNode->addChild(downArrow);

  // Shift UP button (right)
  auto upNormal = createRect(buttonWidth, buttonHeight, SHIFT_UP_COLOR);
  auto upPressed = createRect(buttonWidth, buttonHeight, BUTTON_PRESSED_COLOR);

  _shiftUpButton = Button::alloc(upNormal, upPressed);
  _shiftUpButton->setAnchor(Vec2::ANCHOR_CENTER);
  _shiftUpButton->setPosition(
      Vec2(centerX + buttonSpacing, controlsY - barHeight * 3.5f));
  _shiftUpButton->setName("f10_shift_up");

  _shiftUpButton->addListener([this](const std::string &name, bool down) {
    if (_isActive && down) {
      shiftUp();
    }
  });
  _rootNode->addChild(_shiftUpButton);

  // Up arrow on button
  std::vector<Vec2> upArrowVerts = {Vec2(0, arrowSize),
                                    Vec2(-arrowSize * 0.7f, -arrowSize * 0.4f),
                                    Vec2(arrowSize * 0.7f, -arrowSize * 0.4f)};
  auto upArrow = PolygonNode::allocWithPoly(Poly2(upArrowVerts, arrowIndices));
  upArrow->setColor(Color4(200, 200, 200, 255));
  upArrow->setAnchor(Vec2::ANCHOR_CENTER);
  upArrow->setPosition(
      Vec2(centerX + buttonSpacing, controlsY - barHeight * 3.5f));
  _rootNode->addChild(upArrow);
}

void F10throttle::buildThrottlePedal() {
  // Rectangular pedal at the bottom
  float pedalWidth = _pageSize.width * 0.35f;
  float pedalHeight = _pageSize.height * 0.12f;
  Vec2 pedalPos(_pageSize.width / 2, _pageSize.height * 0.10f);

  auto normalNode = createRect(pedalWidth, pedalHeight, THROTTLE_COLOR);
  auto pressedNode =
      createRect(pedalWidth, pedalHeight, THROTTLE_PRESSED_COLOR);
  _throttleNode = normalNode;

  _throttleButton = Button::alloc(normalNode, pressedNode);
  _throttleButton->setAnchor(Vec2::ANCHOR_CENTER);
  _throttleButton->setPosition(pedalPos);
  _throttleButton->setName("f10_throttle");

  _throttleButton->addListener([this](const std::string &name, bool down) {
    if (_isActive) {
      bool wasThrottling = _isThrottling;
      _isThrottling = down;
      _isInteracting = down;

      if (down) {
        if (_isStalled) {
          _isStalled = false;
          startEngine();
        } else if (!_engineRunning) {
          startEngine();
        } else if (!wasThrottling) {
          // Throttle pressed - kick of acceleration
          Haptics::transient(0.6f, 0.25f);
        }
      } else if (wasThrottling && _engineRunning) {
        // Throttle released - engine settling back
        Haptics::transient(0.4f, 0.15f);
      }
    }
  });

  _rootNode->addChild(_throttleButton);

  // Pedal texture lines
  int numLines = 5;
  float lineSpacing = pedalWidth / (numLines + 1);
  for (int i = 1; i <= numLines; i++) {
    auto line = createRect(3.0f, pedalHeight * 0.6f, Color4(40, 40, 45, 255));
    line->setPosition(pedalPos + Vec2(-pedalWidth / 2 + i * lineSpacing, 0));
    _rootNode->addChild(line);
  }
}

float F10throttle::getMaxRPMForGear(Gear gear) {
  switch (gear) {
  case Gear::NEUTRAL:
    return 3000.0f;
  case Gear::GEAR_1:
    return 5000.0f;
  case Gear::GEAR_2:
    return 7000.0f;
  case Gear::GEAR_3:
    return 10000.0f;
  case Gear::GEAR_4:
    return 12500.0f;
  case Gear::GEAR_5:
    return 15000.0f;
  case Gear::REVERSE:
    return 20000.0f;
  }
  return 3000.0f;
}

float F10throttle::getShiftWindowMinRPM() {
  // Must be in top 15% of current gear's max RPM to shift up
  float maxRPM = getMaxRPMForGear(_currentGear);
  return maxRPM * (1.0f - SHIFT_WINDOW_PERCENT);
}

bool F10throttle::isInShiftWindow() {
  return _currentRPM >= getShiftWindowMinRPM();
}

void F10throttle::shiftUp() {
  // Can't shift up from neutral without being in a gear first
  if (_currentGear == Gear::NEUTRAL) {
    // Shift to 1st from neutral - satisfying clunk into gear
    _currentGear = Gear::GEAR_1;
    Haptics::heavy();
    Haptics::buzz(0.7f, 0.2f, 0.1f); // Short deep buzz
    updateGearIndicator();
    updateHaptics();
    return;
  }

  // Can't shift up from 5th - limiter hit
  if (_currentGear == Gear::GEAR_5 || _currentGear == Gear::REVERSE) {
    Haptics::heavy();
    Haptics::transient(0.8f, 0.9f); // Sharp rejection
    return;
  }

  // Check if in shift window
  if (!isInShiftWindow()) {
    // STALL - shifted too early!
    stallEngine();
    return;
  }

  // Successful upshift - satisfying mechanical clunk
  int currentNum = static_cast<int>(_currentGear);
  Gear newGear = static_cast<Gear>(currentNum + 1);

  // Gear-specific shift feel
  switch (newGear) {
  case Gear::GEAR_2:
    Haptics::heavy();
    Haptics::buzz(0.8f, 0.25f, 0.08f);
    break;
  case Gear::GEAR_3:
    Haptics::heavy();
    Haptics::transient(0.9f, 0.4f);
    break;
  case Gear::GEAR_4:
    Haptics::heavy();
    Haptics::transient(0.95f, 0.5f);
    break;
  case Gear::GEAR_5:
    // Into top gear - aggressive snap
    Haptics::heavy();
    Haptics::transient(1.0f, 0.7f);
    Haptics::buzz(0.6f, 0.6f, 0.05f);
    break;
  default:
    Haptics::heavy();
    break;
  }

  _currentGear = newGear;

  // RPM drops when upshifting (relative to new gear's max)
  float newMax = getMaxRPMForGear(_currentGear);
  _currentRPM = newMax * 0.4f;
  _currentRPM = std::max(IDLE_RPM, _currentRPM);

  updateGearIndicator();
  updateHaptics();
}

void F10throttle::shiftDown() {
  // Can't shift down from neutral or 1st
  if (_currentGear == Gear::NEUTRAL || _currentGear == Gear::GEAR_1) {
    if (_currentGear == Gear::GEAR_1) {
      // Pop out of 1st into neutral
      _currentGear = Gear::NEUTRAL;
      Haptics::medium();
      Haptics::transient(0.5f, 0.3f);
      updateGearIndicator();
      updateHaptics();
    } else {
      // Already in neutral - soft bump
      Haptics::transient(0.4f, 0.2f);
    }
    return;
  }

  if (_currentGear == Gear::REVERSE) {
    _currentGear = Gear::NEUTRAL;
    Haptics::medium();
    updateGearIndicator();
    updateHaptics();
    return;
  }

  // Downshift - aggressive engine braking feel
  int currentNum = static_cast<int>(_currentGear);
  Gear newGear = static_cast<Gear>(currentNum - 1);

  // Aggressive downshift haptics - feels like engine braking
  Haptics::heavy();
  Haptics::transient(0.85f, 0.35f); // Mechanical clunk

  // Brief aggressive buzz simulating engine braking surge
  Haptics::buzz(0.75f, 0.3f, 0.12f);

  _currentGear = newGear;

  // RPM spikes when downshifting (engine braking effect)
  float newMax = getMaxRPMForGear(_currentGear);
  _currentRPM = std::min(_currentRPM * 1.3f, newMax * 0.85f);
  _currentRPM = std::max(IDLE_RPM, _currentRPM);

  updateGearIndicator();
  updateHaptics();
}

void F10throttle::stallEngine() {
  _isStalled = true;
  _engineRunning = false;
  _currentRPM = 0.0f;

  _enginePlayer.stop();

  // VIOLENT stall feedback - you messed up!
  // Initial heavy impact
  Haptics::heavy();

  // Grinding/crunching feel
  Haptics::buzz(1.0f, 0.5f, 0.15f);

  // Secondary impact as engine dies
  Haptics::transient(0.9f, 0.3f);

  // Final shudder
  Haptics::buzz(0.7f, 0.2f, 0.2f);

  // Visual - flash gear indicator red
  if (_gearIndicator) {
    _gearIndicator->setColor(STALL_COLOR);
  }

  // Reset to neutral
  _currentGear = Gear::NEUTRAL;
  updateGearIndicator();
}

void F10throttle::startEngine() {
  if (_engineRunning)
    return;

  _engineRunning = true;
  _isStalled = false;
  _currentRPM = IDLE_RPM;

  // Engine start sequence - starter motor then catch
  Haptics::buzz(0.6f, 0.4f, 0.15f); // Starter whirr
  Haptics::transient(0.7f, 0.3f);   // Engine catches
  Haptics::buzz(0.5f, 0.2f, 0.1f);  // Settles to idle

  // Start the continuous engine rumble - substantial even at idle
  _enginePlayer.setLooping(true);
  _enginePlayer.start(0.5f, 0.15f); // Strong idle rumble

  updateGearIndicator();
}

void F10throttle::stopEngine() {
  if (!_engineRunning)
    return;

  _engineRunning = false;
  _currentRPM = 0.0f;
  _enginePlayer.stop();
}

void F10throttle::updateGearIndicator() {
  // Update gear bars
  int gearNum = 0;
  switch (_currentGear) {
  case Gear::GEAR_1:
    gearNum = 1;
    break;
  case Gear::GEAR_2:
    gearNum = 2;
    break;
  case Gear::GEAR_3:
    gearNum = 3;
    break;
  case Gear::GEAR_4:
    gearNum = 4;
    break;
  case Gear::GEAR_5:
    gearNum = 5;
    break;
  default:
    gearNum = 0;
    break;
  }

  for (int i = 0; i < 5; i++) {
    if (_gearBars[i]) {
      _gearBars[i]->setColor((i < gearNum) ? GEAR_ACTIVE_COLOR
                                           : GEAR_INACTIVE_COLOR);
    }
  }

  // Update main gear indicator
  if (_gearIndicator && !_isStalled) {
    Color4 indicatorColor;
    switch (_currentGear) {
    case Gear::NEUTRAL:
      indicatorColor = Color4(100, 100, 100, 255);
      break;
    case Gear::GEAR_1:
      indicatorColor = Color4(80, 200, 100, 255);
      break;
    case Gear::GEAR_2:
      indicatorColor = Color4(150, 220, 80, 255);
      break;
    case Gear::GEAR_3:
      indicatorColor = Color4(220, 220, 60, 255);
      break;
    case Gear::GEAR_4:
      indicatorColor = Color4(255, 160, 50, 255);
      break;
    case Gear::GEAR_5:
      indicatorColor = Color4(255, 80, 80, 255);
      break;
    case Gear::REVERSE:
      indicatorColor = Color4(180, 80, 255, 255);
      break;
    }
    _gearIndicator->setColor(indicatorColor);
  }
}

void F10throttle::updateGaugeVisuals() {
  float maxRPM = getMaxRPMForGear(_currentGear);
  // Needle shows 0-100% of CURRENT GEAR's max
  float rpmRatio = _currentRPM / maxRPM;
  rpmRatio = std::min(1.0f, std::max(0.0f, rpmRatio));

  // Needle angle: π (left, 0%) to 0 (right, 100%)
  float startAngle = M_PI;
  float needleAngle = startAngle - rpmRatio * M_PI - M_PI / 2;

  if (_needle) {
    _needle->setAngle(needleAngle);
  }

  // Pulse gear indicator when near max RPM (time to shift!)
  if (_gearIndicator && !_isStalled) {
    if (rpmRatio > 0.85f && _currentGear != Gear::GEAR_5 &&
        _currentGear != Gear::NEUTRAL && _currentGear != Gear::REVERSE) {
      float pulse = 1.0f + 0.15f * sinf(_currentRPM * 0.05f);
      _gearIndicator->setScale(pulse);

      // Change color to indicate shift window
      if (isInShiftWindow()) {
        _gearIndicator->setColor(
            Color4(100, 255, 100, 255)); // Green = safe to shift
      }
    } else {
      _gearIndicator->setScale(1.0f);
    }
  }
}

void F10throttle::updateHaptics() {
  if (!_engineRunning)
    return;

  float maxRPM = getMaxRPMForGear(_currentGear);
  float rpmRatio = maxRPM > 0.0f ? (_currentRPM / maxRPM) : 0.0f;

  // Minimum floor intensity - higher gears never drop below this
  // This creates the feeling that higher gears have more "load" even at low RPM
  float floorIntensity = 0.0f;
  float floorSharpness = 0.0f;

  // Base intensity at idle RPM, max intensity at redline
  float baseIntensity = 0.0f;
  float maxIntensity = 0.0f;
  float baseSharpness = 0.0f;
  float maxSharpness = 0.0f;

  switch (_currentGear) {
  case Gear::NEUTRAL:
    floorIntensity = 0.15f;
    floorSharpness = 0.1f;
    baseIntensity = 0.3f;
    maxIntensity = 0.5f;
    baseSharpness = 0.15f;
    maxSharpness = 0.25f;
    break;
  case Gear::GEAR_1:
    // 1st gear: Deep, heavy rumble - like a diesel truck
    floorIntensity = 0.25f;
    floorSharpness = 0.08f;
    baseIntensity = 0.5f;
    maxIntensity = 0.85f;
    baseSharpness = 0.1f; // Very low sharpness = deep rumble
    maxSharpness = 0.25f;
    break;
  case Gear::GEAR_2:
    // 2nd gear: Strong, slightly tighter
    floorIntensity = 0.35f;
    floorSharpness = 0.12f;
    baseIntensity = 0.55f;
    maxIntensity = 0.9f;
    baseSharpness = 0.2f;
    maxSharpness = 0.35f;
    break;
  case Gear::GEAR_3:
    // 3rd gear: Powerful, balanced
    floorIntensity = 0.4f;
    floorSharpness = 0.18f;
    baseIntensity = 0.6f;
    maxIntensity = 0.95f;
    baseSharpness = 0.3f;
    maxSharpness = 0.5f;
    break;
  case Gear::GEAR_4:
    // 4th gear: Aggressive, higher pitch feel
    floorIntensity = 0.45f;
    floorSharpness = 0.25f;
    baseIntensity = 0.65f;
    maxIntensity = 1.0f;
    baseSharpness = 0.45f;
    maxSharpness = 0.7f;
    break;
  case Gear::GEAR_5:
    // 5th gear: Screaming, intense, buzzy - highest floor
    floorIntensity = 0.5f;
    floorSharpness = 0.35f;
    baseIntensity = 0.7f;
    maxIntensity = 1.0f;
    baseSharpness = 0.6f;
    maxSharpness = 0.9f;
    break;
  case Gear::REVERSE:
    floorIntensity = 0.2f;
    floorSharpness = 0.1f;
    baseIntensity = 0.4f;
    maxIntensity = 0.6f;
    baseSharpness = 0.15f;
    maxSharpness = 0.3f;
    break;
  }

  // Interpolate based on RPM ratio
  float intensity = baseIntensity + rpmRatio * (maxIntensity - baseIntensity);
  float sharpness = baseSharpness + rpmRatio * (maxSharpness - baseSharpness);

  // Ensure we never drop below the floor (vibration never fully stops in gear)
  intensity = std::max(intensity, floorIntensity);
  sharpness = std::max(sharpness, floorSharpness);

  // Add extra punch when throttling hard at high RPM
  if (_isThrottling && rpmRatio > 0.7f) {
    intensity = std::min(1.0f, intensity + 0.1f);
  }

  // Near redline - max intensity with pulsing feel
  if (rpmRatio > 0.9f) {
    intensity = 1.0f;
  }

  _enginePlayer.setIntensity(intensity);
  _enginePlayer.setSharpness(sharpness);
}

void F10throttle::update(float timestep) {
  FidgetableView::update(timestep);

  static float redlineHapticTimer = 0.0f;

  if (_engineRunning && !_isStalled) {
    float maxRPM = getMaxRPMForGear(_currentGear);

    if (_isThrottling) {
      _currentRPM += RPM_ACCEL_RATE * timestep;

      // Hit the rev limiter
      if (_currentRPM >= maxRPM) {
        _currentRPM = maxRPM;

        // Periodic limiter haptic bursts
        redlineHapticTimer -= timestep;
        if (redlineHapticTimer <= 0.0f) {
          Haptics::transient(0.9f, 0.8f); // Sharp limiter hit
          redlineHapticTimer = 0.08f;     // Rapid bursts
        }
      } else {
        redlineHapticTimer = 0.0f;
      }
    } else {
      // Deceleration - RPM always decelerates toward 0 on all gears
      _currentRPM -= RPM_DECEL_RATE * timestep;

      // In neutral, clamp to idle RPM (engine stays running)
      if (_currentGear == Gear::NEUTRAL) {
        _currentRPM = std::max(_currentRPM, IDLE_RPM);
      } else {
        // In any gear, hitting 0 RPM causes a stall
        if (_currentRPM <= 0.0f) {
          stallEngine();
          redlineHapticTimer = 0.0f;
        } else {
          _currentRPM = std::max(_currentRPM, 0.0f);
        }
      }

      redlineHapticTimer = 0.0f;
    }

    updateHaptics();
  }

  updateGaugeVisuals();
}

void F10throttle::setActive(bool active) {
  FidgetableView::setActive(active);

  if (!active && _engineRunning) {
    _enginePlayer.pause();
  } else if (active && _engineRunning && !_isStalled) {
    _enginePlayer.play();
  }
}

void F10throttle::activateInputs() {
  if (_shiftUpButton)
    _shiftUpButton->activate();
  if (_shiftDownButton)
    _shiftDownButton->activate();
  if (_throttleButton)
    _throttleButton->activate();
}

void F10throttle::deactivateInputs() {
  if (_shiftUpButton)
    _shiftUpButton->deactivate();
  if (_shiftDownButton)
    _shiftDownButton->deactivate();
  if (_throttleButton)
    _throttleButton->deactivate();
}
