#include "RenderThreadPool.h"

#include "../../utils/Logger.h"

#include "queues/LifoBoundedMPSCBlockingQueue.h"

namespace faithful {
namespace details {

// TODO: .h
static void error_callback(int error, const char *description) {
  (void)error;
  fprintf(stderr, "Error: %s\n", description);
}


RenderThreadPool::RenderThreadPool() {
  task_queue_ = new queue::LifoBoundedMPSCBlockingQueue<Task>;
}
RenderThreadPool::~RenderThreadPool() {
  delete task_queue_;
}

void RenderThreadPool::Join() {
  state_ = State::kJoined;
  // TODO: Join() from main thread and Run() from AudioThread intersecting there
  //   need synchronization
  while (!task_queue_->Empty()) {
    (task_queue_->Front())();
  }
  /*for (auto& thread : threads_) {
    thread.join();
  }*/
}

void RenderThreadPool::InitOpenGLContext() {
  if (openal_initialized_) {
    return;
  }


  /// initialization
/*  InitOpenALContext();
  if (!openal_initialized_) {
    std::cerr << "Can't create OpenAL context" << std::endl;
    std::terminate(); // TODO: replace by Logger::LogIF OR FAITHFUL_TERMINATE
  }
  InitOpenALBuffersAndSources();*/

  glfwSetErrorCallback(error_callback);

  if (!glfwInit()) {
    return -1;
  }

  // Force create OpenGL 3.3
  // NOTE(syoyo): Linux + NVIDIA driver segfaults for some reason? commenting out glfwWindowHint will work.
  // Note (PE): On laptops with intel hd graphics card you can overcome the segfault by enabling experimental, see below (tested on lenovo thinkpad)
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glewExperimental = GL_TRUE;

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif


  /// we have only one OpenGL context TODO: static assert?

  Window window = Window(800, 600, "TinyGLTF basic example");
  glfwMakeContextCurrent(window.window);

#ifdef __APPLE__
  // https://stackoverflow.com/questions/50192625/openggl-segmentation-fault
  glewExperimental = GL_TRUE;
#endif

  // GLAD
  /*glewInit();
  std::cout << glGetString(GL_RENDERER) << ", " << glGetString(GL_VERSION)
            << std::endl;

  if (!GLEW_VERSION_3_3) {
    std::cerr << "OpenGL 3.3 is required to execute this app." << std::endl;
    return EXIT_FAILURE;
  }*/


  std::cerr << "OpenAL context initialized" << std::endl;

  openal_initialized_ = true;
}
void RenderThreadPool::DeinitOpenGLContext() {
  if (openal_initialized_) {
    // TODO: blocking call glfwWindowsShouldStop
    glfwTerminate();
    openal_initialized_ = false;
  }
}

void RenderThreadPool::Run() {
  if (state_ != State::kNotStarted) {
    return;
  }

  Shaders shader = Shaders();
  glUseProgram(shader.pid);

  // grab uniforms to modify
  GLuint MVP_u = glGetUniformLocation(shader.pid, "MVP");
  GLuint sun_position_u = glGetUniformLocation(shader.pid, "sun_position");
  GLuint sun_color_u = glGetUniformLocation(shader.pid, "sun_color");

  tinygltf::Model model;
  if (!loadModel(model, filename.c_str())) return;

  std::pair<GLuint, std::map<int, GLuint>> vaoAndEbos = bindModel(model);
  // dbgModel(model); return;

  // Model matrix : an identity matrix (model will be at the origin)
  glm::mat4 model_mat = glm::mat4(1.0f);
  glm::mat4 model_rot = glm::mat4(1.0f);
  glm::vec3 model_pos = glm::vec3(-3, 0, -3);

  // generate a camera view, based on eye-position and lookAt world-position
  glm::mat4 view_mat = genView(glm::vec3(2, 2, 20), model_pos);

  glm::vec3 sun_position = glm::vec3(3.0, 10.0, -5.0);
  glm::vec3 sun_color = glm::vec3(1.0);

  /// main loop
  while (!window.Close()) {
    if (!task_queue_->Empty()) { // TODO: OpenGL tasks from other threads
      (task_queue_->Front())();
    }

    window.Resize();

    glClearColor(0.2, 0.2, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 trans =
        glm::translate(glm::mat4(1.0f), model_pos);  // reposition model
    model_rot = glm::rotate(model_rot, glm::radians(0.8f),
                            glm::vec3(0, 1, 0));  // rotate model on y axis
    model_mat = trans * model_rot;

    // build a model-view-projection
    GLint w, h;
    glfwGetWindowSize(window.window, &w, &h);
    glm::mat4 mvp = genMVP(view_mat, model_mat, 45.0f, w, h);
    glUniformMatrix4fv(MVP_u, 1, GL_FALSE, &mvp[0][0]);

    glUniform3fv(sun_position_u, 1, &sun_position[0]);
    glUniform3fv(sun_color_u, 1, &sun_color[0]);

    drawModel(vaoAndEbos, model);
    glfwSwapBuffers(window.window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &vaoAndEbos.first);
}


} // namespace details
} // namespace faithful
