#version 120
// layout (location = 0) in vec3 aPos;
// layout (location = 1) in vec2 aTexCoords;
// layout (location = 2) in vec3 aNormal;

varying vec2 TexCoords;
varying vec3 WorldPos;
varying vec3 Normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{	

    TexCoords = gl_MultiTexCoord0.xy;
    //vec2 temp = aTexCoords;
    //vec4 amt = texture(normalMap, temp);
    WorldPos = vec3(model * gl_Vertex);
    Normal = mat3(model) * gl_Normal;   

    gl_Position =  projection * view * vec4(WorldPos, 1.0);
}