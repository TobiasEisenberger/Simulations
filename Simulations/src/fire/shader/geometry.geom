#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in VS_OUT {
    vec4 color;
    float size;
    float rotation;
} gs_in[];

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 ageColor;
out vec2 TexCoords;
out float rotation;

void main() {    
    mat4 mvpMatrix = projection * view * model;

    vec4 pos = gl_in[0].gl_Position;
    float particleSize = gs_in[0].size;
    gl_Position = mvpMatrix * (pos + vec4(-1.0f, -1.0f, 0.0f, 0.0f) * particleSize);
    ageColor = gs_in[0].color;
    rotation = gs_in[0].rotation;
    TexCoords = vec2(0.0f, 0.0f);
    EmitVertex();

    gl_Position = mvpMatrix * (pos + vec4(-1.0f, 1.0f, 0.0f, 0.0f) * particleSize);
    TexCoords = vec2(0.0f, 1.0f);
    EmitVertex();

    gl_Position = mvpMatrix * (pos + vec4(1.0f, -1.0f, 0.0f, 0.0f) * particleSize);
    TexCoords = vec2(1.0f, 0.0f);
    EmitVertex();

    gl_Position = mvpMatrix * (pos + vec4(1.0f, 1.0f, 0.0f, 0.0f) * particleSize);
    TexCoords = vec2(1.0f, 1.0f);
    EmitVertex();

    EndPrimitive();
}  