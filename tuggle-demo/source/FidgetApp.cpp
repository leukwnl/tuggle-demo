//
//  FidgetApp.cpp
//  Tuggle
//
//  Implementation of the main Fidgetable carousel application.
//  Uses InputController for unified pointer input.
//

#include "FidgetApp.h"
#include "AudioController.h"
#include <cugl/audio/CUAudioEngine.h>
#include <cugl/audio/CUSoundLoader.h>
#include <cugl/core/input/CUAccelerometer.h>
#include <cugl/core/input/CUHaptics.h>

using namespace cugl;
using namespace cugl::graphics;
using namespace cugl::scene2;

// Game height used for scene scaling
#define GAME_HEIGHT 1024

// Background color (light warm gray)
#define BACKGROUND_COLOR Color4(40, 40, 40, 255)

#pragma mark -
#pragma mark Application Lifecycle

void FidgetApp::onStartup()
{
  _scene = Scene2::allocWithHint(Size(0, GAME_HEIGHT));

  // Create sprite batch and set background color
  _batch = SpriteBatch::alloc();
  setClearColor(BACKGROUND_COLOR);
  _scene->setSpriteBatch(_batch);

  // Create and configure the asset manager
  _assets = AssetManager::alloc();
  _assets->attach<Texture>(TextureLoader::alloc()->getHook());
  _assets->attach<Font>(FontLoader::alloc()->getHook());
  _assets->attach<audio::Sound>(audio::SoundLoader::alloc()->getHook());

  // Load assets from JSON
  _assets->loadDirectory("json/assets.json");

  // Initialize the InputController singleton
  InputController *input = InputController::getInstance();
  input->init();
  input->start();

  // Initialize haptic feedback system
  Haptics::init();

  // Initialize audio engine and controller for F9soundboard
  audio::AudioEngine::start();
  AudioController::start();

  // Activate accelerometer
  Input::activate<Accelerometer>();

  // Build the scene
  buildScene();

  // Call parent startup
  Application::onStartup();

  // // Log safe area info for debugging
  // Rect bounds = getSafeBounds();
  // CULog("Safe Area: %sx%s", bounds.origin.toString().c_str(),
  //       bounds.size.toString().c_str());
  // CULog("Drag left/right to navigate between fidgetables");
  // CULog("Tap the centered circle to interact");
}

void FidgetApp::onShutdown()
{
  // Deactivate accelerometer
  Input::deactivate<Accelerometer>();

  // Stop audio controller and engine (controller first since it uses engine)
  AudioController::stop();
  audio::AudioEngine::stop();

  // Dispose haptic feedback system
  Haptics::dispose();

  // Stop and release the InputController
  InputController::getInstance()->stop();
  InputController::release();

  // Dispose carousel
  if (_carousel != nullptr)
  {
    _carousel->dispose();
    _carousel = nullptr;
  }

  // Clear all resources
  _scene = nullptr;
  _batch = nullptr;
  _assets = nullptr;

  Application::onShutdown();
}

#pragma mark -
#pragma mark Scene Building

void FidgetApp::buildScene()
{
  // Get display size and calculate scale
  Size displaySize = getDisplaySize();
  _screenToSceneScale = GAME_HEIGHT / displaySize.height;
  Size scaledSize = displaySize * _screenToSceneScale;

  // Create the carousel controller with the scale factor and asset manager
  _carousel =
      SwipeCarouselController::alloc(_scene, scaledSize, _screenToSceneScale, _assets);

  // Activate button inputs after adding to scene
  _carousel->activateInputs();

  CULog("Carousel initialized with page size: %s",
        scaledSize.toString().c_str());
  CULog("Screen to scene scale: %f", _screenToSceneScale);
}

#pragma mark -
#pragma mark Game Loop

void FidgetApp::update(float timestep)
{
  // Update the InputController first (processes input state)
  InputController *input = InputController::getInstance();
  input->update(timestep);

  // Update the carousel (uses InputController for input)
  if (_carousel != nullptr)
  {
    _carousel->update(timestep);
  }

  // Clear interaction flags at the end of the frame
  input->clearInteractionFlags();
}

void FidgetApp::draw()
{
  // Render the scene graph
  _scene->render();
}
