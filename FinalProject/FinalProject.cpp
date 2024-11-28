#include <windows.h>

//Imgui headers for UI
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <sstream>
#include <fstream>

#include "DebugCallback.h"
#include "InitShader.h"    //Functions for loading shaders from text files
#include "LoadMesh.h"      //Functions for creating OpenGL buffers from mesh files
#include "LoadTexture.h"   //Functions for creating OpenGL textures from image files

//adaptable screen
float fov = 45.0f;
float nearClip = 0.3f;
float farClip = 3.0f;
float aspectRatio = 16.0f / 9.0f;

namespace window
{
   const char* const title = USER_NAME " " PROJECT_NAME; //defined in project settings
   int size[2] = {1024, 1024};
   float clear_color[4] = {0.35f, 0.35f, 0.35f, 1.0f};
}

namespace scene
{
   const std::string asset_dir = "assets/";
   const std::string mesh_name1 = "WinterHouse1.obj";
   const std::string tex_name1 = "WinterHouse1.bmp";
   const std::string mesh_name2 = "ChristmasTree.obj";
   const std::string tex_name2 = "ChristmasTree.bmp";
   const std::string mesh_name3 = "Santa.obj";
   const std::string tex_name3 = "Santa.bmp";
   const std::string mesh_name4 = "board.obj";
   const std::string tex_name4 = "board.bmp";
   
   MeshData mesh1;
   GLuint tex_id1 = -1; //Texture map for mesh1
   MeshData mesh2;
   GLuint tex_id2 = -1; //Texture map for mesh2
   MeshData mesh3;
   GLuint tex_id3 = -1; //Texture map for mesh3
   MeshData mesh4;
   GLuint tex_id4 = -1; //Texture map for mesh4

   float angle = 0.0f;
   float scale = 1.0f;

   //initialize position
   glm::vec3 house_position = glm::vec3(-0.176f, 0.647f, -0.116f);
   glm::vec3 tree_position = glm::vec3(-0.529f, -5.0f, -0.465f);
   glm::vec3 santa_position = glm::vec3(0.0f, 0.059f, -0.349f);
   glm::vec3 board_position = glm::vec3(-9.294f, 2.61f, -2.326f);

   //initialize rotation
   float house_rotation = 0.0f;
   float tree_rotation = 0.0f;
   float santa_rotation = -90.0f;
   float board_rotation = 30.0f;

   //initialize scale
   float house_scale = 1.677f;
   float tree_scale = 5.0f;
   float santa_scale = 1.627f;
   float board_scale = 0.134f;

   // View matrix parameters for House
   glm::vec3 camera_position = glm::vec3(0.118f, 1.059f, 1.0f);
   glm::vec3 camera_target = glm::vec3(0.118f, 2.0f, -2.093f);
   glm::vec3 camera_up = glm::vec3(0.012f, 1.0f, 0.0f);

   // View matrix parameters for ChristmasTree
   glm::vec3 tree_camera_position = glm::vec3(0.0f, -6.706f, -0.465f);
   glm::vec3 tree_camera_target = glm::vec3(0.118f, 10.0f, -0.698f);
   glm::vec3 tree_camera_up = glm::vec3(0.012f, 0.671f, 0.233f);

   // View matrix parameters for Santa
   glm::vec3 santa_camera_position = glm::vec3(-10.0f, 3.176f, -1.628f);
   glm::vec3 santa_camera_target = glm::vec3(0.118f, 2.0f, -3.023f);
   glm::vec3 santa_camera_up = glm::vec3(0.082f, 1.0f, 0.0f);

   // View matrix parameters for Board
   glm::vec3 board_camera_position = glm::vec3(-10.0f, 3.176f, -1.628f);
   glm::vec3 board_camera_target = glm::vec3(0.118f, 2.0f, -3.023f);
   glm::vec3 board_camera_up = glm::vec3(0.082f, 1.0f, 0.0f);

   // Projection parameters
   float fov = 62.363f;       // Field of View in degrees
   float near_clip = 0.1f;  // Near clipping plane
   float far_clip = 50.0f;  // Far clipping plane

   //animation control

   enum SantaAnimationState
   {
       MoveUp,
       PauseAndRotate,
       MoveDown,
       Pause
   };

