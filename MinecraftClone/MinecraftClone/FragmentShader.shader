#version 330 core

out vec4 color;
uniform sampler2D uTexture;

in vec2 vTextCoord;

void main()
{
	vec4 textColor = texture(uTexture, vTextCoord);
	//color = vec4(textColor.rgb, 1.0);
	color = textColor;
	//color = vec4(vTextCoord,1.0,1.0);
};