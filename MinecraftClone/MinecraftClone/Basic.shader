#shader Vertex
#version 330 core

layout(location = 0) in vec3 position;

void main()
{
	gl_Position = vec4(position.x, position.y, position.z, 1.0);
};

#shader Fragment
#version 330 core

out vec4 color;
uniform vec4 uColor;

void main()
{
	color = uColor;
};