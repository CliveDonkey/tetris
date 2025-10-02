#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "shader.h"
#include "stb_image.h"
#include "glmath/glmath.h"

//Tetris code, might become own file
//defines shtuff like a piece, togeteher with its collision detector etc
enum direction{
  left,
  down,
  up,
  right
};

struct square {
  int x;
  vec4 color;
  int y;
};

struct gameState {
  int rotation;
  int ypos;
  int xpos;
  float time;
  float prevTime;
  int ticks;
  vec4 backColor;
  vec4 gameBoard[20][10];
  struct square (*shape);
  int score;
};

int addShape(int y, int x, vec4 (*gameBoard)[20][10], struct square *shape);
int drawShape(int y, int x, struct square *shape, mat4 aspectRatioScale, int transformLoc, int squareColor);
int collisionDetector(const int y, const int x, vec4 defaultColor, vec4 (*gameBoard)[20][10], struct square *shape, int rotation);
void removeLine(struct gameState *state);

//colors:
const vec4 red = (vec4){{1.0f, 0.0f, 0.0f, 1.0f}};
const vec4 yellow = (vec4){{1.0f, 1.0f, 0.0f, 1.0f}};
const vec4 turquise = (vec4){{0.2f, 0.4f, 1.0f, 1.0f}};
const vec4 purple = (vec4){{0.8f, 0.0f, 0.8f, 1.0f}};

//global variables:
float windowScale = 0.5f;

const int shapeCount = 3;
const struct square shapes[][4] = 
  {
    //T
    {{-1, purple, 0}, {0, purple, 0}, {1, purple, 0}, {0, purple , -1}},      //t up
    {{0, purple, 1}, {0, purple, 0}, {0, purple, -1}, {1, purple, 0}},         //t right
    {{-1, purple, 0}, {0, purple, 0}, {1, purple, 0}, {0, purple, 1}},         //t down
    {{0, purple, 1}, {0, purple, 0}, {0, purple, -1}, {-1, purple, 0}},         //t left
    //L
    {{0, turquise, -1}, {0, turquise, 0}, {0, turquise, 1}, {1, turquise, 1}}, //L-up
    {{-1, turquise, 0}, {0, turquise, 0}, {1, turquise, 0}, {-1, turquise, 1}}, //L-right
    {{0, turquise, -1}, {0, turquise, 0}, {0, turquise, 1}, {-1, turquise, -1}}, //L-down
    {{-1, turquise, 0}, {0, turquise, 0}, {1, turquise, 0}, {1, turquise, -1}}, //L-left
    //square
    {{0, red, 0}, {0, red, 1}, {1, red, 0}, {1, red, 1}},
    {{0, red, 0}, {0, red, 1}, {1, red, 0}, {1, red, 1}},
    {{0, red, 0}, {0, red, 1}, {1, red, 0}, {1, red, 1}},
    {{0, red, 0}, {0, red, 1}, {1, red, 0}, {1, red, 1}}
  };

//openGL section, drawing shit
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

