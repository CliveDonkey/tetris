#include "glmath/glmath.h"

typedef struct {
  unsigned int VAO;
  unsigned int VBO;
  unsigned int EBO;

  unsigned int* shaderPrograms;

  mat4 orientation;
  mat4 position;
  mat4 size;
  vec4 color;

  unsigned int vertex_number;
} gameObject;
