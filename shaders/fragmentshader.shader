#version 330 core
in vec2 UV;
out vec4 color;
uniform sampler2D myTextureSampler;
uniform float multiplier;
void main()
{
	color.rgb = multiplier*texture(myTextureSampler, UV).rbg;

}