int main(int argc, char** argv) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    printf("Failed to create GLFW window");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD");
    return -1;
  }
  
  glViewport(0, 0, 800, 600);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetKeyCallback(window, key_callback);

  unsigned int shaderProgram = makeShader("shaders/vert.vs", "shaders/frag.fs");

  //defining triangle corners:
  float vertices[] = {
    //positions            colors
    -1.0f, -1.0f, 0.0f,   1.0f, 0.0f, 0.0f,    //bottom left
     1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,    //bottom right
    -1.0f,  1.0f, 0.0f,   0.0f, 1.0f, 0.0f,    //top left
     1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 1.0f,    //top right
  };

  unsigned int indices[] = {
    0, 1, 2,  //first triangle
    1, 2, 3   //second triange
  };

  //creating new VBO:
  unsigned int VBOs[1];
  glGenBuffers(1, VBOs);

  //creating EBO:
  unsigned int EBOs[1];
  glGenBuffers(1, EBOs);

  //creating newVAO
  unsigned int VAOs[1];
  glGenVertexArrays(1, VAOs);

  glBindVertexArray(VAOs[0]);

  //sets vertex atrib pointer:
  //binding the buffer and adding the vertices data to it:
  glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  glBindVertexArray(0);

  glUseProgram(shaderProgram);
  //global game-state variables
  struct gameState state = {
    .rotation = 0,
    .ypos = 5,
    .xpos = 5,
    .time = glfwGetTime(),
    .prevTime = glfwGetTime(),
    .ticks = 0,
    .backColor = (vec4){{0.1f, 0.1f, 0.1f, 1.0f}},
    .shape = &shapes[4 * (shapeCount - 1)],
    .score = 0
  };
  
  glfwSetWindowUserPointer(window, &state);

  for (int i = 0; i < 20; ++i) {
    for (int j = 0; j < 10; ++j) {
      state.gameBoard[i][j] = state.backColor;
    }
  }

  //setting transform for vertex-shader:
  unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
  unsigned int squareColor = glGetUniformLocation(shaderProgram, "color");

  //Renderloop ahead:
  while(!glfwWindowShouldClose(window)) {
    processInput(window);

    //clear image with uniform color:
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    mat4 aspectRatioScale = mat4init(1.0f);
    mat4 invAspScale = mat4init(1.0f);
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    const float aspectRatio = 0.5f;
    float aspRatioRatio = ((float)width/(float)height)/aspectRatio;
    if (aspRatioRatio >= 1.0f) {
      aspectRatioScale = mat4scale(aspectRatioScale, (vec3){{1.0f/aspRatioRatio, 1.0f, 1.0f}});
    } else {
      aspectRatioScale = mat4scale(aspectRatioScale, (vec3){{1.0f, aspRatioRatio, 1.0f}});
    }
    if (aspectRatio >= 1.0f) {
      invAspScale = mat4scale(aspectRatioScale, (vec3){{1.0f/aspectRatio, 1.0f, 1.0f}});
    } else {
      invAspScale = mat4scale(aspectRatioScale, (vec3){{1.0f, aspectRatio, 1.0f}});
    }

    mat4 backgroundTransform = mat4init(1.0f);
    backgroundTransform = mat4mult(aspectRatioScale, backgroundTransform);
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, backgroundTransform.data);
    glUniform4f(squareColor, 0.0f, 0.0f, 0.0f, 1.0f);
    
    glBindVertexArray(VAOs[0]);
    //draw elements and swap buffer
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    //glDrawArrays(GL_TRIANGLES, 0, 3);

    //matrix transform to rotate move and scale container:
    mat4 trans = mat4init(1.0f);
    trans = mat4mult(aspectRatioScale, trans);
    trans = mat4rotate(trans, (vec3){{0.0f, 0.0f, 1.0f}}, 0.0f);
    trans = mat4scale(trans, (vec3){{0.09f, 0.045f, 0.2f}});
    
    //drawing the gameboard
    for (int i = 0; i < 20; ++i) {
      for (int j = 0; j < 10; ++j) {
        mat4 squareTrans = trans;
        squareTrans = mat4translate(trans, (vec3){{-10.0f + 2.22f * j, 21.0f - 2.22f * i, 0.0f}});
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, squareTrans.data);
        glUniform4fv(squareColor, 1, &state.gameBoard[i][j]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      }
    }

    //drawing moving shape
    drawShape(state.ypos, state.xpos, &state.shape[4*state.rotation], aspectRatioScale, transformLoc, squareColor);

    glBindVertexArray(0);

    glfwSwapBuffers(window);
    //poll for inputs:
    glfwPollEvents();

    //control rendering to happen at fixed intervals, and add in physics ticks
    const float frameTime = 10000.0f; //duration of frame in u-seconds
    const int framesPTick = 100;    //number of frames between physics ticks

    state.time = glfwGetTime();
    float waitTime = frameTime - 1000000.0f * (state.time - state.prevTime);
    waitTime = waitTime > 0 ? waitTime : 0.0f;
    usleep(waitTime);
    state.prevTime = state.time;

    //physics
    state.ticks++;
    if (state.ticks % framesPTick == 0) { //check to see if physics should be processed this frame.
      
        //check for wether current tetrinomicon will collide if moved down, if so, it should not be moved, but be added to the background.
      if (collisionDetector(state.ypos + 1, state.xpos, state.backColor, state.gameBoard, state.shape, state.rotation)) {
        //adds the shape to the gameboard
        addShape(state.ypos, state.xpos, state.gameBoard, &state.shape[4*state.rotation]);

        //reset state of piece, position orientation etc..
        state.ypos = 0;
        state.xpos = 5;
        state.rotation = 0;

        //randomly select what tetrinomicon should be used next, this is simpler but wrong
        state.shape = shapes[4 * ((int)(state.ticks) % shapeCount)];    //time in micro-seconds used as random number, gud nuff, but also wrong!

        //check to se if a row is full, if so, add score, and remove + do gravity
        removeLine(&state);
      } 
      else
        state.ypos = (state.ypos + 3) % 22 -2;
    }
  }

  glfwTerminate();
  return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

//processing input, this function is for reading state of keys right now
void processInput(GLFWwindow *window) {
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, 1);

  if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  if(glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    windowScale -= 0.03f;
    if (windowScale < 0.0f) windowScale = 0.0f;
  }
}

