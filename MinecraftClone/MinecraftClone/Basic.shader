#shader Vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in Vec2 textCoord;

out vec2 vTextCoord;

void main()
{
	gl_Position = vec4(position.x, position.y, position.z, 1.0);
	vTextCoord = textCoord;
};

#shader Fragment
#version 330 core

in vec2 vTextCoord;
out vec4 color;
uniform vec4 uColor;
uniform sampler2D uTexture;

void main()
{
	vec4 textColor = texture(uTexture, vTextCoord);
	color = uColor;
};