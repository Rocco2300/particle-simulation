#version 440

#define MAX_PARTICLES 4096

in vec3 vertexNormal;
in vec3 vertexPosition;
in vec2 vertexTexCoord;

out vec4 diffuseColor;

uniform mat4 view;
uniform mat4 projection;

uniform int count;

layout(std430, binding = 0) buffer positions {
    float position[MAX_PARTICLES][3];
};

layout(std430, binding = 1) buffer colors {
    float color[MAX_PARTICLES][4];
};

vec3 getPosition(int index) {
    return vec3(position[index][0], position[index][1], position[index][2]);
}

vec4 getColor(int index) {
    return vec4(color[index][0], color[index][1], color[index][2], color[index][3]);
}

mat4 getModel(int index) {
    vec3 position = getPosition(index);

    mat4 scale = mat4(1);
    scale[0][0] = 0.25;
    scale[1][1] = 0.25;
    scale[2][2] = 0.25;

    mat4 translate = mat4(1);
    translate[3][0] = position.x;
    translate[3][1] = position.y;
    translate[3][2] = position.z;

    return translate * scale;
}

void main() {
    mat4 mvp = projection * view * getModel(gl_InstanceID);
    gl_Position = mvp * vec4(vertexPosition, 1.0);

    diffuseColor = getColor(gl_InstanceID);
}
