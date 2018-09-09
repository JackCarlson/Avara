#version 330 core

in vec3 tilt;

uniform vec3 groundColor;
uniform vec3 horizonColor;
uniform vec3 skyColor;

out vec3 color;

void main()
{
    float gradientHeight = 0.05;

    float phi = tilt.y * -1.0;
    if (phi <= 0.0) {
        color = groundColor;
    }
    else if (phi > gradientHeight) {
        color = skyColor;
    }
    else {
        float gradientValue = phi / gradientHeight;
        color = skyColor * gradientValue + horizonColor * (1.0 - gradientValue);
    }
}
