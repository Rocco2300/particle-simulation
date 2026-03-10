#version 440

#define MAX_PARTICLES 5000

in vec3 vertexNormal;
in vec3 vertexPosition;
in vec2 vertexTexCoord;

out int instanceId;
out vec4 diffuseColor;

uniform mat4 view;
uniform mat4 projection;

uniform int count;

layout(std430, binding=1) buffer radii {
    float radius[MAX_PARTICLES];
};

layout(std430, binding = 2) buffer positions {
    vec3 position[MAX_PARTICLES];
};

layout(std430, binding = 3) buffer colors {
    vec4 color[MAX_PARTICLES];
};

mat4 getModel(int index) {
    float rad = radius[index];
    vec3 pos  = position[index];

    mat4 scale = mat4(1);
    scale[0][0] = rad;
    scale[1][1] = rad;
    scale[2][2] = rad;

    mat4 translate = mat4(1);
    translate[3][0] = pos.x;
    translate[3][1] = pos.y;
    translate[3][2] = pos.z;

    return translate * scale;
}

void main() {
    int index = gl_InstanceID;
    instanceId = gl_InstanceID;

    mat4 mvp = projection * view * getModel(index);
    gl_Position = mvp * vec4(vertexPosition, 1.0);

    diffuseColor = color[index];
}
