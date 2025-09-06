#ifndef GL_MATH_H
#define GL_MATH_H

typedef struct {float data[2];} vec2;

typedef struct {float data[3];} vec3;

typedef struct {float data[4];} vec4;

typedef struct {float data[2][2];} mat2;

typedef struct {float data[3][3];} mat3;

typedef struct {float data[4][4];} mat4;

vec2 vec2init(float x, float y);
vec3 vec3init(float x, float y, float z);
vec4 vec4init(float x, float y, float z, float w);

int vec2equal(vec2 a, vec2 b);
int vec3equal(vec3 a, vec3 b);
int vec4equal(vec4 a, vec4 b);

mat2 mat2init(float diagonal);
mat3 mat3init(float diagonal);
mat4 mat4init(float diagonal);

float vec2dot(vec2 a, vec2 b);
float vec3dot(vec3 a, vec3 b);
float vec4dot(vec4 a, vec4 b);

vec3 vec3cross(vec3 a, vec3 b);

vec2 matvec2mult(mat2 mat, vec2 vec);
vec3 matvec3mult(mat3 mat, vec3 vec);
vec4 matvec4mult(mat4 mat, vec4 vec);

mat2 mat2mult(mat2 a, mat2 b);
mat3 mat3mult(mat3 a, mat3 b);
mat4 mat4mult(mat4 a, mat4 b);

mat4 mat4translate(mat4 mat, vec3 translate);
mat4 mat4rotate(mat4 mat, vec3 axis, float rads);
mat4 mat4scale(mat4 mat, vec3 coficcents);

#endif
