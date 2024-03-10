#include "LeadThread.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "../common/DrawManager.h"
#include "../common/InputManager.h"

#include "ExecutionEnvironment.h"

#include "../../config/MainMenuLayout.h"

// TODO: include & integrate AufioContext

namespace faithful {
namespace details {

void GlfwWindowCloseCallback(GLFWwindow* window) {
  void* data = glfwGetWindowUserPointer(window);
  reinterpret_cast<GlobalStateInfo*>(data)
      ->execution_environment->Join();
}

void GlfwErrorCallback(int error __attribute__((unused)),
                       const char *description) {
  fprintf(stderr, "Error: %s\n", description);
}

void GlfwFramebufferSizeCallback(GLFWwindow* window __attribute__((unused)),
                                 int width, int height) {
  glViewport(0, 0, width, height);
}

LeadThread::GlfwInitializer::GlfwInitializer() {
  if (!glfwInit()) {
    std::cerr << "glfw init error" << std::endl;
    std::terminate();
  }
  glfwSetErrorCallback(GlfwErrorCallback);
  // TODO: set OpenGL & GLFW error callbacks depends on FAITHFUL_DEBUG / ext
}
LeadThread::GlfwInitializer::~GlfwInitializer() {
  glfwTerminate();
}

void LeadThread::SetGlfwWindowUserPointer(void* data) {
  glfwSetWindowUserPointer(window_.Glfw(), data);
}
void LeadThread::UnSetGlfwWindowUserPointer() {
  glfwSetWindowUserPointer(window_.Glfw(), nullptr);
}

/// such weird constructor because Cursor and Camera classes have
/// explicitly deleted ctor (there's no sense to use it) and
/// should be initialized with class faithful::Window
LeadThread::LeadThread(DrawManager* draw_manager,
                       InputManager* input_manager,
                       AudioContext* audio_context,
                       std::queue<folly::Function<void()>>& task_queue)
    : glfw_initializer_(), // glfw initialized here
      window_(),
      camera_game_(window_.GetResolution()),
      cursor_arrow_(faithful::embedded::kCursorMainMenuData,
                    faithful::embedded::kCursorMainMenuWidth,
                    faithful::embedded::kCursorMainMenuHeight),
      cursor_target_(faithful::embedded::kCursorMainGameData,
                     faithful::embedded::kCursorMainGameWidth,
                     faithful::embedded::kCursorMainGameHeight),
      current_cursor_(&cursor_arrow_),
      draw_manager_(draw_manager),
      input_manager_(input_manager),
      audio_context_(audio_context),
      global_task_queue_(task_queue) {
  Init();
}

void LeadThread::Init() {
  /// set GlfwMonitorUserPointer for getting info about monitor connect/disconnect
  window_.GetMonitorInfoRef()
      .InitUpdateRef(reinterpret_cast<void*>(&need_to_update_monitor_));
  glfwSetWindowCloseCallback(window_.Glfw(), GlfwWindowCloseCallback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    std::terminate();
  }
  glfwSetFramebufferSizeCallback(window_.Glfw(), GlfwFramebufferSizeCallback);

  glClearColor(0.2, 0.2, 0.2, 1.0);

  InitScreenMenuMain();
  InitScreenMenuNewGame();
  InitScreenMenuLoadGame();
  InitScreenMenuArchive();
  InitScreenMenuOptions();
  InitScreenMenuOptionsGeneral();
  InitScreenMenuOptionsInterface();
  InitScreenMenuOptionsAudio();
  InitScreenMenuOptionsVideo();
  InitScreenMenuOptionsControls();
  InitScreenMenuCredits();
  InitScreenMenuQuit();

  // TODO 1: we don't need vector there because we know exactly about their amount
  // TODO 2: we also know about total memory usage, so we can just allocate it at once
}

/*TODO:
 * now there's no "managers" and channels between them, we're using
 * task-based multithreading. But we still need to separate modules, so
 * CollisionManager -> CollisionModule - holds only BVH data and provides
 * functions to check collisions.
 *
 * Delete GlobalStateInfo, but still need to somehow handle glfwWindowShouldClose()
 *
 * */


void MenuKeyCallback(GLFWwindow* window, int key, int scancode,
                  int action, int mods) {
  if ((key == GLFW_KEY_ENTER || key == GLFW_KEY_SPACE) &&
      action == GLFW_PRESS) {
    // call action of cur_chosen_hud_button for cur_global_button_list
  }
  if (action == GLFW_REPEAT) {
    // TODO: return just GLFW_PRESS (that's for fast button switching)
    //  (convert GLFW_REPEAT to just GLFW_PRESS)
  }
}

void MouseButtonCallback(GLFWwindow* window, int button,
                         int action, int mods) {
  if (action == GLFW_PRESS) {
    /// doesn't matter which button pressed
    // TODO:
    //  __IF cur_hud::type is slider - we change cur_global_button_list
    //  to empty (we don't need collision checking there) and
    //  set cur_chosen_hud_button to that slider
    //  __ELSE call action of cur_chosen_hud_button for cur_global_button_list
  } else if (action == GLFW_RELEASE) { // only for slider
    // TODO:
    //  __IF cur_hud::type is slider - we change cur_global_button_list
    //  to not-empty (we need collision checking there) and
    //  unset cur_chosen_hud_button from that slider to kNone
  }
}

void ScrollCallback(GLFWwindow* window,
                    double xoffset, double yoffset) {
  // call "SCROLL" of cur_chosen_hud_button for cur_global_button_list
  // should be NOT SLIDER but area for that slider
}

// global



// TODO: interesting feature - frozen cursor - it position changes not
//  by player  but bind to map_coordinate / enemy

// TODO: can play both with mouse and without by using right keyboard half:
//  then we'll have UIO JKL M,. instead of mouse position
//  <-- as a separate config in settings. WHY do we need it? For my dev comfort)


void LeadThread::Run() {
  // TODO: Poison Pill (task to shared task queue)


  while (!window_.ShouldBeClosed()) {
    if (processing_state_ == ProcessingState::kMenu) {
      glfwSetMouseButtonCallback();
      glfwSetScrollCallback();
      glfwSetKeyCallback();
//      audio_context_->SetBackground(); // TODO:
      // TODO: states
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
    }
    while (processing_state_ == ProcessingState::kMenu) {
      UpdateFramerate();
      glBindFramebuffer(GL_FRAMEBUFFER, picking_framebuffer_);
      DrawButtonsPicking();

      double cursor_pos_x, cursor_pos_y;

      glfwGetCursorPos(window_.Glfw(), &cursor_pos_x, &cursor_pos_y);

      GLuint objectID;
      glReadPixels(static_cast<GLint>(cursor_pos_x),
                   static_cast<GLint>(cursor_pos_y),
                   1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &objectID);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      cur_hud_preset_->SetCurId(objectID);

      audio_context_->Update();

      draw_manager_->Update();
      input_manager_->Update();

      DrawButtons();
      DrawBackground(); // TODO: animated scene from game (wind, tesselation)
      DrawText();
    }
    while (processing_state_ == ProcessingState::kGame) {
      // let's make our inventory dynamic, so again we just rerender it and
      // don't care about other states than collision.
      // TODO: but what to do with __Pause__ screens?
      // BASICALLY: we just don't put any tasks
      draw_manager_->Update();
      input_manager_->Update();
    }
    if (processing_state_ == ProcessingState::kJoined) {
      break;
    }
  }
}

void LeadThread::InitPickingFramebuffer() {
  GLuint framebuffer;
  glGenFramebuffers(1, &framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI,
               window_.GetResolution().x, window_.GetResolution().y,
               0, GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

  // GL version is 4.4 or greater.
  glClearTexImage(texture, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);

  GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, drawBuffers);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cerr << "Error: picking Framebuffer is not complete" << std::endl;
  }
}

void LeadThread::InitButtons() {
  /// init texture
  button_texture_ = texture_pool_.Load("buttons.astc");

  /// init shader
  ShaderObject button_vert_shader = shader_object_pool_.Load("Button.vert");
  ShaderObject button_frag_shader = shader_object_pool_.Load("Button.frag");
  button_shader_program_ = shader_program_pool_.CreateProgram();
  button_shader_program_.AttachShader(button_vert_shader);
  button_shader_program_.AttachShader(button_frag_shader);
  button_shader_program_.Bake();

  /// pos_coord, tex_coord
  float vertices[] = {
      0.5f,  0.5f,   1.0f, 1.0f,
      0.5f, -0.5f,   1.0f, 0.0f,
      -0.5f, -0.5f,   0.0f, 0.0f,
      -0.5f,  0.5f,   0.0f, 1.0f
  };
  unsigned int indices[] = {
      0, 1, 3,
      1, 2, 3
  };
  unsigned int vbo, ebo;
  glGenVertexArrays(1, &button_vao_);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(button_vao_);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glDeleteBuffers(1, &vbo);

  glBindVertexArray(0);
}

void LeadThread::InitButtonsPicking() {
  /// init shader
  ShaderObject button_picking_vert_shader = shader_object_pool_.Load("ButtonPicking.vert");
  ShaderObject button_picking_frag_shader = shader_object_pool_.Load("ButtonPicking.frag");
  button_picking_shader_program_ = shader_program_pool_.CreateProgram();
  button_picking_shader_program_.AttachShader(button_picking_vert_shader);
  button_picking_shader_program_.AttachShader(button_picking_frag_shader);
  button_picking_shader_program_.Bake();
}

void LeadThread::DrawButtons() {
  button_shader_program_.Bind();
  button_shader_program_.SetUniform("tex_button", static_cast<int>(0));
  glActiveTexture(GL_TEXTURE0);
  button_texture_.Bind();
  glBindVertexArray(button_vao_);

  button_shader_program_.SetUniformVec4("color", 1, static_cast<int>(0));
  button_shader_program_.SetUniform("brightness", static_cast<float>(0));
  cur_hud_preset_->Draw(button_shader_program_);

  button_shader_program_.SetUniformVec4("color", 1, static_cast<int>(0));
  button_shader_program_.SetUniform("brightness", static_cast<float>(0));
  cur_hud_preset_->DrawSelected(button_shader_program_);
}

void LeadThread::DrawButtonsPicking() {
  button_picking_shader_program_.Bind();
  glBindVertexArray(button_vao_);
  cur_hud_preset_->DrawPicking(button_picking_shader_program_);
}

void LeadThread::InitFonts() {
  using namespace faithful::config::menu;
  /// load fonts bitmaps
  menu_button_font_texture_ = texture_pool_.Load("menu_button_font.astc");
  glActiveTexture(main_btn_font_texture);
  menu_button_font_texture_.Bind();

  menu_description_font_texture_ = texture_pool_.Load("menu_description_font.astc");
  glActiveTexture(description_font_texture);
  menu_button_font_texture_.Bind();

  menu_version_copyright_font_texture_ = texture_pool_.Load("menu_version_copyright_font.astc");
  glActiveTexture(version_copyright_font_texture);
  menu_button_font_texture_.Bind();

  game_font_texture_ = texture_pool_.Load("game_font.astc");
  glActiveTexture(game_default_font_texture);
  menu_button_font_texture_.Bind();

  game_storytelling_font_texture_ = texture_pool_.Load("game_storytelling_font.astc");
  glActiveTexture(game_storytelling_font_texture);
  menu_button_font_texture_.Bind();

  /// init shader
  ShaderObject button_vert_shader = shader_object_pool_.Load("Text.vert");
  ShaderObject button_frag_shader = shader_object_pool_.Load("Text.frag");
  text_shader_program_ = shader_program_pool_.CreateProgram();
  text_shader_program_.AttachShader(button_vert_shader);
  text_shader_program_.AttachShader(button_frag_shader);
  text_shader_program_.Bake();

  // TODO: set default tex_coord and in DrawText() simply SetUniform("offset", 0.0f, 0.5f)
  //  we just locate them close to each other
}

void LeadThread::DrawText() {
  using namespace faithful::config::menu;
  /// bind only once for all text
  text_shader_program_.Bind();
  glBindVertexArray(text_vao_);
  /// the same texture and tex_coord
  text_shader_program_.SetUniform2v("tex_background", texcoord_offset);
  text_shader_program_.SetUniform4v("in_coord", static_cast<int>(0));

  /// draw all text for buttons
  text_shader_program_.SetUniform(
      "tex_bitmap", static_cast<int>(main_btn_font_texture - GL_TEXTURE0));
  text_shader_program_.SetUniform4v("tex_color", tex_color);
  text_shader_program_.SetUniform2v("tex_brightness", tex_brightness);
  cur_hud_preset_->DrawText(text_shader_program_);

  text_shader_program_.SetUniform2v("tex_brightness", tex_brightness);
  cur_hud_preset_->DrawSelectedText(text_shader_program_);

  /// draw description of a current button
  text_shader_program_.SetUniform(
      "tex_bitmap", static_cast<int>(description_font_texture - GL_TEXTURE0));
  text_shader_program_.SetUniform4v("tex_color", tex_color);
  text_shader_program_.SetUniform2v("tex_brightness", tex_brightness);
  cur_hud_preset_->DrawDescription(text_shader_program_, position); // TODO: position

  /// draw game version & copyrights
  text_shader_program_.SetUniform(
      "tex_bitmap", static_cast<int>(version_copyright_font_texture - GL_TEXTURE0));
  text_shader_program_.SetUniform4v("tex_color", tex_color);
  text_shader_program_.SetUniform2v("tex_brightness", tex_brightness);
  // TODO: draw to draw version & copyright
}


void LeadThread::InitScreenMenuMain() {
  auto btn_new_game = std::make_unique<Button>();
  auto btn_load_game = std::make_unique<Button>();
  auto btn_continue = std::make_unique<Button>();
  auto btn_options = std::make_unique<Button>();
  auto btn_archive = std::make_unique<Button>();
  auto btn_credits = std::make_unique<Button>();
  auto btn_quit = std::make_unique<Button>();

  /// rather an area, not button;
  /// serves as a default cursor pos
  auto btn_none = std::make_unique<Button>();

  btn_new_game->SetEffect([&](int key) {
    if (key == GLFW_KEY_ENTER || key == GLFW_KEY_SPACE) {
      cur_hud_preset_ = menu_hud_.new_game_.get();
    } else if (key == GLFW_KEY_DOWN || key == GLFW_KEY_S) {
      auto center = btn_load_game.get()->GetCenter();
      glfwSetCursorPos(window_.Glfw(), center.x, center.y);
    } else if (key == GLFW_KEY_UP || key == GLFW_KEY_W) {
      auto center = btn_none.get()->GetCenter();
      glfwSetCursorPos(window_.Glfw(), center.x, center.y);
    }
  });
  btn_new_game->SetTextureCoord();
  btn_new_game->TranslateTo(); // TODO: relative to screen resolution
  btn_new_game->ScaleTo(0.05f);
  btn_new_game->SetText("New Game");
  btn_new_game->SetDescription("- start new game -");


  menu_hud_.main_.get()->AddButton(std::move(btn_new_game));
  menu_hud_.main_.get()->AddButton(std::move(btn_load_game));
  menu_hud_.main_.get()->AddButton(std::move(btn_continue));
  menu_hud_.main_.get()->AddButton(std::move(btn_options));
  menu_hud_.main_.get()->AddButton(std::move(btn_archive));
  menu_hud_.main_.get()->AddButton(std::move(btn_credits));
  menu_hud_.main_.get()->AddButton(std::move(btn_quit));
}

void LeadThread::InitScreenMenuNewGame() {}

void LeadThread::InitScreenMenuLoadGame() {}

void LeadThread::InitScreenMenuArchive() {}

void LeadThread::InitScreenMenuOptions() {}

void LeadThread::InitScreenMenuOptionsGeneral() {}

void LeadThread::InitScreenMenuOptionsInterface() {}

void LeadThread::InitScreenMenuOptionsAudio() {}

void LeadThread::InitScreenMenuOptionsVideo() {}

void LeadThread::InitScreenMenuOptionsControls() {}

void LeadThread::InitScreenMenuCredits() {}

void LeadThread::InitScreenMenuQuit() {}

} // namespace details
} // namespace faithful
