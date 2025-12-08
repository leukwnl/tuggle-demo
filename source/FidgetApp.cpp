//
//  FidgetApp.cpp
//  Tuggle
//
//  Implementation of the main Fidgetable carousel application.
//  Uses InputController for unified pointer input.
//

#include "FidgetApp.h"

using namespace cugl;
using namespace cugl::graphics;
using namespace cugl::scene2;

// Game height used for scene scaling
#define GAME_HEIGHT 1024

// Background color (light warm gray)
#define BACKGROUND_COLOR Color4(245, 243, 240, 255)

#pragma mark -
#pragma mark Application Lifecycle

void FidgetApp::onStartup() {
    // Create a scene graph scaled to game height
    _scene = Scene2::allocWithHint(Size(0, GAME_HEIGHT));
    
    // Create sprite batch and set background color
    _batch = SpriteBatch::alloc();
    setClearColor(BACKGROUND_COLOR);
    _scene->setSpriteBatch(_batch);
    
    // Create and configure the asset manager
    _assets = AssetManager::alloc();
    _assets->attach<Texture>(TextureLoader::alloc()->getHook());
    _assets->attach<Font>(FontLoader::alloc()->getHook());
    
    // Load assets from JSON
    _assets->loadDirectory("json/assets.json");
    
    // Get the font for labels
    _font = _assets->get<Font>("sans");
    
    // Initialize the InputController singleton
    InputController* input = InputController::getInstance();
    input->init();
    input->start();
    
    // Build the scene with carousel
    buildScene();
    
    // Call parent startup (transitions to FOREGROUND state)
    Application::onStartup();
    
    // Log safe area info for debugging
    Rect bounds = getSafeBounds();
    CULog("Safe Area: %sx%s", bounds.origin.toString().c_str(),
          bounds.size.toString().c_str());
    CULog("Drag left/right to navigate between fidgetables");
    CULog("Tap the centered circle to interact");
}

void FidgetApp::onShutdown() {
    // Stop and release the InputController
    InputController::getInstance()->stop();
    InputController::release();
    
    // Dispose carousel
    if (_carousel != nullptr) {
        _carousel->dispose();
        _carousel = nullptr;
    }
    
    // Clear all resources
    _font = nullptr;
    _scene = nullptr;
    _batch = nullptr;
    _assets = nullptr;
    
    Application::onShutdown();
}

#pragma mark -
#pragma mark Scene Building

void FidgetApp::buildScene() {
    // Get display size and calculate scale
    Size displaySize = getDisplaySize();
    _screenToSceneScale = GAME_HEIGHT / displaySize.height;
    Size scaledSize = displaySize * _screenToSceneScale;
    
    // Create the carousel controller with the scale factor
    _carousel = SwipeCarouselController::alloc(_scene, scaledSize, _font, _screenToSceneScale);
    
    // Activate button inputs after adding to scene
    _carousel->activateInputs();
    
    CULog("Carousel initialized with page size: %s", scaledSize.toString().c_str());
    CULog("Screen to scene scale: %f", _screenToSceneScale);
}

#pragma mark -
#pragma mark Game Loop

void FidgetApp::update(float timestep) {
    // Update the InputController first (processes input state)
    InputController* input = InputController::getInstance();
    input->update(timestep);
    
    // Update the carousel (uses InputController for input)
    if (_carousel != nullptr) {
        _carousel->update(timestep);
    }
    
    // Clear interaction flags at the end of the frame
    input->clearInteractionFlags();
}

void FidgetApp::draw() {
    // Render the scene graph
    _scene->render();
}
