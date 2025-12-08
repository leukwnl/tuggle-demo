//
//  Fidgetable3.h
//  Tuggle
//
//  Third fidgetable toy - currently a placeholder circle.
//

#ifndef __FIDGETABLE_3_H__
#define __FIDGETABLE_3_H__

#include "FidgetableView.h"

/**
 * Third fidgetable toy in the carousel.
 */
class Fidgetable3 : public FidgetableView {
public:
    Fidgetable3() : FidgetableView() {}
    virtual ~Fidgetable3() {}
    
    bool init(int index, const cugl::Size& pageSize,
              std::shared_ptr<cugl::graphics::Font> font) override {
        return FidgetableView::init(index, pageSize, font);
    }
    
    static std::shared_ptr<Fidgetable3> alloc(const cugl::Size& pageSize,
                                               std::shared_ptr<cugl::graphics::Font> font) {
        std::shared_ptr<Fidgetable3> result = std::make_shared<Fidgetable3>();
        if (result->init(3, pageSize, font)) {
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

#endif /* __FIDGETABLE_3_H__ */

