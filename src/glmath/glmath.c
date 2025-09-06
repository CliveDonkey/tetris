#include <stdio.h>
#include <math.h>
#include "glmath.h"

/*int main() {

  vec4 vec = {{1.0f, 0.0f, 0.0f, 1.0f}};
  mat4 trans = mat4init(1.0f);
  trans = mat4rotate(trans, (vec3){{0.0f, 0.0f, 1.0f}}, M_PI_2);
  trans = mat4scale(trans, (vec3){{0.5f, 0.5f, 0.5f}});

  vec4 result = matvec4mult(trans, vec);

  printf("result is: %f\t%f\t%f\n", result.data[0], result.data[1], result.data[2]);
}*/

vec2 vec2init(float x, float y) {
  vec2 result = {{x, y}};
  return result;
}

vec3 vec3init(float x, float y, float z) {
  vec3 result = {{x, y, z}};
  return result;
}

vec4 vec4init(float x, float y, float z, float w) {
  vec4 result = {{x, y, z, w}};
  return result;
}

int vec2equal(vec2 a, vec2 b) {
  int result = 1;
  for (int i = 0; i < 2; i++) {
    if (a.data[i] != b.data[i]) {
       result = 0;
       break;
    }
  }
  return result;
}

int vec3equal(vec3 a, vec3 b) {
  int result = 1;
  for (int i = 0; i < 3; i++) {
    if (a.data[i] != b.data[i]) {
       result = 0;
       break;
    }
  }
  return result;
}
int vec4equal(vec4 a, vec4 b) {
  int result = 1;
  for (int i = 0; i < 4; i++) {
    if (a.data[i] != b.data[i]) {
       result = 0;
       break;
    }
  }
  return result;
}

mat2 mat2init(float diagonal) {
  mat2 result = {{{diagonal, 0.0f}, {0.0f, diagonal}}};
  return result;
}

mat3 mat3init(float diagonal) {
  mat3 result = {{{diagonal, 0.0f, 0.0f}, {0.0f, diagonal, 0.0f}, {0.0f, 0.0f, diagonal}}};
  return result;
}

mat4 mat4init(float diagonal) {
  mat4 result = {{{diagonal, 0.0f, 0.0f, 0.0f},
  	{0.0f, diagonal, 0.0f, 0.0f},
	{0.0f, 0.0f, diagonal, 0.0f},
	{0.0f, 0.0f, 0.0f, diagonal}}};
  return result;
}

float vec2dot(vec2 a, vec2 b) {
  return a.data[0]*b.data[0] + a.data[1]*b.data[1];
}

float vec3dot(vec3 a, vec3 b) {
  return a.data[0]*b.data[0] + a.data[1]*b.data[1] + a.data[2]*b.data[2];
}

float vec4dot(vec4 a, vec4 b) {
  return a.data[0]*b.data[0] + a.data[1]*b.data[1] + a.data[2]*b.data[2] + a.data[3]*b.data[3];
}

vec3 vec3cross(vec3 a, vec3 b) {
  vec3 result;

  result.data[0] = a.data[1]*b.data[2] - a.data[2]*b.data[1];
  result.data[1] = a.data[2]*b.data[0] - a.data[0]*b.data[2];
  result.data[2] = a.data[0]*b.data[1] - a.data[1]*b.data[0];

  return result;
}

vec2 matvec2mult(mat2 mat, vec2 vec) {
  vec2 result;
  for (int i = 0; i < 2; i++) {
    result.data[i] = 0;
    for (int j = 0; j < 2; j++) {
      result.data[i] += mat.data[i][j] * vec.data[j];
    }
  }

  return result;
}

vec3 matvec3mult(mat3 mat, vec3 vec) {
  vec3 result;
  for (int i = 0; i < 3; i++) {
    result.data[i] = 0;
    for (int j = 0; j < 3; j++) {
      result.data[i] += mat.data[i][j] * vec.data[j];
    }
  }

  return result;
}

vec4 matvec4mult(mat4 mat, vec4 vec) {
  vec4 result;
  for (int i = 0; i < 4; i++) {
    result.data[i] = 0;
    for (int j = 0; j < 4; j++) {
      result.data[i] += mat.data[i][j] * vec.data[j];
    }
  }

  return result;
}

mat2 mat2mult(mat2 a, mat2 b) {
  mat2 result;
  
  for (int i=0; i < 2; i++) {
    for (int j=0; j < 2; j++) {
      result.data[i][j] = 0;
      for (int k = 0; k < 2; k++) {
        result.data[i][j] += a.data[i][k] * b.data[k][j];
      }
    }
  }

  return result;
}

mat3 mat3mult(mat3 a, mat3 b) {
  mat3 result;

  for (int i=0; i < 3; i++) {
    for (int j=0; j < 3; j++) {
      result.data[i][j] = 0;
      for (int k = 0; k < 3; k++) {
        result.data[i][j] += a.data[i][k] * b.data[k][j];
      }
    }
  }

  return result;
}

mat4 mat4mult(mat4 a, mat4 b) {
  mat4 result;
  
  for (int i=0; i < 4; i++) {
    for (int j=0; j < 4; j++) {
      result.data[i][j] = 0;
      for (int k = 0; k < 4; k++) {
        result.data[i][j] += a.data[i][k] * b.data[k][j];
      }
    }
  }

  return result;
}

mat4 mat4translate(mat4 mat, vec3 translate) {
  mat4 transMatrix = mat4init(1.0f);
  for (int i = 0; i < 3; i++) {
    transMatrix.data[3][i] = translate.data[i];
  }
  transMatrix = mat4mult(transMatrix, mat);
  return transMatrix;
}

mat4 mat4rotate(mat4 mat, vec3 axis, float rads) {
  float sr = sinf(rads);
  float cs = cosf(rads);

  float x = axis.data[0];
  float y = axis.data[1];
  float z = axis.data[2];

  mat4 rotMatrix = {{
    {cs + x*x * (1 - cs), x*y*(1-cs) + z*sr,   x*z*(1-cs) - y*sr,  0.0f},
    {x*y*(1-cs) - z*sr,   cs + y*y * (1 - cs), y*z*(1-cs) + x*sr,  0.0f},
    {x*z*(1-cs) + y*sr,   y*z*(1-cs) - x*sr,   cs + z*z * (1 - cs), 0.0f},
    {0.0f,		  0.0f,		       0.0f, 		    1.0f}}};

  rotMatrix = mat4mult(rotMatrix, mat);
  return rotMatrix;
}

mat4 mat4scale(mat4 mat, vec3 coficcents) {
  mat4 transMatrix = mat4init(1.0f);
  for (int i = 0; i < 3; i++) {
    transMatrix.data[i][i] = coficcents.data[i];
  }
  transMatrix = mat4mult(transMatrix, mat);
  return transMatrix;
}
