//
//  Fidgetable6.h
//  Tuggle
//
//  Sixth fidgetable toy - currently a placeholder circle.
//

#ifndef __FIDGETABLE_6_H__
#define __FIDGETABLE_6_H__

#include "FidgetableView.h"

/**
 * Sixth fidgetable toy in the carousel.
 */
class Fidgetable6 : public FidgetableView {
public:
    Fidgetable6() : FidgetableView() {}
    virtual ~Fidgetable6() {}
    
    bool init(int index, const cugl::Size& pageSize,
              std::shared_ptr<cugl::graphics::Font> font) override {
        return FidgetableView::init(index, pageSize, font);
    }
    
    static std::shared_ptr<Fidgetable6> alloc(const cugl::Size& pageSize,
                                               std::shared_ptr<cugl::graphics::Font> font) {
        std::shared_ptr<Fidgetable6> result = std::make_shared<Fidgetable6>();
        if (result->init(6, pageSize, font)) {
            return result;
        }
        return nullptr;
    }
    
    void update(float timestep) override {
        FidgetableView::update(timestep);
    }
    
    void onPressed() override {
        FidgetableView::onPressed();
    }
};

#endif /* __FIDGETABLE_6_H__ */

