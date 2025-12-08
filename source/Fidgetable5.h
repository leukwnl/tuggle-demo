//
//  Fidgetable5.h
//  Tuggle
//
//  Fifth fidgetable toy - currently a placeholder circle.
//

#ifndef __FIDGETABLE_5_H__
#define __FIDGETABLE_5_H__

#include "FidgetableView.h"

/**
 * Fifth fidgetable toy in the carousel.
 */
class Fidgetable5 : public FidgetableView {
public:
    Fidgetable5() : FidgetableView() {}
    virtual ~Fidgetable5() {}
    
    bool init(int index, const cugl::Size& pageSize,
              std::shared_ptr<cugl::graphics::Font> font) override {
        return FidgetableView::init(index, pageSize, font);
    }
    
    static std::shared_ptr<Fidgetable5> alloc(const cugl::Size& pageSize,
                                               std::shared_ptr<cugl::graphics::Font> font) {
        std::shared_ptr<Fidgetable5> result = std::make_shared<Fidgetable5>();
        if (result->init(5, pageSize, font)) {
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

#endif /* __FIDGETABLE_5_H__ */

