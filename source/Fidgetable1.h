//
//  Fidgetable1.h
//  Tuggle
//
//  First fidgetable toy - currently a placeholder circle.
//  
//  EXPANSION NOTES:
//  This class is set up for future customization. To make this toy unique:
//  1. Override buildContent() to create custom visuals
//  2. Override update() to add animations
//  3. Override onPressed() for custom interaction
//  4. Add member variables for toy-specific state
//

#ifndef __FIDGETABLE_1_H__
#define __FIDGETABLE_1_H__

#include "FidgetableView.h"

/**
 * First fidgetable toy in the carousel.
 * 
 * Currently uses the default circle implementation from FidgetableView.
 * Customize this class to create a unique interactive toy.
 */
class Fidgetable1 : public FidgetableView {
protected:
    // Add toy-specific member variables here
    // Example: animation state, interaction counters, etc.
    
public:
    /**
     * Creates an uninitialized Fidgetable1.
     */
    Fidgetable1() : FidgetableView() {}
    
    /**
     * Destroys this Fidgetable1.
     */
    virtual ~Fidgetable1() {}
    
    /**
     * Initializes this fidgetable.
     */
    bool init(int index, const cugl::Size& pageSize,
              std::shared_ptr<cugl::graphics::Font> font) override {
        return FidgetableView::init(index, pageSize, font);
    }
    
    /**
     * Static allocator for Fidgetable1.
     */
    static std::shared_ptr<Fidgetable1> alloc(const cugl::Size& pageSize,
                                               std::shared_ptr<cugl::graphics::Font> font) {
        std::shared_ptr<Fidgetable1> result = std::make_shared<Fidgetable1>();
        if (result->init(1, pageSize, font)) {
            return result;
        }
        return nullptr;
    }
    
    /**
     * Updates this fidgetable.
     * Override to add custom animation.
     */
    void update(float timestep) override {
        FidgetableView::update(timestep);
        // TODO: Add custom animation here
    }
    
    /**
     * Called when the toy is pressed.
     * Override to add custom interaction.
     */
    void onPressed() override {
        FidgetableView::onPressed();
        // TODO: Add custom interaction here
    }
};

#endif /* __FIDGETABLE_1_H__ */

