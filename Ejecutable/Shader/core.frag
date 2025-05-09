#version 330 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D texture_diffuse;

void main()
{
    vec4 texColor = texture(texture_diffuse, TexCoords);
    color = texColor;
}
