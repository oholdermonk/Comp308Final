#version 330 core
in vec3 ourColor;
in vec3 lightCol;
in vec2 TexCoord;

out vec4 color;

uniform sampler2D ourTexture;
//uniform sampler2D extraTextures;

void main()
{
    //color = vec4(ourColor, 1.0f);

    color = texture(ourTexture, TexCoord)*vec4(ourColor, 1.0f);
}