   SantaAnimationState santa_animation_state = MoveUp; // 初始化为向上移动
   bool santa_animation_active = false;  // control animation start or not
   float santa_animation_time = 0.0f;   // count animation existing time
   int santa_animation_direction = 1;  // animation direction: 1 up, -1 down
   float santa_pause_time = 0.0f;        // time pause count
   float santa_pause_rotation = 0.0f;    // pause rotation
   int santa_rotation_direction = 1;  // count the roation direction
   
   const std::string shader_dir = "shaders/";
   const std::string vertex_shader("lab2_vs.glsl");
   const std::string fragment_shader("lab2_fs.glsl");

   GLuint shader = -1;
}
//For an explanation of this program's structure see https://www.glfw.org/docs/3.3/quick.html 


//Draw the ImGui user interface
void draw_gui(GLFWwindow* window)
{
   //Begin ImGui Frame
   ImGui_ImplOpenGL3_NewFrame();
   ImGui_ImplGlfw_NewFrame();
   ImGui::NewFrame();

   //Draw Gui
   ImGui::Begin("Debug window");
   if (ImGui::Button("Quit"))
   {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
   }

   //asset control
   ImGui::Begin("Asset Control");
   ImGui::Text("Adjust the position of assets:");

   // WinterHouse controls
   ImGui::Text("WinterHouse:");
   ImGui::SliderFloat3("House Position", glm::value_ptr(scene::house_position), -5.0f, 5.0f);
   ImGui::SliderAngle("House Rotation", &scene::house_rotation, -180.0f, 180.0f);
   ImGui::SliderFloat("House Scale", &scene::house_scale, 0.1f, 5.0f);

   // ChristmasTree controls
   ImGui::Text("ChristmasTree:");
   ImGui::SliderFloat3("Tree Position", glm::value_ptr(scene::tree_position), -5.0f, 5.0f);
   ImGui::SliderAngle("Tree Rotation", &scene::tree_rotation, -180.0f, 180.0f);
   ImGui::SliderFloat("Tree Scale", &scene::tree_scale, 0.1f, 5.0f);

   // Santa controls
   ImGui::Text("Santa:");
   ImGui::SliderFloat3("Santa Position", glm::value_ptr(scene::santa_position), -5.0f, 5.0f);
   ImGui::SliderAngle("Santa Rotation", &scene::santa_rotation, -180.0f, 180.0f);
   ImGui::SliderFloat("Santa Scale", &scene::santa_scale, 0.1f, 5.0f);

   // Board controls
   ImGui::Text("Board:");
   ImGui::SliderFloat3("Board Position", glm::value_ptr(scene::board_position), -10.0f, 10.0f);
   ImGui::SliderAngle("Board Rotation", &scene::board_rotation, -180.0f, 180.0f);
   ImGui::SliderFloat("Board Scale", &scene::board_scale, 0.1f, 5.0f);

   // Camera controls for house
   ImGui::Separator();
   ImGui::Text("House Camera Control:");
   ImGui::SliderFloat3("Camera Position", glm::value_ptr(scene::camera_position), -10.0f, 10.0f);
   ImGui::SliderFloat3("Camera Target", glm::value_ptr(scene::camera_target), -10.0f, 10.0f);
   ImGui::SliderFloat3("Camera Up Vector", glm::value_ptr(scene::camera_up), -1.0f, 1.0f);

   // Camera controls for tree
   ImGui::Separator();
   ImGui::Text("ChristmasTree Camera Control:");
   ImGui::SliderFloat3("Tree Camera Position", glm::value_ptr(scene::tree_camera_position), -10.0f, 10.0f);
   ImGui::SliderFloat3("Tree Camera Target", glm::value_ptr(scene::tree_camera_target), -10.0f, 10.0f);
   ImGui::SliderFloat3("Tree Camera Up Vector", glm::value_ptr(scene::tree_camera_up), -1.0f, 1.0f);

   // Camera controls for santa
   ImGui::Separator();
   ImGui::Text("Santa Camera Control:");
   ImGui::SliderFloat3("Santa Camera Position", glm::value_ptr(scene::santa_camera_position), -10.0f, 10.0f);
   ImGui::SliderFloat3("Santa Camera Target", glm::value_ptr(scene::santa_camera_target), -10.0f, 10.0f);
   ImGui::SliderFloat3("Santa Camera Up Vector", glm::value_ptr(scene::santa_camera_up), -1.0f, 1.0f);

   // Camera controls for board
   ImGui::Separator();
   ImGui::Text("Board Camera Control:");
   ImGui::SliderFloat3("Board Camera Position", glm::value_ptr(scene::board_camera_position), -10.0f, 10.0f);
   ImGui::SliderFloat3("Board Camera Target", glm::value_ptr(scene::board_camera_target), -10.0f, 10.0f);
   ImGui::SliderFloat3("Board Camera Up Vector", glm::value_ptr(scene::board_camera_up), -1.0f, 1.0f);

   // Projection controls
   ImGui::Separator();
   ImGui::Text("Projection Control:");
   ImGui::SliderFloat("Field of View", &scene::fov, 10.0f, 120.0f);
   ImGui::SliderFloat("Near Plane", &scene::near_clip, 0.1f, 5.0f);
   ImGui::SliderFloat("Far Plane", &scene::far_clip, 1.0f, 50.0f);

   ImGui::End();

   //Santa Claus Animation
   ImGui::Begin("Animation Control");
   if (ImGui::Button("Move Santa Up"))
   {
       scene::santa_animation_active = true;  // start animation
       scene::santa_animation_time = 0.0f;    // reset animation time
   }
   ImGui::End();

   ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
   ImGui::End();

   static bool show_test = true;
   if(show_test)
   {
      ImGui::ShowDemoWindow(&show_test);
   }

   //End ImGui Frame
   ImGui::Render();
   ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


// This function gets called every time the scene gets redisplayed
void display(GLFWwindow* window)
{
   //Clear the screen to the color previously specified in the glClearColor(...) call.
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glm::mat4 PVM;

   //P
   glm::mat4 P = glm::perspective(glm::radians(scene::fov), aspectRatio, scene::near_clip, scene::far_clip);
   

   //house MV
   glm::mat4 M = glm::translate(scene::house_position) *
       glm::rotate(scene::house_rotation, glm::vec3(0.0f, 1.0f, 0.0f)) *
       glm::scale(glm::vec3(scene::house_scale * scene::mesh1.mScaleFactor * 1.6));
   glm::mat4 V = glm::lookAt(scene::camera_position, scene::camera_target, scene::camera_up);
   
   //tree MV
   glm::mat4 M2 = glm::translate(scene::tree_position) *
       glm::rotate(scene::tree_rotation, glm::vec3(0.0f, 1.0f, 0.0f)) *
       glm::scale(glm::vec3(scene::tree_scale * scene::mesh2.mScaleFactor * 0.15));
   glm::mat4 V2 = glm::lookAt(scene::tree_camera_position, scene::tree_camera_target, scene::tree_camera_up);

   //Santa MV
   glm::mat4 M3 = glm::translate(scene::santa_position) *
       glm::rotate(scene::santa_rotation, glm::vec3(0.0f, 1.0f, 0.0f)) *
       glm::scale(glm::vec3(scene::santa_scale * scene::mesh3.mScaleFactor * 5.0f));
   if (scene::santa_animation_state == scene::PauseAndRotate)
   {
       M3 = M3 * glm::rotate(glm::radians(scene::santa_pause_rotation), glm::vec3(0.0f, 1.0f, 0.0f));
   }
   glm::mat4 V3 = glm::lookAt(scene::santa_camera_position, scene::santa_camera_target, scene::santa_camera_up);

   //Board MV
   glm::mat4 M4 = glm::translate(scene::board_position) *
       glm::rotate(scene::board_rotation, glm::vec3(0.0f, 1.0f, 0.0f)) *
       glm::scale(glm::vec3(scene::board_scale * scene::mesh4.mScaleFactor));
   glm::mat4 V4 = glm::lookAt(scene::board_camera_position, scene::board_camera_target, scene::board_camera_up);

   glUseProgram(scene::shader);


   // mesh1: house
   // -------------
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, scene::tex_id1);
   int tex_loc = glGetUniformLocation(scene::shader, "diffuse_tex");
   if (tex_loc != -1)
   {
      glUniform1i(tex_loc, 0); // we bound our texture to texture unit 0
   }

   //Get location for shader uniform variable
   int PVM_loc = glGetUniformLocation(scene::shader, "PVM");
   if (PVM_loc != -1)
   {
      PVM = P * V * M;
      //Set the value of the variable at a specific location
      glUniformMatrix4fv(PVM_loc, 1, false, glm::value_ptr(PVM));
   }

   glBindVertexArray(scene::mesh1.mVao);
   scene::mesh1.DrawMesh();
   
   // mesh2: tree
   // -------------
   glBindTexture(GL_TEXTURE_2D, scene::tex_id2);
   glUniform1i(tex_loc, 0);

   PVM = P * V2 * M2;
       //Set the value of the variable at a specific location
   glUniformMatrix4fv(PVM_loc, 1, false, glm::value_ptr(PVM));
  
   glBindVertexArray(scene::mesh2.mVao);
   scene::mesh2.DrawMesh();

   // mesh3: santa
   // -------------
   glBindTexture(GL_TEXTURE_2D, scene::tex_id3);
   glUniform1i(tex_loc, 0);

   // Santa's transformation matrix (MVP)
   PVM = P * V3 * M3;
   if (PVM_loc != -1) {
       glUniformMatrix4fv(PVM_loc, 1, false, glm::value_ptr(PVM));
   }

   // Bind Santa's VAO and draw the mesh
   glBindVertexArray(scene::mesh3.mVao);
   scene::mesh3.DrawMesh();

   // mesh4: board
   // -------------
   glBindTexture(GL_TEXTURE_2D, scene::tex_id4);
   glUniform1i(tex_loc, 0);

   // Santa's transformation matrix (MVP)
   PVM = P * V4 * M4;
   if (PVM_loc != -1) {
       glUniformMatrix4fv(PVM_loc, 1, false, glm::value_ptr(PVM));
   }

   // Bind Santa's VAO and draw the mesh
   glBindVertexArray(scene::mesh4.mVao);
   scene::mesh4.DrawMesh();

   draw_gui(window);

   // Swap front and back buffers
   glfwSwapBuffers(window);
}

