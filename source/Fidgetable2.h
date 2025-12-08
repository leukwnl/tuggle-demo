//
//  Fidgetable2.h
//  Tuggle
//
//  Second fidgetable toy - currently a placeholder circle.
//

#ifndef __FIDGETABLE_2_H__
#define __FIDGETABLE_2_H__

#include "FidgetableView.h"

/**
 * Second fidgetable toy in the carousel.
 */
class Fidgetable2 : public FidgetableView {
public:
    Fidgetable2() : FidgetableView() {}
    virtual ~Fidgetable2() {}
    
    bool init(int index, const cugl::Size& pageSize,
              std::shared_ptr<cugl::graphics::Font> font) override {
        return FidgetableView::init(index, pageSize, font);
    }
    
    static std::shared_ptr<Fidgetable2> alloc(const cugl::Size& pageSize,
                                               std::shared_ptr<cugl::graphics::Font> font) {
        std::shared_ptr<Fidgetable2> result = std::make_shared<Fidgetable2>();
        if (result->init(2, pageSize, font)) {
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

#endif /* __FIDGETABLE_2_H__ */

