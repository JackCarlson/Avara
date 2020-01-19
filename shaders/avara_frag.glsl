#version 330 core

in vec3 fragmentColor;
in vec3 fragmentNormal;

uniform vec3 light0 = vec3(0, 45, 20);
uniform vec3 light1 = vec3(0, 20, 200);
uniform vec3 light2 = vec3(0, 0, 0);
uniform vec3 light3 = vec3(0, 0, 0);
uniform float ambient = 0;
uniform float lights_active = 1.0;
uniform float decal;

out vec3 color;

vec3 lightColor = vec3(1, 1, 1);

vec3 to_cartesian (in float azimuth, in float elevation)
{
    return vec3(sin(azimuth) * cos(elevation),
                -sin(elevation),
                -cos(azimuth) * cos(elevation));
}

vec3 calc_light (vec3 light, vec3 normal) {
    vec3 lightPos = normalize(normal - to_cartesian(radians(light.z), radians(light.y)));
    return max(dot(normal, lightPos), 0.0) * lightColor * light.x;
}

void main()
{
    vec3 normal = fragmentNormal * mix(-1.0, 1.0, gl_FrontFacing);
    color = mix(
            ambient * lightColor * fragmentColor,
            ((ambient * lightColor) + (calc_light(light0, normal) 
                                    + calc_light(light1, normal)
                                    + calc_light(light2, normal)
                                    + calc_light(light3, normal))) * fragmentColor,
            lights_active);

    gl_FragDepth = gl_FragCoord.z * decal;
}
