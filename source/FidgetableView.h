//
//  FidgetableView.h
//  Tuggle
//
//  Base class for all Fidgetable toys in the carousel.
//  Each Fidgetable is a page containing an interactive circle button
//  with a label below it.
//
//  EXPANSION NOTES:
//  - Override update() to add custom animation/behavior
//  - Override onPressed() to handle custom interaction
//  - Add member variables for toy-specific state
//  - The circle node can be replaced with custom graphics
//

#ifndef __FIDGETABLE_VIEW_H__
#define __FIDGETABLE_VIEW_H__

#include <cugl/cugl.h>

/**
 * Base class for a single Fidgetable toy in the carousel.
 * 
 * Each FidgetableView represents one "page" in the horizontal carousel.
 * It contains:
 *   - A circular button that can be pressed
 *   - A label showing the fidgetable's name
 *   - Methods for update, rendering, and input handling
 * 
 * To create a new toy type:
 *   1. Subclass FidgetableView
 *   2. Override buildContent() to add custom visuals
 *   3. Override onPressed() to handle interaction
 *   4. Override update() for animations
 */
class FidgetableView {
protected:
    /** The root node containing all visual elements for this fidgetable */
    std::shared_ptr<cugl::scene2::SceneNode> _rootNode;
    
    /** The interactive circle button */
    std::shared_ptr<cugl::scene2::Button> _circleButton;
    
    /** The circle visual node (drawn as filled circle) */
    std::shared_ptr<cugl::scene2::PolygonNode> _circleNode;
    
    /** The label below the circle */
    std::shared_ptr<cugl::scene2::Label> _label;
    
    /** The index/ID of this fidgetable (1-8) */
    int _index;
    
    /** The name displayed in the label */
    std::string _name;
    
    /** Whether this fidgetable is currently active (centered in carousel) */
    bool _isActive;
    
    /** The size of the page (typically screen size) */
    cugl::Size _pageSize;
    
    /** The radius of the circle button */
    float _circleRadius;
    
    /** Reference to font for creating label */
    std::shared_ptr<cugl::graphics::Font> _font;
    
    /**
     * Creates the visual content for this fidgetable.
     * Override in subclasses to customize appearance.
     */
    virtual void buildContent();
    
    /**
     * Creates a filled circle polygon for the button.
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
     * 
     * @param index     The index of this fidgetable (1-8)
     * @param pageSize  The size of each page in the carousel
     * @param font      The font to use for the label
     * @return true if initialization was successful
     */
    virtual bool init(int index, const cugl::Size& pageSize, 
                      std::shared_ptr<cugl::graphics::Font> font);
    
    /**
     * Static allocator for FidgetableView.
     * 
     * @param index     The index of this fidgetable (1-8)
     * @param pageSize  The size of each page in the carousel
     * @param font      The font to use for the label
     * @return A newly allocated FidgetableView
     */
    static std::shared_ptr<FidgetableView> alloc(int index, 
                                                  const cugl::Size& pageSize,
                                                  std::shared_ptr<cugl::graphics::Font> font);
    
    /**
     * Disposes of all resources used by this fidgetable.
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
     * Called when the circle button is pressed.
     * Override in subclasses for custom interaction.
     */
    virtual void onPressed();
    
    /**
     * Returns the root scene node for this fidgetable.
     * Add this to your scene graph to display the fidgetable.
     * 
     * @return The root scene node
     */
    std::shared_ptr<cugl::scene2::SceneNode> getNode() const { return _rootNode; }
    
    /**
     * Sets whether this fidgetable is currently active (centered).
     * Only the active fidgetable should receive input.
     * 
     * @param active    Whether this fidgetable is active
     */
    void setActive(bool active);
    
    /**
     * Returns whether this fidgetable is currently active.
     * 
     * @return true if active
     */
    bool isActive() const { return _isActive; }
    
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
     * Activates the button for input processing.
     * Must be called after adding to scene.
     */
    void activateButton();
    
    /**
     * Deactivates the button from input processing.
     */
    void deactivateButton();
};

#endif /* __FIDGETABLE_VIEW_H__ */

