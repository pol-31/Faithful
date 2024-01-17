/*

/// currently a little bit changed example provided by syoyo: tinygltf


#include <iostream>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "glm/gtc/matrix_transform.hpp"

//#define TINYGLTF_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#define TINYGLTF_NOEXCEPTION
//#define JSON_NOEXCEPTION
//#define TINYGLTF_USE_RAPIDJSON
#include "tiny_gltf.h"

#include "io/Window.h"
#include "loader/ShaderProgram.h"


bool TinygltfLoadImageCallback(tinygltf::Image *, const int, std::string *,
                               std::string *, int, int,
                               const unsigned char *, int,
                               void *user_pointer) {
  std::cout << "+1 loaded image" << std::endl;
  return true;
}

bool LoadModel(tinygltf::Model &model, const char *filename) {
  tinygltf::TinyGLTF loader;
  loader.SetImageLoader(TinygltfLoadImageCallback, nullptr);
  std::string err;
  std::string warn;

  bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
  if (!warn.empty()) {
    std::cout << "WARN: " << warn << std::endl;
  }

  if (!err.empty()) {
    std::cout << "ERR: " << err << std::endl;
  }

  if (!res) {
    std::cout << "Failed to load glTF: " << filename << std::endl;
  } else {
    std::cout << "Loaded glTF: " << filename << std::endl;
  }
  return res;
}

void BindMesh(std::map<int, GLuint> &vbos,
              tinygltf::Model &model, tinygltf::Mesh &mesh) {
  for (size_t i = 0; i < model.bufferViews.size(); ++i) {
    const tinygltf::BufferView &buffer_view = model.bufferViews[i];
    if (buffer_view.target == 0) {
      std::cout << "WARN: bufferView.target is zero" << std::endl;
      continue;  // Unsupported bufferView.
    }

    const tinygltf::Buffer &buffer = model.buffers[buffer_view.buffer];
    std::cout << "bufferview.target " << buffer_view.target << std::endl;

    GLuint vbo;
    glGenBuffers(1, &vbo);
    vbos[i] = vbo;
    glBindBuffer(buffer_view.target, vbo);

    std::cout << "buffer.data.size = " << buffer.data.size()
              << ", bufferview.byteOffset = " << buffer_view.byteOffset
              << std::endl;

    glBufferData(buffer_view.target, buffer_view.byteLength,
                 &buffer.data.at(0) + buffer_view.byteOffset, GL_STATIC_DRAW);
  }

  for (size_t i = 0; i < mesh.primitives.size(); ++i) {
    tinygltf::Primitive primitive = mesh.primitives[i];
    tinygltf::Accessor index_accessor = model.accessors[primitive.indices];

    for (auto &attrib : primitive.attributes) {
      tinygltf::Accessor accessor = model.accessors[attrib.second];
      int byte_stride =
          accessor.ByteStride(model.bufferViews[accessor.bufferView]);
      glBindBuffer(GL_ARRAY_BUFFER, vbos[accessor.bufferView]);

      int size = 1;
      if (accessor.type != TINYGLTF_TYPE_SCALAR) {
        size = accessor.type;
      }

      int vaa = -1;
      if (attrib.first.compare("POSITION") == 0) {
        vaa = 0;
      }
      if (attrib.first.compare("NORMAL") == 0) {
        vaa = 1;
      }
      if (attrib.first.compare("TEXCOORD_0") == 0) {
        vaa = 2;
      }
      if (vaa > -1) {
        glEnableVertexAttribArray(vaa);
        glVertexAttribPointer(vaa, size, accessor.componentType,
                              accessor.normalized ? GL_TRUE : GL_FALSE,
                              byte_stride, (char*)(nullptr) + accessor.byteOffset);
      } else {
        std::cout << "vaa missing: " << attrib.first << std::endl;
      }
    }

    if (!model.textures.empty()) {
      tinygltf::Texture &tex = model.textures[0];
      if (tex.source > -1) {
        GLuint texid;
        glGenTextures(1, &texid);

        // TODO: tinygltf::Image as_is = true
        //    all model textures are compressed into RGBA ASTC

        tinygltf::Image &image = model.images[tex.source];
        glBindTexture(GL_TEXTURE_2D, texid);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        GLenum format = GL_RGBA;

        // TODO: need to find out this
        GLenum type = GL_UNSIGNED_BYTE;
        if (image.bits == 8) {
        } else if (image.bits == 16) {
          type = GL_UNSIGNED_SHORT;
        } else {
          // ???
        }

//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
//                     format, type, &image.image.at(0));
      }
    }
  }
}

void BindModelNodes(std::map<int, GLuint> &vbos, tinygltf::Model &model,
                    tinygltf::Node &node) {
  if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
    BindMesh(vbos, model, model.meshes[node.mesh]);
  }
  for (size_t i = 0; i < node.children.size(); i++) {
    assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
    BindModelNodes(vbos, model, model.nodes[node.children[i]]);
  }
}

std::pair<GLuint, std::map<int, GLuint>> BindModel(tinygltf::Model &model) {
  std::map<int, GLuint> vbos;
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  const tinygltf::Scene &scene = model.scenes[model.defaultScene];
  for (size_t i = 0; i < scene.nodes.size(); ++i) {
    assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model.nodes.size()));
    BindModelNodes(vbos, model, model.nodes[scene.nodes[i]]);
  }

  glBindVertexArray(0);
  // TODO: vbo deleting? (not ebo)
  for (auto it = vbos.cbegin(); it != vbos.cend();) {
    tinygltf::BufferView buffer_view = model.bufferViews[it->first];
    if (buffer_view.target != GL_ELEMENT_ARRAY_BUFFER) {
      glDeleteBuffers(1, &vbos[it->first]);
      vbos.erase(it++);
    } else {
      ++it;
    }
  }

  return {vao, vbos};
}

void DrawMesh(const std::map<int, GLuint> &vbos,
              tinygltf::Model &model, tinygltf::Mesh &mesh) {
  for (size_t i = 0; i < mesh.primitives.size(); ++i) {
    tinygltf::Primitive primitive = mesh.primitives[i];
    tinygltf::Accessor index_accessor = model.accessors[primitive.indices];

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos.at(index_accessor.bufferView));

    glDrawElements(primitive.mode, index_accessor.count,
                   index_accessor.componentType,
                   (char*)(nullptr) + index_accessor.byteOffset);
  }
}
void DrawModelNodes(const std::pair<GLuint, std::map<int, GLuint>> &vao_and_ebos,
                    tinygltf::Model &model, tinygltf::Node &node) {
  if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
    DrawMesh(vao_and_ebos.second, model, model.meshes[node.mesh]);
  }
  for (size_t i = 0; i < node.children.size(); i++) {
    DrawModelNodes(vao_and_ebos, model, model.nodes[node.children[i]]);
  }
}
void DrawModel(const std::pair<GLuint, std::map<int, GLuint>> &vao_and_ebos,
               tinygltf::Model &model) {
  glBindVertexArray(vao_and_ebos.first);

  const tinygltf::Scene &scene = model.scenes[model.defaultScene];
  for (size_t i = 0; i < scene.nodes.size(); ++i) {
    DrawModelNodes(vao_and_ebos, model, model.nodes[scene.nodes[i]]);
  }
  glBindVertexArray(0);
}


// TODO: faithful::Camera
glm::mat4 GenView(glm::vec3 pos, glm::vec3 lookat) {
  return glm::lookAt(pos, lookat, glm::vec3(0, 1, 0));
}
glm::mat4 GenMvp(glm::mat4 view_mat, glm::mat4 model_mat, float fov, int w,
                 int h) {
  glm::mat4 projection =
      glm::perspective(glm::radians(fov),
                       (float) w / (float) h,
                       0.01f,
                       1000.0f);
  glm::mat4 mvp = projection * view_mat * model_mat;
  return mvp;
}

// TODO: input
// TODO: consider OpenGL concurrency




void RunRenderLoop(faithful::Window &window, const std::string &filename) {
  faithful::utility::ShaderProgram shader("../../assets/shaders/tinygltf_test.vert",
                                          "../../assets/shaders/tinygltf_test.frag");
  glUseProgram(shader.Id());

  GLuint mvp_u = glGetUniformLocation(shader.Id(), "MVP");
  GLuint sun_position_u = glGetUniformLocation(shader.Id(), "sun_position");
  GLuint sun_color_u = glGetUniformLocation(shader.Id(), "sun_color");

  tinygltf::Model model;
  if (!LoadModel(model, filename.c_str())) {
    return;
  }
  std::pair<GLuint, std::map<int, GLuint>> vao_and_ebos = BindModel(model);

  glm::mat4 model_mat = glm::mat4(1.0f);
  glm::mat4 model_rot = glm::mat4(1.0f);
  glm::vec3 model_pos = glm::vec3(-3, 0, -3);

  glm::mat4 view_mat = GenView(glm::vec3(2, 10, 6), model_pos);

  glm::vec3 sun_position = glm::vec3(3.0, 7.0, 10.0);
  glm::vec3 sun_color = glm::vec3(0.2f, 1.0f, 0.2f);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  while (!glfwWindowShouldClose(window.Glfw())) {
    window.Resize();

    glClearColor(0.2, 0.2, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 trans =
      glm::translate(glm::mat4(1.0f), model_pos);
    model_rot = glm::rotate(model_rot, glm::radians(0.8f),
                            glm::vec3(0, 1, 0));
    model_mat = trans * model_rot;

    GLint w, h;
    glfwGetWindowSize(window.Glfw(), &w, &h);
    glm::mat4 mvp = GenMvp(view_mat, model_mat, 45.0f, w, h);
    glUniformMatrix4fv(mvp_u, 1, GL_FALSE, &mvp[0][0]);

    glUniform3fv(sun_position_u, 1, &sun_position[0]);
    glUniform3fv(sun_color_u, 1, &sun_color[0]);

    DrawModel(vao_and_ebos, model);
    glfwSwapBuffers(window.Glfw());
    glfwPollEvents();
  }


  glDeleteVertexArrays(1, &vao_and_ebos.first);
}

static void ErrorCallback(int error, const char *description) {
  (void) error;
  fprintf(stderr, "Error: %s\n", description);
}

int main() {

  std::string filename = "/home/pavlo/Downloads/DamagedHelmet.gltf";

  glfwSetErrorCallback(ErrorCallback);

  if (!glfwInit()) {
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  faithful::Window window("Faithful", 800, 600);
  glfwMakeContextCurrent(window.Glfw());

  gladLoadGL();

  RunRenderLoop(window, filename);

  glfwTerminate();
  return 0;
}*/

