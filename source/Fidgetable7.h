//
//  Fidgetable7.h
//  Tuggle
//
//  Seventh fidgetable toy - currently a placeholder circle.
//

#ifndef __FIDGETABLE_7_H__
#define __FIDGETABLE_7_H__

#include "FidgetableView.h"

/**
 * Seventh fidgetable toy in the carousel.
 */
class Fidgetable7 : public FidgetableView {
public:
    Fidgetable7() : FidgetableView() {}
    virtual ~Fidgetable7() {}
    
    bool init(int index, const cugl::Size& pageSize,
              std::shared_ptr<cugl::graphics::Font> font) override {
        return FidgetableView::init(index, pageSize, font);
    }
    
    static std::shared_ptr<Fidgetable7> alloc(const cugl::Size& pageSize,
                                               std::shared_ptr<cugl::graphics::Font> font) {
        std::shared_ptr<Fidgetable7> result = std::make_shared<Fidgetable7>();
        if (result->init(7, pageSize, font)) {
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

#endif /* __FIDGETABLE_7_H__ */

