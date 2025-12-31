//
//  SettingsScene.cpp
//  TriggerHappy
//
//  Created by Pedro on 5/15/25.
//

#include "SettingsScene.h"

#include <cugl/cugl.h>

#include <iostream>
#include <sstream>

#include "AudioController.h"
#include "DeviceUtils.h"
#include "InputController.h"

/** For scaling the input properly maybe */
#define SCREEN_TO_SCENE_SCALE (_size / Application::get()->getDisplaySize());

using namespace cugl;
using namespace cugl::scene2;
using namespace std;

#pragma mark -
#pragma mark Level Layout

/** Regardless of logo, lock the height to this */
#define SCENE_HEIGHT 1080
#define SQUARE_HEIGHT 1440

#pragma mark -
#pragma mark Constructors

bool SettingsScene::init(const std::shared_ptr<cugl::AssetManager> &assets) {
  // Initialize the scene to a locked width
  if (assets == nullptr) {
    return false;
  } else if (!Scene2::initWithHint(Size(
                 0, DeviceUtils::isTablet() ? SQUARE_HEIGHT : SCENE_HEIGHT))) {
    return false;
  }
  _assets = assets;

  Size dimen = getSize();
  std::shared_ptr<scene2::SceneNode> scene =
      _assets->get<scene2::SceneNode>("settings");
  scene->setContentSize(dimen);

  _backout = std::dynamic_pointer_cast<scene2::Button>(
      _assets->get<scene2::SceneNode>("settings.back"));
  _backout->addListener([this](const std::string &name, bool down) {
    if (down) {
      transition->setOut();
      state = SettingsState::BACK;
      AudioController::get()->playRandom(
          vector<string>{"briefcase close 1", "briefcase close 2"});
#if TARGET_OS_IPHONE
      cugl::HapticFeedback::triggerMedium();
#endif
    }
  });
  state = SettingsState::STAY;

  auto localegp = _assets->get<LocaleGroup>("ui");
  _scroll = assets->get<SceneNode>(
      "settings.settingsmenu.ordering.lang.button.drop.contents.scrollable");
  _scroll->getParent()->getParent()->setVisible(false);
  _scroll->setContentSize(570, localegp->getSupportedLangs()->size() * 100);
  for (int i = 0; i < localegp->getSupportedLangs()->size(); i++) {
    auto key = localegp->getSupportedLangs()->get(i)->key();
    auto name = localegp->getSupportedLangs()->getString(key);
    auto text = localegp->getLocalizedString("lang", key);
    unordered_map<string, string> fontMap = {
        {"en_US", "triggerhappyRegular64"},
        {"en_BR", "triggerhappyRegular64"},
        {"en_UW", "triggerhappyRegular64"},
        {"es_ES", "triggerhappyRegular64"},
        {"fr_FR", "triggerhappyRegular64"},
        {"it_IT", "triggerhappyRegular64"},
        {"hi_IN", "noto50"},
        {"ja_JP", "notojapanese50"},
        {"zh_TW", "nototraditional50"},
        {"zh_CN", "notosimplified50"},
        {"he_IL", "notohebrew50"},
        {"ko_KR", "notokorean50"},
        {"ms_MY", "triggerhappyRegular64"},
    };

    string fontName = fontMap.count(key) ? fontMap[key] : "noto50";
    auto font = _assets->get<Font>(fontName);
    auto label = dynamic_pointer_cast<Label>(Label::allocWithText(text, font));
    label->setContentSize(570, 100);
    label->setForeground(Color4("#f0e7c2"));
    label->setHorizontalAlignment(HorizontalAlign::CENTER);
    label->setPriority(300);
    label->setName(name + "-label");
    auto button = dynamic_pointer_cast<Button>(Button::alloc(label));
    button->setContentSize(570, 100);
    button->addListener([this, key](const std::string &name, bool down) {
      if (down) {
        _scroll->getParent()->getParent()->setVisible(false);
        _assets->get<LocaleGroup>("ui")->setLangCode(key);
#if TARGET_OS_IPHONE
        cugl::HapticFeedback::triggerMedium();
#endif
      }
    });
    button->setPriority(350);
    _scroll->getLayout()->add(name, JsonValue::allocObject());
    _scroll->addChildWithName(button, name);
  }
  _scroll->doLayout();
  _scroll->setAnchor(0, 1);
  _scroll->setPositionY(_scroll->getParent()->getContentHeight());
  auto p = _scroll->getParent();
  p->setScissor(Scissor::alloc(
      Rect(0, 20, p->getContentWidth(), p->getContentHeight() - 10)));

  _langbutton = dynamic_pointer_cast<Button>(
      assets->get<SceneNode>("settings.settingsmenu.ordering.lang.button"));
  _langbutton->deactivate();
  _langbutton->addListener([this](const std::string &name, bool down) {
    if (down) {
      _scroll->getParent()->getParent()->setVisible(
          !_scroll->getParent()->getParent()->isVisible());
#if TARGET_OS_IPHONE
      cugl::HapticFeedback::triggerMedium();
#endif
    }
  });

  _musicslider = dynamic_pointer_cast<Slider>(
      assets->get<SceneNode>("settings.settingsmenu.ordering.music.slider"));
  _musicslider->deactivate();
  _musicslider->setValue(_musicslider->getValue());

  _prevMusicValue = _musicslider->getValue();

  // For the music slider
  _musicslider->addListener([this](const std::string &name, float val) {
    // trust me bro
    float musicVolume = (val - 4.0f) / 92.0f;
    musicVolume = max(0.0f, min(1.0f, musicVolume));
    AudioController::get()->setMusicVolume(musicVolume);

    // Only trigger haptics when user is actively interacting with the
    // slider
    auto knob =
        dynamic_pointer_cast<Button>(_musicslider->getChildByName("knob"));
    if (knob && knob->isDown()) {
#if TARGET_OS_IPHONE
      cugl::HapticFeedback::triggerLight();
#endif
    }
  });

  // Add separate listeners for pressing and releasing the knob
  dynamic_pointer_cast<Button>(_musicslider->getChildByName("knob"))
      ->addListener([this](const std::string &name, bool down) {
        if (down) {
// Initial touch - trigger once
#if TARGET_OS_IPHONE
          cugl::HapticFeedback::triggerMedium();
#endif
          _prevMusicValue = _musicslider->getValue();
        } else {
// Release - trigger once
#if TARGET_OS_IPHONE
          cugl::HapticFeedback::triggerLight();
#endif
        }
      });

  _sfxslider = dynamic_pointer_cast<Slider>(
      assets->get<SceneNode>("settings.settingsmenu.ordering.sfx.slider"));
  _sfxslider->deactivate();
  _sfxslider->setValue(_sfxslider->getValue());
  _prevSfxValue = _sfxslider->getValue();
  _sfxslider->addListener([this](const std::string &name, float val) {
    // trust me bro
    float sfxVolume = (val - 4.0f) / 92.0f;
    sfxVolume = max(0.0f, min(1.0f, sfxVolume));
    AudioController::get()->setSfxVolume(sfxVolume);

    // Only trigger haptics when user is actively interacting with the
    // slider
    auto knob =
        dynamic_pointer_cast<Button>(_sfxslider->getChildByName("knob"));
    if (knob && knob->isDown()) {
#if TARGET_OS_IPHONE
      cugl::HapticFeedback::triggerLight();
#endif
    }
  });
  dynamic_pointer_cast<Button>(_sfxslider->getChildByName("knob"))
      ->addListener([this](const std::string &name, bool down) {
        if (down) {
// Initial touch - trigger once
#if TARGET_OS_IPHONE
          cugl::HapticFeedback::triggerMedium();
#endif
          _prevSfxValue = _sfxslider->getValue();
        } else {
// Release - trigger once
#if TARGET_OS_IPHONE
          cugl::HapticFeedback::triggerLight();
#endif
          AudioController::get()->play("briefcase close 1");
        }
      });

  addTransition(_assets, scene);
  addChild(scene);
  setActive(false);
  scene->doLayout();
  return true;
}