void idle()
{
   float time_sec = static_cast<float>(glfwGetTime());

   if (scene::santa_animation_active)
   {
       switch (scene::santa_animation_state)
       {
       case scene::MoveUp:
           // 向上移动
           scene::santa_position.y += 0.05f;
           scene::santa_animation_time += 0.01f;

           if (scene::santa_animation_time >= 0.1f) // 上升 1 秒后进入暂停状态
           {
               scene::santa_animation_time = 0.0f;
               scene::santa_animation_state = scene::PauseAndRotate;
               scene::santa_pause_time = 3.0f; // 暂停 3 秒
           }
           break;

       case scene::PauseAndRotate:
           // 在最高点暂停并旋转
           if (scene::santa_pause_time > 0.0f)
           {
               scene::santa_pause_time -= 0.01f;

               // 控制旋转角度在 0° 到 60° 之间摆动
               scene::santa_pause_rotation += 1.0f * scene::santa_rotation_direction; // 根据方向更新角度

               if (scene::santa_pause_rotation >= 60.0f)
               {
                   scene::santa_pause_rotation = 60.0f; // 限制最大角度
                   scene::santa_rotation_direction = -1; // 改变方向
               }
               else if (scene::santa_pause_rotation <= 0.0f)
               {
                   scene::santa_pause_rotation = 0.0f; // 限制最小角度
                   scene::santa_rotation_direction = 1; // 改变方向
               }
           }
           else
           {
               // 暂停结束，进入向下移动状态
               scene::santa_animation_state = scene::MoveDown;
               scene::santa_pause_rotation = 0.0f; // 重置旋转角度
           }
           break;

       case scene::MoveDown:
           // 向下移动
           scene::santa_position.y -= 0.05f;
           scene::santa_animation_time += 0.01f;

           if (scene::santa_animation_time >= 0.1f) // 下降 1 秒后进入暂停状态
           {
               scene::santa_animation_time = 0.0f;
               scene::santa_animation_state = scene::Pause;
               scene::santa_pause_time = 3.0f; // 暂停 3 秒
           }
           break;

       case scene::Pause:
           // 在最低点暂停，不旋转
           if (scene::santa_pause_time > 0.0f)
           {
               scene::santa_pause_time -= 0.01f;
           }
           else
           {
               // 暂停结束，重新进入向上移动状态
               scene::santa_animation_state = scene::MoveUp;
           }
           break;
       }
   }


   //Pass time_sec value to the shaders
   int time_loc = glGetUniformLocation(scene::shader, "time");
   if (time_loc != -1)
   {
      glUniform1f(time_loc, time_sec);
   }
}

