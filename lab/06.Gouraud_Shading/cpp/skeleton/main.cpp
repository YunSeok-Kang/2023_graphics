﻿///// main.cpp
///// OpenGL 3+, GLSL 1.20, GLEW, GLFW3

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <fstream>
#include <cassert>
#include <map>

// include glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// include imGui and imGuIZMO
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imGuIZMOquat.h"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Material.h"
#include "Light.h"

////////////////////////////////////////////////////////////////////////////////
/// initialization 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
GLFWwindow* createWindow(int width, int height, const char* title);
void init_window(GLFWwindow* window);
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// shading 관련 변수
//////////////////////////////////////////////////////////////////////////////// 
glm::vec3 g_clear_color(0.5f, 0.5f, 0.5f);
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
/// 쉐이더 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
GLuint  program;          // 쉐이더 프로그램 객체의 레퍼런스 값
GLint   loc_a_position;   // attribute 변수 a_position 위치
GLint   loc_a_normal;     // attribute 변수 a_normal 위치
GLint   loc_u_PVM;        // uniform 변수 u_PVM 위치

// TODO: GPU 쪽 uniform/attribute 변수의 위치를 받아놓을 GLint 타입의 변수 선언 
GLint   loc_u_model_matrix;
GLint   loc_u_normal_matrix;

GLint   loc_u_camera_position;
GLint   loc_u_light_position;

GLint   loc_u_light_ambient;
GLint   loc_u_light_diffuse;
GLint   loc_u_light_specular;

GLint   loc_u_obj_ambient;
GLint   loc_u_obj_diffuse;
GLint   loc_u_obj_specular;
GLint   loc_u_obj_shininess;


GLuint create_shader_from_file(const std::string& filename, GLuint shader_type);
void init_shader_program();
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 카메라 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
struct Camera
{
  glm::vec3 position = glm::vec3(0.0f, 0.0f, 2.0f);
  glm::vec3 at = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
  float aspect = 1.0f;
  float fovy = 60.0f;
};

Camera g_camera;
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 모델 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
enum ShadingType { kSmooth, kFlat };
ShadingType   g_shading_type = kSmooth;

// triangle mesh
struct Mesh
{
  // raw data from a file
  std::vector<glm::vec3>      positions;      // per-vertex 3D positions (raw data)
  std::vector<glm::vec3>      normals;        // per-vertex 3D normal (size = #vertices)
  std::vector<unsigned int>   tv_indices;     // size = 3 x #triangles

  // position and normal for flat shading
  std::vector<glm::vec3>      tv_positions;       // per triangle-vertex 3D position (size = 3 x #triangles)
  std::vector<glm::vec3>      tv_flat_normals;    // per triangle-vertex flat normal (size = 3 x #triangles)
  std::vector<glm::vec3>      tv_smooth_normals;  // per triangle-vertex smooth normal (size = 3 x #triangles)

  GLuint  position_buffer;    // GPU 메모리에서 vertices_buffer 위치 
  GLuint  normal_buffer;      // GPU 메모리에서 normal_buffer 위치

  Material   material;        // mesh material  
};

struct Model 
{
  std::vector<Mesh> meshes;    // a model consisting of triangle meshes
  
  std::string       name;

  glm::vec3         translate = glm::vec3(0.0f);
  glm::quat         quat = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
  glm::vec3         scale = glm::vec3(1.5f);
};

Model g_model;

bool updata_mesh_data(Mesh& mesh);    
bool gen_gl_buffers(Mesh& mesh);
bool set_gl_buffers(Mesh& mesh); 
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 렌더링 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
Light g_light;

bool init_scene(const std::string& filename);
bool load_model(const std::string& filename);
bool init_gpu_buffers();
bool load_asset(const std::string& filename);
void render_object();           // rendering 함수: 물체(삼각형)를 렌더링하는 함수.
void draw_mesh(Mesh& mesh);
void render(GLFWwindow* window);
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// IMGUI / keyboard / scroll input 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
ImFont* font = NULL;
std::string font_path = "../../../../third_party/fonts/NotoSansKR/NotoSansKR-Regular.otf";
float font_size_pixels = 18.0f;
bool is_font_loaded = false;

int g_obj_select_idx = 0;
int g_cam_select_idx = 0;

void init_imgui(GLFWwindow* window);
void compose_imgui_frame(GLFWwindow* window, int key, int scancode, int action, int mods);

