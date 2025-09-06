#version 330 core

out vec4 FragColor;

in vec2 texCoord;

uniform float mixConstant;

void main() {
	FragColor = vec4(0.2, 0.3, 0.3, 1.0);
}
