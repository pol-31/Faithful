#ifndef FAITHFUL_SRC_GUI_GUI_H
#define FAITHFUL_SRC_GUI_GUI_H

#include <map>

#include <glm/glm.hpp>

#include "../collision/Shapes.h"
#include "../common/IdManager.h"
#include "../loader/Texture.h"
#include "../loader/ShaderProgram.h"
#include "../../utils/Function.h"

/// collision detection - fully by render picking from Depth-buffer
namespace faithful {

/*Example for Pasha, what effect to set:
void Use(int action) {
  if (action == GLFW_KEY_ENTER ||
      action == GLFW_KEY_SPACE) {
    // run pressing animation
    // do something
  }
}

OR

        RowSwitchButton / ColumnSwitchButton:

    void Use(int action) {
  if (action == GLFW_KEY_RIGHT ||
      action == GLFW_KEY_F) {
    // switch state to prev
  }
  if (action == GLFW_KEY_LEFT ||
      action == GLFW_KEY_A) {
    // switch state to next
  }
}
* */

// TODO: do we really need V-tables?
class Button {
 public:
  Button()
      : size_(glm::vec2{0}),
        global_id_(details::id_manager.Acquire()) {}
  virtual ~Button() = default;

  /// not copyable (due to member folly::Function)
  Button(const Button&) = delete;
  Button& operator=(const Button&) = delete;

  /// movable
  Button(Button&&) = default;
  Button& operator=(Button&&) = default;

  void SetSize(float width, float height) {
    size_.max.x = size_.min.x + width;
    size_.max.y = size_.min.y + height;
  }
  void SetBounds(glm::vec2 min, glm::vec2 max) {
    size_.min = min;
    size_.max = max;
  }
  void SetPosition(glm::vec2 pos) {
    size_.min = pos;
  }

  void SetStartingTexture(Texture texture) {
    texture_starting_ = texture;
  }
  void SetPressingTexture(Texture texture) {
    texture_pressing_ = texture;
  }

  void SetEffect(folly::Function<void(int)> effect) {
    effect_ = std::move(effect);
  }

  virtual void Press(int key) {
    effect_(key);
  }

  virtual void Release() {
    // TODO: need somehow say that we want to draw texture_starting and
    //  not texture_pressing anymore
  }

  /// we could draw all with the same texture & shader at once just by
  /// array of position, but there shouldn't be a lot of keys, so don't care
  virtual void Draw() {
    // TODO:
  }

  /// method const, because internally Texture is just id and std::shared_ptr,
  /// so we simply return its copy
  Texture GetStartingTexture() const {
    return texture_starting_;
  }
  Texture GetPressingTexture() const {
    return texture_pressing_;
  }

  const details::collision::Rectangle& GetBounds() const {
    return size_;
  }
  details::collision::Rectangle GetBounds() {
    return size_;
  }

  int GetId() const {
    return global_id_;
  }

 protected:
  details::collision::Rectangle size_;
  int global_id_;
  Texture texture_starting_;
  Texture texture_pressing_;
  ShaderProgram shader_program_;
  folly::Function<void(int)> effect_; // int means action like GLFW_PRESS_SPACE
};

// TODO: buttons should handle "next" and "prev" on his own, because
//  we can't implement it auto due to RowSwitch / ColumnSwitch (one requires 1,
//  another requires 2.. wtf)

// Mechanics of different buttons implemented via just folly::Function<int action>

// SO our glfw callback only create event after what we should check current
//   button and call its HudComponent::effect_

// TODO:
//  lists prev-cur-next to iterate with keyboard
//  slider

// TODO: list prev-cur-next handled inside the Button::effect_

class CheckBox;
class RadioButton;

class CheckBoxSharedState {
 public:
  CheckBoxSharedState() = default;
  CheckBoxSharedState(int limit)
      : limit_(limit) {
    set_idx_.reserve(limit);
  }

  int BindButton(CheckBox* check_box) {
    check_box->UnSet();
    buttons_.push_back(check_box);
    return buttons_.size() - 1;
  }

  void SetLimit(int limit) {
    limit_ = limit;
    int excessive_num = static_cast<std::size_t>(limit_) - set_idx_.size();
    while (excessive_num > 0) {
      buttons_[set_idx_.back()].UnSet();
      set_idx_.pop_back();
    }
  }

  int GetLimit() const {
    return limit_;
  }

  /// Set() called before; just only UnSet excessive
  void Set(int button_id) {
    if (limit_ == -1 ||
        static_cast<std::size_t>(limit_) < set_idx_.size()) {
      set_idx_.push_back(button_id);
    } else {
      buttons_[set_idx_[limit_ - 1]].UnSet();
      set_idx_[limit_ - 1] - button_id;
    }
  }