void reload_shader()
{
   std::string vs = scene::shader_dir + scene::vertex_shader;
   std::string fs = scene::shader_dir + scene::fragment_shader;

   GLuint new_shader = InitShader(vs.c_str(), fs.c_str());

   if (new_shader == -1) // loading failed
   {
      glClearColor(1.0f, 0.0f, 1.0f, 0.0f); //change clear color if shader can't be compiled
   }
   else
   {
      glClearColor(window::clear_color[0], window::clear_color[1], window::clear_color[2], window::clear_color[3]);

      if (scene::shader != -1)
      {
         glDeleteProgram(scene::shader);
      }
      scene::shader = new_shader;
   }
}

//This function gets called when a key is pressed
void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   std::cout << "key : " << key << ", " << char(key) << ", scancode: " << scancode << ", action: " << action << ", mods: " << mods << std::endl;

   if (action == GLFW_PRESS)
   {
      switch (key)
      {
      case 'r':
      case 'R':
         reload_shader();
         break;

      case GLFW_KEY_ESCAPE:
         glfwSetWindowShouldClose(window, GLFW_TRUE);
         break;
      }
   }
}

//This function gets called when the mouse moves over the window.
void mouse_cursor(GLFWwindow* window, double x, double y)
{
   //std::cout << "cursor pos: " << x << ", " << y << std::endl;
}