void SettingsScene::update(float timestep) {
  auto ic = InputController::getInstance();
  ic->update(timestep);
  auto parent = _scroll->getParent()->getBoundingBox();
  auto tapPt = _scroll->getParent()->screenToNodeCoords(ic->getPosition());
  auto tapStartPt =
      _scroll->getParent()->screenToNodeCoords(ic->getStartPosition());
  if (ic->isDragging() && parent.contains(tapStartPt)) {
    _scroll->setPosition(_scroll->getPosition() +
                         ic->getDelta() *
                             (_size / Application::get()->getDisplaySize()) *
                             Vec2(0, -1));
    if (_scroll->getPositionY() > _scroll->getContentHeight())
      _scroll->setPositionY(_scroll->getContentHeight());
    if (_scroll->getPositionY() < _scroll->getParent()->getContentHeight())
      _scroll->setPositionY(_scroll->getParent()->getContentHeight());
  }
  if (_scroll->getParent()->getParent()->isVisible()) {
    int i = 0;
    for (auto c : _scroll->getChildren()) {
      auto button = dynamic_pointer_cast<Button>(c);
      auto label = dynamic_pointer_cast<Button>(c)->getChild(0);
      c->setAnchor(0, 0);
      auto rect = Rect(0, 100 * i, _scroll->getContentWidth(), 100);
      auto tapSc = _scroll->screenToNodeCoords(ic->getPosition());
      if (ic->didTap() &&
          rect.contains(Vec2(tapSc.x, _scroll->getContentHeight() - tapSc.y)) &&
          parent.contains(tapPt)) {
        button->setDown(true);

      } else {
        button->setDown(false);
      }
      i++;
    }
  } else {
    _scroll->setPositionY(_scroll->getParent()->getContentHeight());
  }
  _musicslider->setValue(min(96.0f, max(4.0f, _musicslider->getValue())));
  _sfxslider->setValue(min(96.0f, max(4.0f, _sfxslider->getValue())));
}

