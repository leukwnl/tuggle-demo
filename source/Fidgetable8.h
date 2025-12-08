//
//  Fidgetable8.h
//  Tuggle
//
//  Eighth fidgetable toy - currently a placeholder circle.
//

#ifndef __FIDGETABLE_8_H__
#define __FIDGETABLE_8_H__

#include "FidgetableView.h"

/**
 * Eighth fidgetable toy in the carousel.
 */
class Fidgetable8 : public FidgetableView {
public:
    Fidgetable8() : FidgetableView() {}
    virtual ~Fidgetable8() {}
    
    bool init(int index, const cugl::Size& pageSize,
              std::shared_ptr<cugl::graphics::Font> font) override {
        return FidgetableView::init(index, pageSize, font);
    }
    
    static std::shared_ptr<Fidgetable8> alloc(const cugl::Size& pageSize,
                                               std::shared_ptr<cugl::graphics::Font> font) {
        std::shared_ptr<Fidgetable8> result = std::make_shared<Fidgetable8>();
        if (result->init(8, pageSize, font)) {
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

#endif /* __FIDGETABLE_8_H__ */

