#ifndef FAITHFUL_SRC_EXECUTORS_LEADTHREAD_H_
#define FAITHFUL_SRC_EXECUTORS_LEADTHREAD_H_

#include <iostream> // todo: replace
#include <memory>
#include <queue>

#include "../io/Window.h"
#include "../io/Camera.h"
#include "../io/Cursor.h"

#include "../../assets/embedded/CursorMainMenu.h"
#include "../../assets/embedded/CursorMainGame.h"

#include "../loader/Texture.h"
#include "../loader/ShaderProgram.h"

#include "../common/GlobalStateInfo.h"

#include "../common/FrameRate.h"

#include "../../utils/Function.h"

#include "../gui/HudPreset.h"

namespace faithful {
namespace details {

class DrawManager;
class InputManager;

class AudioContext;

void GlfwWindowCloseCallback(GLFWwindow* window);
void GlfwErrorCallback(int error, const char *description);
void GlfwFramebufferSizeCallback(GLFWwindow* window, int width, int height);

/// TODO: explain GLFW / contention problem
/// in charge of Render / input / Audio processing
class LeadThread {
 public:
  LeadThread() = delete;
  LeadThread(DrawManager* draw_manager,
             InputManager* input_manager,
             AudioContext* audio_context,
             std::queue<folly::Function<void()>>& task_queue);

  void Run();

 protected:
  friend class ExecutionEnvironment;
  void SetGlfwWindowUserPointer(void* data);
  void UnSetGlfwWindowUserPointer();

 private:
  enum class ProcessingState {
    kMenu,
    kGame,
    kJoined
  };

  // TODO: explain this 1000iq high-brain supremacy domination move
  //  (because Window, Cursor, Camera
  //  don't have default ctor and all should be aware of window and
  //  also glfwInit() (because window can't be created without glfwInit()))
  //  -- and the same "issue" with ExecutionEnvironment() default ctor
  struct GlfwInitializer {
    GlfwInitializer();
    ~GlfwInitializer(); // glfw deinit at class dtor
  };

  void Init();
  void InitPickingFramebuffer();
  void InitButtons();
  void InitButtonsPicking();
  void InitFonts();

  // TODO: explain each there

  void InitScreenMenuMain();

  void InitScreenMenuNewGame();

  void InitScreenMenuLoadGame();

  /**collection of records or data, which could include both items and mobs.
   *
   *
   * */
  void InitScreenMenuArchive();

  void InitScreenMenuOptions();

  void InitScreenMenuOptionsGeneral();

  void InitScreenMenuOptionsInterface();

  void InitScreenMenuOptionsAudio();

  void InitScreenMenuOptionsVideo();

  void InitScreenMenuOptionsControls();

  void InitScreenMenuCredits();

  void InitScreenMenuQuit();

  /// no InitScreenMenuContinue - just load

  void DrawButtons();
  void DrawButtonsPicking();

  void DrawText();


  ProcessingState processing_state_;

  GlfwInitializer glfw_initializer_;

  io::Window window_;

  Camera camera_game_;

  faithful::details::io::Cursor cursor_arrow_;
  faithful::details::io::Cursor cursor_target_;

  faithful::details::io::Cursor* current_cursor_;

  DrawManager* draw_manager_;
  InputManager* input_manager_;

  AudioContext* audio_context_;

  std::queue<folly::Function<void()>>& global_task_queue_;

  /// We don't use our own task_queue_
  // NOT using Base::task_queue_;

  Texture button_texture_;
  ShaderProgram button_shader_program_;
  GLuint button_vao_;
  ShaderProgram button_picking_shader_program_;
  /// reuse the same button_vao_, just not using text_coords

  Texture menu_button_font_texture_;
  Texture menu_description_font_texture_;
  Texture menu_version_copyright_font_texture_;
  Texture game_font_texture_;
  Texture game_storytelling_font_texture_;

  ShaderProgram text_shader_program_;

  struct MenuHud {
    std::unique_ptr<HudPreset> main_;
    std::unique_ptr<HudPreset> new_game_;
    std::unique_ptr<HudPreset> load_game_;
    std::unique_ptr<HudPreset> archive_;
    std::unique_ptr<HudPreset> options_;
    std::unique_ptr<HudPreset> options_general_;
    std::unique_ptr<HudPreset> options_interface_;
    std::unique_ptr<HudPreset> options_audio_;
    std::unique_ptr<HudPreset> options_video_;
    std::unique_ptr<HudPreset> options_controls_;
    std::unique_ptr<HudPreset> credits_;
    std::unique_ptr<HudPreset> quit_;
  };

  MenuHud menu_hud_;
  HudPreset* cur_hud_preset_ = menu_hud_.main_.get();

  Framerate framerate_; // TODO: integrate
  bool need_to_update_monitor_ = false;
};

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_EXECUTORS_LEADTHREAD_H_
