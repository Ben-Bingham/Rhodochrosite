#version 330 core
layout (location = 0) in vec3 inputPositon;
layout (location = 1) in vec2 inputTextureCords;

out vec2 textureCordinates;

void main() {
    textureCordinates = inputTextureCords;

    gl_Position = vec4(inputPositon, 1.0);
}  