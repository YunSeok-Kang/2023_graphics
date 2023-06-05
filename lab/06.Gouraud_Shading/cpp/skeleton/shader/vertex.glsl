#version 120                  // GLSL 1.20

attribute vec3 a_position;    // per-vertex position (per-vertex input)
attribute vec3 a_normal;

uniform mat4 u_PVM;

// for phong shading
// TODO define uniform variables

varying	  vec3 v_color;

// reflection coefficients
uniform vec3 u_obj_ambient;
uniform vec3 u_obj_diffuse;
uniform float u_obj_shininess;

// light color information
uniform vec3 u_light_ambient;
uniform vec3 u_light_diffuse;
uniform vec3 u_light_specular;
uniform vec3 u_light_position;

uniform mat4 u_model_matrix;
uniform mat3 u_normal_matrix;

uniform vec3 u_camera_position;

vec3 directional_light()
{
	vec3 color = vec3(0.0);

	// TODO compute phong reflection model
	// 1) ambient color
	color += (u_obj_ambient * u_light_ambient);

	// 22) diffuse color
	// normal: per-vertex data(-> attribute)
	// light_dir: per-scene data(-> uniform)
	vec3 light_dir = normalize(u_light_position);// - a_position)
	vec3 normal_wc = normalize(u_normal_matrix * a_normal);
	float ldotn = max(dot(light_dir, normal_wc), 0.0);
	color += (u_obj_diffuse * ldotn * u_light_diffuse);

	// 3) specular color
	vec3 position_wc = u_model_matrix * vec4(a_position, 1.0f).xyz;
	vec3 view_dir = (u_camera_position - position_wc);
	vec3 reflect_dir = reflect(-light_dir, normal_wc);
	float rdotv = max(dot(reflect_dir, view_dir), 0);
	color += u_obj_specular * pow(rdotv, u_obj_shininess) * u_light_specular;

	return color;
}

void main()
{
  gl_Position = u_PVM * vec4(a_position, 1.0f);

  v_color = directional_light();
}