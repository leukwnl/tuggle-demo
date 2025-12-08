//
//  FidgetApp.h
//  Tuggle
//
//  Main application class for the Fidgetable carousel app.
//  Manages the carousel of interactive toys that users can swipe through.
//

#ifndef __FIDGET_APP_H__
#define __FIDGET_APP_H__

#include <cugl/cugl.h>
#include "SwipeCarouselController.h"
#include "InputController.h"

/**
 * Main application class for the Fidgetable carousel.
 * 
 * This application displays a horizontal carousel of 8 fidgetable toys.
 * Users can:
 * - Drag left/right to scroll through toys
 * - Tap the centered toy to interact with it
 * 
 * The app uses CUGL's scene graph system and InputController for unified
 * pointer input handling across desktop (mouse) and mobile (touch) platforms.
 */
class FidgetApp : public cugl::Application {
protected:
    /** The asset manager for loading textures, fonts, etc. */
    std::shared_ptr<cugl::AssetManager> _assets;
    
    /** The scene graph for 2D rendering */
    std::shared_ptr<cugl::scene2::Scene2> _scene;
    
    /** The sprite batch for rendering */
    std::shared_ptr<cugl::graphics::SpriteBatch> _batch;
    
    /** The carousel controller that manages all fidgetables */
    std::shared_ptr<SwipeCarouselController> _carousel;
    
    /** The font used for labels */
    std::shared_ptr<cugl::graphics::Font> _font;
    
    /** Scale factor from screen to scene coordinates */
    float _screenToSceneScale;
    
    /**
     * Builds the scene graph including the carousel.
     */
    void buildScene();
    
public:
    /**
     * Creates an uninitialized FidgetApp.
     */
    FidgetApp() : Application(), _screenToSceneScale(1.0f) {}
    
    /**
     * Destroys the FidgetApp.
     */
    ~FidgetApp() {}
    
    /**
     * Called after OpenGL is initialized, but before running the application.
     * Initializes all resources and builds the scene.
     */
    virtual void onStartup() override;
    
    /**
     * Called when the application is ready to quit.
     * Disposes of all resources.
     */
    virtual void onShutdown() override;
    
    /**
     * Called every frame to update application state.
     * 
     * @param timestep  Time elapsed since last frame (in seconds)
     */
    virtual void update(float timestep) override;
    
    /**
     * Called every frame to render the application.
     */
    virtual void draw() override;
};

#endif /* __FIDGET_APP_H__ */
