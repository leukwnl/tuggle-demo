//
//  FidgetableView.h
//  Tuggle
//
//  Abstract base class for all Fidgetable toys in the carousel.
//  Each Fidgetable is a page that can contain any interactive content.
//
//  This base class provides:
//  - Root scene node and page size management
//  - Active state tracking (only centered page receives input)
//  - Helper utilities for creating common shapes
//
//  Subclasses must implement:
//  - buildContent() to create their specific UI elements
//  - activateInputs() / deactivateInputs() to manage their input listeners
//  - setActive() to handle visual changes when becoming active/inactive
//

#ifndef __FIDGETABLE_VIEW_H__
#define __FIDGETABLE_VIEW_H__

#include <cugl/cugl.h>

/**
 * Abstract base class for a single Fidgetable toy in the carousel.
 * 
 * Each FidgetableView represents one "page" in the horizontal carousel.
 * The base class manages the root node and page dimensions, but does not
 * assume any specific UI elements - subclasses define their own content.
 * 
 * To create a new toy type:
 *   1. Subclass FidgetableView
 *   2. Implement buildContent() to create custom visuals
 *   3. Implement activateInputs() / deactivateInputs() for input handling
 *   4. Override setActive() for active/inactive visual changes
 *   5. Override update() for animations
 */
class FidgetableView {
protected:
    /** The root node containing all visual elements for this fidgetable */
    std::shared_ptr<cugl::scene2::SceneNode> _rootNode;
    
    /** The index/ID of this fidgetable (1-8) */
    int _index;
    
    /** The name displayed in the label */
    std::string _name;
    
    /** Whether this fidgetable is currently active (centered in carousel) */
    bool _isActive;
    
    /** Whether this fidgetable is currently being interacted with (blocks carousel swiping) */
    bool _isInteracting;
    
    /** The size of the page (typically screen size) */
    cugl::Size _pageSize;
    
    /**
     * Creates the visual content for this fidgetable.
     * Subclasses MUST implement this to build their specific UI.
     */
    virtual void buildContent() = 0;
    
    /**
     * Creates a filled circle polygon node.
     * Utility method available to all subclasses.
     * 
     * @param radius    The radius of the circle
     * @param color     The fill color
     * @param segments  Number of segments for circle approximation
     * @return A polygon node representing the circle
     */
    std::shared_ptr<cugl::scene2::PolygonNode> createCircle(float radius, 
                                                             cugl::Color4 color, 
                                                             int segments = 64);
    
public:
    /**
     * Creates an uninitialized FidgetableView.
     * You must call init() before using this object.
     */
    FidgetableView();
    
    /**
     * Destroys the FidgetableView, releasing all resources.
     */
    virtual ~FidgetableView();
    
    /**
     * Initializes the FidgetableView with the given parameters.
     * Creates the root node and calls buildContent().
     * 
     * @param index     The index of this fidgetable (1-8)
     * @param pageSize  The size of each page in the carousel
     * @return true if initialization was successful
     */
    virtual bool init(int index, const cugl::Size& pageSize);
    
    /**
     * Disposes of all resources used by this fidgetable.
     * Subclasses should override to clean up their specific resources.
     */
    virtual void dispose();
    
    /**
     * Updates the fidgetable state.
     * Override in subclasses for custom animation/behavior.
     * 
     * @param timestep  Time elapsed since last frame (in seconds)
     */
    virtual void update(float timestep);
    
    /**
     * Returns the root scene node for this fidgetable.
     * Add this to your scene graph to display the fidgetable.
     * 
     * @return The root scene node
     */
    std::shared_ptr<cugl::scene2::SceneNode> getNode() const { return _rootNode; }
    
    /**
     * Sets whether this fidgetable is currently active (centered).
     * Subclasses should override to update visuals (e.g., dim inactive toys).
     * 
     * @param active    Whether this fidgetable is active
     */
    virtual void setActive(bool active);
    
    /**
     * Returns whether this fidgetable is currently active.
     * 
     * @return true if active
     */
    bool isActive() const { return _isActive; }
    
    /**
     * Returns whether this fidgetable is currently being interacted with.
     * When true, carousel swiping should be suspended.
     * 
     * @return true if user is interacting with this fidgetable
     */
    bool isInteracting() const { return _isInteracting; }
    
    /**
     * Returns the index of this fidgetable.
     * 
     * @return The index (1-8)
     */
    int getIndex() const { return _index; }
    
    /**
     * Returns the name of this fidgetable.
     * 
     * @return The display name
     */
    const std::string& getName() const { return _name; }
    
    /**
     * Activates input listeners for this fidgetable.
     * Subclasses MUST implement to activate their specific buttons/controls.
     * Called after adding to scene.
     */
    virtual void activateInputs() = 0;
    
    /**
     * Deactivates input listeners for this fidgetable.
     * Subclasses MUST implement to deactivate their specific buttons/controls.
     */
    virtual void deactivateInputs() = 0;
};

#endif /* __FIDGETABLE_VIEW_H__ */
