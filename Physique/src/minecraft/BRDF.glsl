#version 330

#ifdef VERTEX_SHADER

layout(location= 0) in vec3 position;
layout(location= 1) in vec2 texcoord;
layout(location= 2) in vec3 normal;

uniform mat4 mvpMatrix;
uniform mat4 modelMatrix;

out vec2 vertex_texture;
out vec3 vertex_normal;
out vec3 vertex_position;

void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1);
    vertex_normal = mat3(modelMatrix) * normal;
    vertex_position = mat3(modelMatrix) * position;
    vertex_texture = texcoord;
}
#endif


#ifdef FRAGMENT_SHADER

in vec2 vertex_texture;
in vec3 vertex_normal;
in vec3 vertex_position;

const float ambient = 0.5;
const float diffus = 0.2;

uniform vec3 light;
uniform vec3 light_lux;
uniform vec3 obs;

uniform sampler2D texture_field;
uniform mat4 mvpMatrix;
uniform float m;

void main( )
{
    vec3 n_light = vec3(vec4(light, 1) * mvpMatrix);
    vec3 n_obs = vec3(vec4(obs, 1) * mvpMatrix);
    vec3 l = normalize(light - vertex_position);
    vec3 o = normalize(obs - vertex_position);
    vec3 h = normalize(l+o);
    vec3 n = normalize(vertex_normal);

    float fl = max(0.0, dot(n, l));
    float fh = max(0.0, dot(n, h));
    float spec = max(0.0, pow(fh, 3));

    vec4 color = texelFetch(texture_field, ivec2(vertex_texture), 0);

    float f = ambient + fl * (diffus + spec);
    gl_FragColor = color * f;
}
#endif