//processing registered events, this will automaticly debounce
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  struct gameState *state = (struct gameState*)glfwGetWindowUserPointer(window);

  if ((key == GLFW_KEY_UP || key == GLFW_KEY_Z) && action == GLFW_PRESS) {
    if (collisionDetector(state->ypos, state->xpos, state->backColor, &(state->gameBoard), state->shape, (state->rotation + 3) % 4)) return;
    state->rotation = (state->rotation + 3) % 4;
  }

  if (key == GLFW_KEY_X && action == GLFW_PRESS) {
    if (collisionDetector(state->ypos, state->xpos, state->backColor, &(state->gameBoard), state->shape, (state->rotation + 1) % 4)) return;
    state->rotation = (state->rotation + 1) % 4;
  }

  if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
    if (collisionDetector(state->ypos, state->xpos-1, (vec4){{0.1f, 0.1f, 0.1f, 1.0f}}, &(state->gameBoard), state->shape, state->rotation)) return;
    state->xpos--;
  }

  if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
    if (collisionDetector(state->ypos, state->xpos+1, (vec4){{0.1f, 0.1f, 0.1f, 1.0f}}, &(state->gameBoard), state->shape, state->rotation)) return;
    state->xpos++;
  }

  if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
    if (collisionDetector(state->ypos+1, state->xpos, (vec4){{0.1f, 0.1f, 0.1f, 1.0f}}, &(state->gameBoard), state->shape, state->rotation)) return;
    state->ypos++;
  }
}

int addShape(int y, int x, vec4 (*gameBoard)[20][10], struct square *shape) {
  for (int i = 0; i < 4; ++i) {
    int ypos = y+shape[i].y;
    int xpos = x+shape[i].x;

    if (ypos < 0 || 20 <= ypos || xpos < 0 || 10 <= xpos ) continue;
    (*gameBoard)[ypos][xpos] = shape[i].color;
  }

  return 0;
}

int drawShape(int y, int x, struct square *shape, mat4 aspectRatioScale, int transformLoc, int squareColor) {
  for (int i = 0; i < 4; ++i) {
    int ypos = y+shape[i].y;
    int xpos = x+shape[i].x;

    if (ypos < 0 || 20 <= ypos || xpos < 0 || 10 <= xpos ) continue;

    //matrix transform to rotate move and scale container:
    mat4 trans = mat4init(1.0f);
    trans = mat4mult(aspectRatioScale, trans);
    trans = mat4rotate(trans, (vec3){{0.0f, 0.0f, 1.0f}}, 0.0f);
    trans = mat4scale(trans, (vec3){{0.09f, 0.045f, 0.2f}});
    
    //drawing shape bacground
    mat4 backTrans = trans;
    backTrans = mat4translate(backTrans, (vec3){{-10.0f + 2.22f * xpos, 21.0f - 2.22f * ypos, 0.0f}});
    backTrans = mat4scale(backTrans, (vec3){{1.05f, 1.05f, 1.05f}});
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, &backTrans);
    glUniform4fv(squareColor, 1,  &(vec4){{1.0f, 1.0f, 1.0f, 1.0f}});
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    //drawing actual piece
    mat4 squareTrans = trans;
    squareTrans = mat4translate(trans, (vec3){{-10.0f + 2.22f * xpos, 21.0f - 2.22f * ypos, 0.0f}});
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, &squareTrans);
    glUniform4fv(squareColor, 1, &shape[i].color);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  }
  
  return 0;
}

int collisionDetector(const int y, const int x, vec4 defaultColor, vec4 (*gameBoard)[20][10], struct square *shape, int rotation) {
  //return codes:
  //0: no collison, 1: collides
  int result = 0;

  for (int i = 0; i < 4; ++i) {
    int ypos = y+(shape+4*rotation)[i].y;
    int xpos = x+(shape+4*rotation)[i].x;

    if (!vec4equal((*gameBoard)[ypos][xpos], defaultColor)) {
      result = 1;
      break;
    }

    if (ypos < 0 || 20 <= ypos || xpos < 0 || 10 <= xpos ) {
      result = 1;
      break;
    }
  }

  return result;
}

//checks for complete line, removes the complete lines and adds to score:
void removeLine(struct gameState *state) {
  for (int i = 0; i < 20; ++i) {
    int result = 1;

    for (int j = 0; j < 10; ++j) {
      if (vec4equal(state->backColor, state->gameBoard[i][j])) {
        result = 0;
        break;
      }
    }
    
    if (result) {
      void *dst = (void*) &(state->gameBoard[1][0]);
      void *src = (void*) &(state->gameBoard[0][0]);
      //maybe off-by-one, try gameBoard[i+1][0]:
      void *dataEnd = (void*) &(state->gameBoard[i+1][0]);
      size_t length = dataEnd - dst;
      memmove(dst, src, length);
      state->score++;
    }
  }
  printf("your score is: %i!\n", state->score);
}
