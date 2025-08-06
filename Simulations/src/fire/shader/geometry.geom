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
    vec3 cameraRight = normalize(vec3(view[0][0], view[1][0], view[2][0]));
    vec3 cameraUp = normalize(vec3(view[0][1], view[1][1], view[2][1]));

    vec4 pos = gl_in[0].gl_Position;
    float particleSize = gs_in[0].size;
    gl_Position = mvpMatrix * vec4((pos.xyz - cameraRight - cameraUp) * particleSize, 1.0f);
    ageColor = gs_in[0].color;
    rotation = gs_in[0].rotation;
    TexCoords = vec2(0.0f, 0.0f);
    EmitVertex();

    gl_Position = mvpMatrix * vec4((pos.xyz - cameraRight + cameraUp) * particleSize, 1.0f);
    TexCoords = vec2(0.0f, 1.0f);
    EmitVertex();

    gl_Position = mvpMatrix * vec4((pos.xyz + cameraRight - cameraUp) * particleSize, 1.0f);
    TexCoords = vec2(1.0f, 0.0f);
    EmitVertex();

    gl_Position = mvpMatrix * vec4((pos.xyz + cameraRight + cameraUp) * particleSize, 1.0f);
    TexCoords = vec2(1.0f, 1.0f);
    EmitVertex();

    EndPrimitive();
}  