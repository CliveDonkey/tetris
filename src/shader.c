#include <stdlib.h>
#include <stdio.h>
#include "glad/glad.h"
#include "shader.h"
#include <string.h>

char* readFile(char* filePath) {

  FILE* fp = fopen(filePath, "r");
  if (fp == NULL) {
    printf("Could not open file %s\n", filePath);
    return NULL;
  }

  fseek(fp, 0L, SEEK_END);
  int eofIndex = ftell(fp);
  rewind(fp);

  if (eofIndex <= 0) {
    printf("File %s to short\n", filePath);
    return NULL;
  }

  if (eofIndex > 1000000) {
    printf("File %s is too large!\n", filePath);
    return NULL;
  }
  
  char* fileString = malloc(eofIndex * sizeof(char));

  int bytesRead = 0;
  char buffer[4096];

  while (bytesRead < eofIndex) {
    memset(buffer, 0, sizeof(buffer));
    int currentRead = fread(buffer, sizeof(char), 4096, fp);
    memcpy(fileString + bytesRead, buffer, currentRead);
    bytesRead += currentRead;
  }
  fileString[eofIndex] = '\0';

  fclose(fp);

  return fileString;
}

unsigned int makeShader(char* vertexPath, char* fragmentPath) {
  const char* vertShader = readFile(vertexPath);
  const char* fragShader = readFile(fragmentPath);

  unsigned int vertexShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertShader, NULL);

  unsigned int fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragShader, NULL);

  int success;
  char infoLog[512];
  glCompileShader(vertexShader);
  
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    printf("Failed to compile shader %s with error\n%s\n", vertexPath, infoLog);
  }

  glCompileShader(fragmentShader);
  
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    printf("Failed to compile shader %s with error\n%s\n", fragmentPath, infoLog);
  }

  unsigned int shaderProgram;
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);

  glLinkProgram(shaderProgram);

  glGetShaderiv(shaderProgram, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shaderProgram, 512, NULL, infoLog);
    printf("Failed to link shaderprogram from %s\t%s\nError:\n%s\n", vertexPath, fragmentPath, infoLog);
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  free((void*)fragShader);
  free((void*)vertShader);

  return shaderProgram;
}
