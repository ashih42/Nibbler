#version 400 core

layout (location = 0) in vec3	position;
layout (location = 1) in vec2	tex_coords;
layout (location = 2) in vec3	vertex_normal;
layout (location = 3) in vec3	face_normal;

out	vec3						v_position;
out vec3						position_transformed;
out vec2						vt_tex_coords;
out vec3						normal;

uniform mat4					model_scale_matrix;
uniform mat4					model_rotate_matrix;
uniform mat4					model_translate_matrix;
uniform mat4					view_matrix;
uniform mat4					projection_matrix;

void main()
{
	gl_Position = projection_matrix * view_matrix *
		model_translate_matrix * model_rotate_matrix * model_scale_matrix * vec4(position, 1.0);

	v_position = position;
	
	position_transformed = (model_translate_matrix * model_rotate_matrix * model_scale_matrix * vec4(position, 1.0)).xyz;

	vt_tex_coords = vec2(tex_coords.x, -tex_coords.y);

	if (dot(vertex_normal, vertex_normal) == 0)
		normal = face_normal;
	else
		normal = vertex_normal;
	normal = (model_rotate_matrix * vec4(normal, 1.0)).xyz;
}
