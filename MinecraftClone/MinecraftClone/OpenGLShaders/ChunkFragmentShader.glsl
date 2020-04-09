#version 330 core

out vec4 color;
uniform sampler2DArray uTexture;
uniform vec3 uFogColor;

in vec3 vTextCoord;
in float vlightIntensity;
in float vVisibility;

void main()
{
	color = vlightIntensity * texture(uTexture, vTextCoord);
	if (color.a == 0)
	{
		discard;
	}

	color = mix(vec4(uFogColor, 1.0), color, vVisibility);
};