#version 460 core
in vec3 normal;
in vec2 texcoord;

uniform sampler2D tex_albedo;
uniform sampler2D tex_emission;
uniform sampler2D tex_metal_rough;
uniform sampler2D tex_normal;
uniform sampler2D tex_occlusion;

//uniform vec3 sun_position;
//uniform vec3 sun_color;

void main() {
    vec3 sun_position = vec3(3.0, 10.0, -5.0);
    vec3 sun_color = vec3(1.0);

    // Lambertian diffuse reflection
    vec3 albedo = texture(tex_albedo, texcoord).rgb;
    vec3 normal_map = normalize(texture(tex_normal, texcoord).rgb * 2.0 - 1.0);
    vec3 light_dir = normalize(sun_position - world_pos.xyz);
    float diffuse_factor = max(dot(normal_map, light_dir), 0.0);
    vec3 diffuse_color = albedo * diffuse_factor * sun_color;

    vec3 view_dir = normalize(-world_pos.xyz);
    vec3 reflect_dir = reflect(-light_dir, normal_map);
    float specular_factor = pow(max(dot(view_dir, reflect_dir), 0.0), 32.0);
    vec3 specular_color = vec3(0.5) * specular_factor * sun_color;

    vec3 final_color = diffuse_color + specular_color;

    float ambient_occlusion = texture(tex_occlusion, texcoord).r;
    final_color *= ambient_occlusion;

    final_color += texture(tex_emission, texcoord).rgb;

    FragColor = vec4(final_color, 1.0);
}
