#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 model; 
uniform mat4 view; 
uniform mat4 projection; 
uniform vec3 color; 

out vec3 fColor;
out vec4 fPosition;

void main()
{
    fColor = color;
    gl_Position = projection * view * model * vec4(position, 1.0f);
} 