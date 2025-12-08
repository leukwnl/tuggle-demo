//
//  Fidgetable4.h
//  Tuggle
//
//  Fourth fidgetable toy - currently a placeholder circle.
//

#ifndef __FIDGETABLE_4_H__
#define __FIDGETABLE_4_H__

#include "FidgetableView.h"

/**
 * Fourth fidgetable toy in the carousel.
 */
class Fidgetable4 : public FidgetableView {
public:
    Fidgetable4() : FidgetableView() {}
    virtual ~Fidgetable4() {}
    
    bool init(int index, const cugl::Size& pageSize,
              std::shared_ptr<cugl::graphics::Font> font) override {
        return FidgetableView::init(index, pageSize, font);
    }
    
    static std::shared_ptr<Fidgetable4> alloc(const cugl::Size& pageSize,
                                               std::shared_ptr<cugl::graphics::Font> font) {
        std::shared_ptr<Fidgetable4> result = std::make_shared<Fidgetable4>();
        if (result->init(4, pageSize, font)) {
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

#endif /* __FIDGETABLE_4_H__ */