#include <stdlib.h>
#include <mimalloc-override.h>
/// We don't need to #include "mimalloc-new-delete.h"
/// as it has already been overridden by libmimalloc.a

#define AL_LIBTYPE_STATIC
//#include "config.h"

#include "AL/al.h"
#include "AL/alc.h"
#include <sndfile.h>
#include <iostream>
//#include "alsa/asoundlib.h"
#include "asoundlib.h"
// Function to check for OpenAL errors
void CheckALError(const char* message) {
//  ALenum error = alGetError();
//  if (error != AL_NO_ERROR) {
//    std::cerr << "OpenAL Error (" << message << "): " << alGetString(error) << std::endl;
//  }
}

int main() {
  const char *pcm_name = "default";
  snd_pcm_t *pcm_handle;
  int rc = snd_pcm_open(&pcm_handle, pcm_name, SND_PCM_STREAM_PLAYBACK, 0);
  if (rc < 0) {
    std::cerr << "Error: Unable to open PCM device '" << pcm_name << "': " << snd_strerror(rc) << std::endl;
    return 1;
  }

  // Display the PCM device information
  std::cout << "PCM Name: " << snd_pcm_name(pcm_handle) << std::endl;
  std::cout << "PCM State: " << snd_pcm_state_name(snd_pcm_state(pcm_handle)) << std::endl;

  // Close the PCM handle
  snd_pcm_close(pcm_handle);
//  return 0;

  // Initialize OpenAL context
  ALCdevice* device = alcOpenDevice(nullptr);
  if (!device) {
    std::cerr << "Failed to open OpenAL device." << std::endl;
    return 1;
  }

  ALCcontext* context = alcCreateContext(device, nullptr);
  if (!context) {
    std::cerr << "Failed to create OpenAL context." << std::endl;
    alcCloseDevice(device);
    return 1;
  }

  alcMakeContextCurrent(context);

  // Load audio file using libsndfile
  const char* audio_file = "/home/pavlo/Desktop/sample2_.wav";  // Replace with your audio file path

  SF_INFO sf_info;
  SNDFILE* snd_file = sf_open(audio_file, SFM_READ, &sf_info);
  if (!snd_file) {
    std::cerr << "Failed to open audio file." << std::endl;
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(context);
    alcCloseDevice(device);
    return 1;
  }

  ALsizei size = static_cast<ALsizei>(sf_info.frames) * sf_info.channels * sizeof(int16_t);
  int16_t* buffer_data = new int16_t[size];
  sf_readf_short(snd_file, buffer_data, sf_info.frames);
  sf_close(snd_file);

  // Set up OpenAL buffer
  ALuint buffer;
  alGenBuffers(1, &buffer);
  alBufferData(buffer, (sf_info.channels == 2) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16,
               buffer_data, size, sf_info.samplerate);

  // Set up source
  ALuint source;
  alGenSources(1, &source);
  alSourcei(source, AL_BUFFER, buffer);

  // Play the sound
  alSourcePlay(source);

  // Wait for the sound to finish playing (you can add more sophisticated logic)
  ALint source_state;
  do {
    alGetSourcei(source, AL_SOURCE_STATE, &source_state);
  } while (source_state == AL_PLAYING);

  // Clean up
  alDeleteSources(1, &source);
  alDeleteBuffers(1, &buffer);
  delete[] buffer_data;

  // Close OpenAL context and device
  alcMakeContextCurrent(nullptr);
  alcDestroyContext(context);
  alcCloseDevice(device);

  CheckALError("msg");

  return 0;
}
