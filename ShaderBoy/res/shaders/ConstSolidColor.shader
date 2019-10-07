#shader vertex
#version 330 core

// 0 matches attrib index, vec4 auto converts incoming vec2
layout(location = 0) in vec4 position;

void main()
{
	gl_Position = position; // could have to vec4(position.xy);
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

void main()
{
	color = vec4(0.2, 0.3, 0.8, 1.0);
}