void SettingsScene::dispose() {
  if (_active) {
    removeAllChildren();
    _active = false;
  }
}

void SettingsScene::setActive(bool value) {
  state = SettingsState::STAY;
  _musicslider->setValue(min(96.0f, max(4.0f, _musicslider->getValue())));
  _sfxslider->setValue(min(96.0f, max(4.0f, _sfxslider->getValue())));
  if (isActive() != value) {
    Scene2::setActive(value);
    if (value) {
      transition->setIn();
      _backout->activate();
      _langbutton->activate();
      _musicslider->activate();
      dynamic_pointer_cast<Button>(_musicslider->getChildByName("knob"))
          ->activate();
      _sfxslider->activate();
      dynamic_pointer_cast<Button>(_sfxslider->getChildByName("knob"))
          ->activate();
      for (auto c : _scroll->getChildren()) {
        dynamic_pointer_cast<Button>(c)->activate();
      }
    } else {
      _backout->deactivate();
      _backout->setDown(false);
      _langbutton->deactivate();
      _langbutton->setDown(false);
      _musicslider->deactivate();
      dynamic_pointer_cast<Button>(_musicslider->getChildByName("knob"))
          ->deactivate();
      dynamic_pointer_cast<Button>(_musicslider->getChildByName("knob"))
          ->setDown(false);
      _sfxslider->deactivate();
      dynamic_pointer_cast<Button>(_sfxslider->getChildByName("knob"))
          ->deactivate();
      dynamic_pointer_cast<Button>(_sfxslider->getChildByName("knob"))
          ->setDown(false);
      for (auto c : _scroll->getChildren()) {
        dynamic_pointer_cast<Button>(c)->deactivate();
        dynamic_pointer_cast<Button>(c)->setDown(false);
      }
      auto localegp = _assets->get<LocaleGroup>("ui");
    }
  }
}