void key_callback();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double x, double y);
////////////////////////////////////////////////////////////////////////////////


GLFWwindow* createWindow(int width, int height, const char* title)
{
  GLFWwindow* window; // create window

  // Initialize GLFW Library
  if (!glfwInit())
    return NULL;

  // Create a GLFW window containing a OpenGL context
  window = glfwCreateWindow(width, height, title, NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return NULL;
  }

  // Make the current OpenGL contexts as one in the window
  glfwMakeContextCurrent(window); 

  // Initialize GLEW library
  if (glewInit() != GLEW_OK)
  {
    std::cout << "GLEW Init Error!" << std::endl;
  }

  // Print out the OpenGL version supported by the graphics card in my PC
  std::cout << glGetString(GL_VERSION) << std::endl;

  return window;
}

void scroll_callback(GLFWwindow* window, double x, double y)
{
  g_camera.fovy += y;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  g_camera.aspect = (float) width / (float) height;
  glViewport(0, 0, width, height);
}


void init_window(GLFWwindow* window) 
{
  init_imgui(window);
  init_shader_program();

  glEnable(GL_DEPTH_TEST);

  glfwSetKeyCallback(window, key_callback);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetScrollCallback(window, scroll_callback);
}


void init_imgui(GLFWwindow* window) 
{
  const char* glsl_version = "#version 120";

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;       // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;     // Enable Multi-Viewport / Platform Windows
  //io.ConfigViewportsNoAutoMerge = true;
  //io.ConfigViewportsNoTaskBarIcon = true;

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  font = io.Fonts->AddFontFromFileTTF(font_path.c_str(), font_size_pixels, NULL, io.Fonts->GetGlyphRangesKorean());
}


bool init_scene(const std::string& filename)
{
  std::ifstream fin(filename);
  if (fin.fail()) 
    return false;

  int count;
  fin >> count;
  if (count != 1)
  {
    std::cout << "ERROR: This program only supports loading a single object in a scene" << std::endl;
    return  false;
  }

  std::string name;
  fin >> name;
  if (!load_model(name))
  { 
    std::cout << "Failed to load a model file: " << name << std::endl;
    return -1;
  }

  fin >> g_model.scale[0] >> g_model.scale[1] >> g_model.scale[2] 
      >> g_model.translate[0] >> g_model.translate[1] >> g_model.translate[2];


  // Cautions: This program doesn't care of loading camera information from a scene file
  fin >> count;
  if (count != 1)
  {
    std::cout << "ERROR: This program only supports loading a single camera in a scene" << std::endl;
    return  false;
  }
  fin >> g_camera.position[0] >> g_camera.position[1] >> g_camera.position[2] 
      >> g_camera.at[0] >> g_camera.at[1] >> g_camera.at[2] 
      >> g_camera.up[0] >> g_camera.up[1] >> g_camera.up[2];

  return true;
}