  /// UnSet() called before; just only remove from tracking
  void UnSet(int button_id) {
    for (auto it = set_idx_.begin(); it != set_idx_.end(); ++it) {
      if (*it == button_id) {
        set_idx_.erase(it);
        break;
      }
    }
  }

  std::vector<CheckBox*> GetStat() {
    std::vector<CheckBox*> stat;
    stat.reserve(set_idx_.size());
    for (const auto& id : set_idx_) {
      stat.push_back(buttons_[id]);
    }
    return stat;
  }

  bool IsSet(int button_id) {
    for (const auto& id : set_idx_) {
      if (id == button_id) {
        return true;
      }
    }
    return false;
  }

 private:
  std::vector<CheckBox*> buttons_;
  std::vector<int> set_idx_;
  int limit_ = -1;
};

class RadioButtonSharedState {
 public:
  RadioButtonSharedState() = default;

  int BindButton(RadioButton* radio_button) {
    radio_button->UnSet();
    buttons_.push_back(radio_button);
    return buttons_.size() - 1;
  }

  /// Set() called before; just only UnSet excessive
  void Set(int button_id) {
    buttons_[set_idx_].UnSet();
    set_idx_ = button_id;
  }

  RadioButton* GetStat() {
    return buttons_[set_idx_];
  }

  bool IsSet(int button_id) {
    return button_id == set_idx_;
  }

 private:
  std::vector<RadioButton*> buttons_;
  int set_idx_ = 0;
};


class StatefulButton : public Button {
 public:
  StatefulButton() = delete;
  StatefulButton(bool default_state)
      : set_(default_state) {}

  virtual void Set() {
    set_ = true;
  }

  virtual void UnSet() {
    set_ = false;
  }

  void Release() override {
    Button::Release();
    set_ ? Set() : UnSet();
  }

  bool IsSet() {
    return set_;
  }

  void Draw() override {
    // TODO:
  }

 protected:
  bool set_;
};

class CheckBox : public StatefulButton {
 public:
  CheckBox() = delete;
  CheckBox(CheckBoxSharedState* shared_state)
      : StatefulButton(false), shared_state_(shared_state) {
    shared_state_id_ = shared_state->BindButton(this);
  }

  void Set() override {
    StatefulButton::Set();
    shared_state_->Set(shared_state_id_);
  }

  void UnSet() override {
    StatefulButton::UnSet();
    shared_state_->UnSet(shared_state_id_);
  }

 private:
  CheckBoxSharedState* shared_state_;
  int shared_state_id_;
};

class RadioButton : public StatefulButton {
 public:
  RadioButton() = delete;
  RadioButton(RadioButtonSharedState* shared_state)
      : StatefulButton(true), shared_state_(shared_state) {
    shared_state_id_ = shared_state->BindButton(this);
  }

  void Set() override {
    StatefulButton::Set();
    shared_state_->Set(shared_state_id_);
  }

  /// shouldn't have UnSet()
  void UnSet() override {}

 private:
  RadioButtonSharedState* shared_state_;
  int shared_state_id_;
};

///////////////////////
///////////////////////
///////////////////////
///////////////////////
///////////////////////

class HudSetup {
 public:
  HudSetup() = default;

  int AddButton(Button button) {
    int button_id = button.GetId();
    auto found_button = buttons_.find(button_id);
    if (found_button != buttons_.end()) {
      return found_button->second.GetId();
    } else {
      buttons_.insert({button_id, std::move(button)});
      return button_id;
    }
  }

  int SetButtonEffect(int id, folly::Function<void(int)> effect) {
    auto button = buttons_.find(id);
    if (button != buttons_.end()) {
      button->second.SetEffect(std::move(effect));
      return -1;
    } else {
      Button new_button;
      new_button.SetEffect(std::move(effect));
      int new_id = new_button.GetId();
      buttons_.insert({new_id, std::move(new_button)});
      return new_id;
    }
  }

  int SetButtonTexture(int id, Texture texture) {
    auto button = buttons_.find(id);
    if (button != buttons_.end()) {
      button->second.SetTexture(texture);
      return -1;
    } else {
      Button new_button;
      new_button.SetTexture(texture);
      int new_id = new_button.GetId();
      buttons_.insert({new_id, std::move(new_button)});
      return new_id;
    }
  }

  void Press(int id, int key) {
    auto button = buttons_.find(id);
    if (button != buttons_.end()) {
      button->second.Press(key);
    }
  }

 private:
  std::map<int, Button> buttons_;
  int cur_id_;
  int cur_holding_id_; // TODO: integrate
  // TODO: tree with up/down/left/right ptrs LISTS?
};

int PseudoMain() {
  Button new_game;
  Button load;
  Button quit;
//  HudPreset main_menu;
//  main_menu.AddButton(new_game);
//  main_menu.AddButton(load);
//  main_menu.AddButton(quit);
}

}  // namespace faithful

#endif  // FAITHFUL_SRC_GUI_GUI_H
