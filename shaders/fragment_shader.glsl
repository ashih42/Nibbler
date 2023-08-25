#version 400 core

#define				PI					3.141592653589793238462643383279
#define 			UNDEFINED_VALUE		-42.0f

in vec3				v_position;
in vec3				position_transformed;
in vec2				vt_tex_coords;
in vec3				normal;

out vec4			frag_color;

uniform sampler2D	current_texture;

uniform bool		is_shaded;

uniform vec3		light_position;

vec2				compute_uv()
{
	vec3			position;

	position = normalize(v_position);
	return vec2(
		0.5 - atan(position.z, position.x) / (2.0 * PI),
		0.5 - asin(position.y) / PI);
}

float				compute_diffuse_intensity()
{
	vec3			vertex_to_light_source;
	float			intensity;

	vertex_to_light_source = light_position - position_transformed;
	vertex_to_light_source = normalize(vertex_to_light_source);
	intensity = dot(vertex_to_light_source, normal);
	intensity = clamp(intensity, 0.0, 1.0);
	return intensity;
}

vec4				compute_shaded_color(vec4 color)
{
	return compute_diffuse_intensity() * color;
}

void main()
{
	vec4	textured_color;
	
	textured_color = texture(current_texture,
		(vt_tex_coords.x == UNDEFINED_VALUE) ? compute_uv() : vt_tex_coords);

	frag_color = textured_color;

	if (is_shaded)
		frag_color = compute_shaded_color(frag_color);
}