//This function gets called when a mouse button is pressed.
void mouse_button(GLFWwindow* window, int button, int action, int mods)
{
   //std::cout << "button : "<< button << ", action: " << action << ", mods: " << mods << std::endl;
}

//Initialize OpenGL state. This function only gets called once.
void init()
{
   glewInit();
   RegisterDebugCallback();

   std::ostringstream oss;
   //Get information about the OpenGL version supported by the graphics driver.	
   oss << "Vendor: "       << glGetString(GL_VENDOR)                    << std::endl;
   oss << "Renderer: "     << glGetString(GL_RENDERER)                  << std::endl;
   oss << "Version: "      << glGetString(GL_VERSION)                   << std::endl;
   oss << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION)  << std::endl;

   //Output info to console
   std::cout << oss.str();

   //Output info to file
   std::fstream info_file("info.txt", std::ios::out | std::ios::trunc);
   info_file << oss.str();
   info_file.close();

   //Set the color the screen will be cleared to when glClear is called
   glClearColor(window::clear_color[0], window::clear_color[1], window::clear_color[2], window::clear_color[3]);

   glEnable(GL_DEPTH_TEST);

   reload_shader();
   
   scene::mesh1 = LoadMesh(scene::asset_dir + scene::mesh_name1);
   scene::tex_id1 = LoadTexture(scene::asset_dir + scene::tex_name1);
   scene::mesh2 = LoadMesh(scene::asset_dir + scene::mesh_name2);
   scene::tex_id2 = LoadTexture(scene::asset_dir + scene::tex_name2);
   scene::mesh3 = LoadMesh(scene::asset_dir + scene::mesh_name3);
   scene::tex_id3 = LoadTexture(scene::asset_dir + scene::tex_name3);
   scene::mesh4 = LoadMesh(scene::asset_dir + scene::mesh_name4);
   scene::tex_id4 = LoadTexture(scene::asset_dir + scene::tex_name4);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Adjust the viewport to the new window dimensions
    glViewport(0, 0, width, height);

    // Update the aspect ratio for the projection matrix
    aspectRatio = (float)width / (float)height;

    glm::mat4 projection = glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
}


// C++ programs start executing in the main() function.
int main(void)
{
   GLFWwindow* window;

   // Initialize the library
   if (!glfwInit())
   {
      return -1;
   }

#ifdef _DEBUG
   glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

   // Create a windowed mode window and its OpenGL context
   window = glfwCreateWindow(window::size[0], window::size[1], window::title, NULL, NULL);
   if (!window)
   {
      glfwTerminate();
      return -1;
   }

   //Register callback functions with glfw. 
   glfwSetKeyCallback(window, keyboard);
   glfwSetCursorPosCallback(window, mouse_cursor);
   glfwSetMouseButtonCallback(window, mouse_button);
   glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

   // Make the window's context current
   glfwMakeContextCurrent(window);

   init();

   // New in Lab 2: Init ImGui
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGui_ImplGlfw_InitForOpenGL(window, true);
   ImGui_ImplOpenGL3_Init("#version 150");
   // Loop until the user closes the window 
   while (!glfwWindowShouldClose(window))
   {
      idle();
      display(window);

      // Poll for and process events 
      glfwPollEvents();
   }

   // New in Lab 2: Cleanup ImGui
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplGlfw_Shutdown();
   ImGui::DestroyContext();

   glfwTerminate();
   return 0;
}