bool load_model(const std::string& filename)
{
  const aiScene* scene = aiImportFile(filename.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
  if (scene == NULL)
      return false;

  aiColor3D tmp;    
  aiString name;

  g_model.meshes.clear();
  for (unsigned int m = 0; m < scene->mNumMeshes; ++m)
  {
    // positions

    // face-vertex indices 

    // material

    // generate and set GPU buffers
  }

  return true;
}

bool updata_mesh_data(Mesh& mesh)
{
  // raw data
  
  // buffer data for flat shading

  // init normals

  // init tv_positions and tv_flat_normals

  // for each triangle
  // 1) compute face normal 
  // 2) update normals, tv_positions, tv_flat_normals 

  // normalize per-vertex normals

  // update tv_smooth_normals

  return  true;
}

bool gen_gl_buffers(Mesh& mesh)
{
  // generate GPU buffers
  glGenBuffers(1, &mesh.position_buffer);
  glGenBuffers(1, &mesh.normal_buffer);

  return  true;
}

bool set_gl_buffers(Mesh& mesh)
{
  // per-vertex position and normal data

  if (g_shading_type == kSmooth)
  {
  }
  else //if (g_shading_type == kFlat)
  {
  }

  return  true;
}


void compose_imgui_frame()
{
  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  
  if (!is_font_loaded)
  {
    ImGui::PushFont(font);
    is_font_loaded = true;
  }

  // control window
  {
    ImGui::Begin("모델(model)");

    ImGui::SliderFloat3("Tranlsate", glm::value_ptr(g_model.translate), -10.0f, 10.0f);
    ImGui::gizmo3D("Rotation", g_model.quat);
    ImGui::SliderFloat3("Scale", glm::value_ptr(g_model.scale), 0.1f, 2.0f);

    ImGui::NewLine();

    ImGui::Text("Shading");
    bool prev_shading_type = g_shading_type;
    bool is_flat_shading = g_shading_type ? kFlat : kSmooth;
    ImGui::Checkbox("Flat shading: ", &is_flat_shading);
    g_shading_type = is_flat_shading ? kFlat : kSmooth;
    if (g_shading_type != prev_shading_type)
    {
      for (std::size_t i = 0; i < g_model.meshes.size(); ++i)
        set_gl_buffers(g_model.meshes[i]);
      std::cout << "shading changed" << std::endl;
    }
    
    ImGui::NewLine();

    ImGui::Text("Materials");
    std::string label;
    for (std::size_t i = 0; i < g_model.meshes.size(); ++i)
    {
      Mesh& mesh = g_model.meshes[i];

      label = mesh.material.name + "-ambient";
      ImGui::ColorEdit3(label.c_str(), glm::value_ptr(mesh.material.ambient));
      label = mesh.material.name + "-diffuse";
      ImGui::ColorEdit3(label.c_str(), glm::value_ptr(mesh.material.diffuse));
      label = mesh.material.name + "-specular";
      ImGui::ColorEdit3(label.c_str(), glm::value_ptr(mesh.material.specular));
      label = mesh.material.name + "-shininess";
      ImGui::SliderFloat(label.c_str(), &mesh.material.shininess, 0.0f, 500.0f);
      ImGui::NewLine();
    }
    
    ImGui::End();
  }

  // control window
  {
    ImGui::Begin("1인칭 시점 카메라");

    ImGui::Text("Intrinsic Parameters");
    ImGui::SliderFloat("fovy (deg)", &g_camera.fovy, 10.f, 160.f);
    
    ImGui::End();
  }

  {
    ImGui::Begin("Lighting");;

    ImGui::ColorEdit3("background color", glm::value_ptr(g_clear_color));

    glm::vec3 vec(-g_light.pos); 
    ImGui::gizmo3D("Light direction", vec);
    g_light.pos = -vec;

    ImGui::ColorEdit3("ambient light", glm::value_ptr(g_light.ambient));
    ImGui::ColorEdit3("diffuse light", glm::value_ptr(g_light.diffuse));
    ImGui::ColorEdit3("specular light", glm::value_ptr(g_light.specular));

    ImGui::End();
  }

  if (is_font_loaded)
  {
    ImGui::PopFont();
    is_font_loaded = false;
  }
}

// GLSL 파일을 읽어서 컴파일한 후 쉐이더 객체를 생성하는 함수
GLuint create_shader_from_file(const std::string& filename, GLuint shader_type)
{
  GLuint shader = 0;

  shader = glCreateShader(shader_type);

  std::ifstream shader_file(filename.c_str());
  std::string shader_string;

  shader_string.assign(
    (std::istreambuf_iterator<char>(shader_file)),
    std::istreambuf_iterator<char>());

  // Get rid of BOM in the head of shader_string
  // Because, some GLSL compiler (e.g., Mesa Shader compiler) cannot handle UTF-8 with BOM
  if (shader_string.compare(0, 3, "\xEF\xBB\xBF") == 0)  // Is the file marked as UTF-8?
  {
    std::cout << "Shader code (" << filename << ") is written in UTF-8 with BOM" << std::endl;
    std::cout << "  When we pass the shader code to GLSL compiler, we temporarily get rid of BOM" << std::endl;
    shader_string.erase(0, 3);                  // Now get rid of the BOM.
  }

  const GLchar* shader_src = shader_string.c_str();
  glShaderSource(shader, 1, (const GLchar * *)& shader_src, NULL);
  glCompileShader(shader);

  GLint is_compiled;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
  if (is_compiled != GL_TRUE)
  {
    std::cout << "Shader COMPILE error: " << std::endl;

    GLint buf_len;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &buf_len);

    std::string log_string(1 + buf_len, '\0');
    glGetShaderInfoLog(shader, buf_len, 0, (GLchar *)log_string.c_str());

    std::cout << "error_log: " << log_string << std::endl;

    glDeleteShader(shader);
    shader = 0;
  }

  return shader;
}

