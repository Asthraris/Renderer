#version 430 core

layout(location = 0) in vec3 aPos;   // Position attribute
layout(location = 1) in vec3 aColor; // Color attribute

out vec3 vertexColor;  // Output to fragment shader

void main() {
    gl_Position = vec4(aPos, 1.0);  // Directly pass the position to gl_Position
    vertexColor = aColor;           // Pass the color to the fragment shader
}