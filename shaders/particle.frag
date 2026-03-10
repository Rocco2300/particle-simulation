#version 440

#define MAX_PARTICLES 5000

flat in int instanceId;
in vec4 diffuseColor;

uniform int renderingType;

layout(std430, binding=0) buffer types {
    int type[MAX_PARTICLES];
};

void main() {
    //if (renderingType != type[instanceId]) {
    //    discard;
    //}

    gl_FragColor = diffuseColor;
}