// vertex shader와 fragment shader를 링크시켜 program을 생성하는 함수
void init_shader_program()
{
  GLuint vertex_shader
    = create_shader_from_file("./shader/vertex.glsl", GL_VERTEX_SHADER);

  std::cout << "vertex_shader id: " << vertex_shader << std::endl;
  assert(vertex_shader != 0);

  GLuint fragment_shader
    = create_shader_from_file("./shader/fragment.glsl", GL_FRAGMENT_SHADER);

  std::cout << "fragment_shader id: " << fragment_shader << std::endl;
  assert(fragment_shader != 0);

  program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  GLint is_linked;
  glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
  if (is_linked != GL_TRUE)
  {
    std::cout << "Shader LINK error: " << std::endl;

    GLint buf_len;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &buf_len);

    std::string log_string(1 + buf_len, '\0');
    glGetProgramInfoLog(program, buf_len, 0, (GLchar *)log_string.c_str());

    std::cout << "error_log: " << log_string << std::endl;

    glDeleteProgram(program);
    program = 0;
  }

  std::cout << "program id: " << program << std::endl;
  assert(program != 0);

  loc_u_PVM = glGetUniformLocation(program, "u_PVM");

  loc_a_position = glGetAttribLocation(program, "a_position");
  // loc_a_color = glGetAttribLocation(program, "a_color");

  

  // TODO: get uniform/attribute locations
  // phong reflection model

}

void render_object()
{
  // set transform
  glm::mat4 mat_view = glm::lookAt(g_camera.position, g_camera.at, g_camera.up);
  glm::mat4 mat_proj = glm::perspective(glm::radians(g_camera.fovy), g_camera.aspect, 0.01f, 1000.0f);


  // 특정 쉐이더 프로그램 사용
  glUseProgram(program);

  // TODO : send uniform for camera & light to GPU
  glUniformMatrix4fv(loc_u_view_matrix, 1, false, glm::value_ptr(mat_view)); 
  glUniform3fv(loc_u_camera_position, 1, glm::value_ptr(g_camera.position));
  
  // TODO : set mat_model, mat_normal, mat_PVM 
  // TODO : send uniform data for model to GPU
  glm::mat4 mat_model = glm::mat4(1.0f);

  mat_model = mat_model * glm::translate(glm::mat4(1.0f), g_model.translate);
  mat_model = mat_model * glm::mat4_cast(g_model.quat);
  mat_model = mat_model * glm::scale(glm::mat4(1.0f), g_model.scale);

  glm::mat3 mat_normal = glm::mat3(mat_model); 

  glm::mat4 mat_PVM = mat_proj * mat_view * mat_model;

  // send uniform to the GPU side: mat_PVM, mat_model, mat_normal

  for (std::size_t i=0; i < g_model.meshes.size(); ++i)
    draw_mesh(g_model.meshes[i]);

  // 쉐이더 프로그램 사용해제
  glUseProgram(0);
}

void draw_mesh(Mesh& mesh)
{
  // send uniform to the GPU side: ambient, diffuse, shininess

  // glBindBuffer, 
  // glEnableVertexAttribArray for position, normal
  // glVertexAttribPointer for position, normal
  // glDrawArrays
  // glDisableVertexAttribArray for position, normal

}


void render(GLFWwindow* window) 
{
  glClearColor(g_clear_color[0], g_clear_color[1], g_clear_color[2], 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glfwPollEvents();
  compose_imgui_frame();

  render_object();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  // Update and Render additional Platform Windows
  // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
  //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    GLFWwindow* backup_current_context = glfwGetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwMakeContextCurrent(backup_current_context);
  }

  // Swap front and back buffers
  glfwSwapBuffers(window);

  // Poll for and process events
  glfwPollEvents();
}


int main(int argc, char* argv[])
{
  // create window
  GLFWwindow* window = createWindow(1000, 1000, "Shading");

  // initialize window
  init_window(window);

  std::string filename = "info.txt";
  if (!init_scene(filename))
  { 
    std::cout << "Failed to load a scene file: " << filename << std::endl;
    return -1;
  }

  // Loop until the user closes the window
  while (!glfwWindowShouldClose(window))
  {
    render(window);
  }

  glfwTerminate();

  return 0;
}
