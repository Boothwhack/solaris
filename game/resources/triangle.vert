#version 440 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec4 a_Color;

out vec4 f_Color;

layout(std140, binding = 0) uniform Transforms {
    mat4 projection;
} u_Transforms;

void main() {
    gl_Position = vec4(a_Position, 1.0) * u_Transforms.projection;
    f_Color = a_Color;
}
