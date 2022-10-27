#version 330
#extension GL_ARB_separate_shader_objects : enable

uniform mat4 combined_xform3;
uniform mat4 model_xform2;

layout (location=0) in vec3 vertex_position;
layout (location=1) in vec3 vertex_colour;

out vec3 varying_colour;

void main(void)
{
	varying_colour = vertex_colour;
	gl_Position = combined_xform3 * model_xform2 * vec4(vertex_position, 1.0);
}