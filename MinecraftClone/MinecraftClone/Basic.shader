#shader Vertex
#version 330 core

layout(location = 0) in vec3 aPos; // the position variable has attribute position 0

void main()
{
	gl_Position = vec4(aPos, 1.0); // see how we directly give a vec3 to vec4's constructor
}

#shader Fragment
#version 330 core

out vec4 color;

void main()
{
	color = vec4(1.0, 0.0, 0.0, 1.0);
};