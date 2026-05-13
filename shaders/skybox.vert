#version 450

layout(location = 0) in  vec3 inPosition;
layout(location = 1) in  vec2 inTexCoord;
layout(location = 2) in  vec3 inNormal;
layout(location = 3) in  vec2 inPara;
layout(location = 4) in  uint inMaterial;

layout(location = 0) out vec3 fragTexCoords;

layout(set = 0, binding = 0) readonly uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

struct ObjectData {
    uint index;
    uint shadow;
    mat4 model;
};

layout(std140, set = 2, binding = 0) readonly buffer ObjectBuffer{
	ObjectData objects[];
} instance;

void main() {
    fragTexCoords = inPosition;
    vec4 position = vec4(inPosition, 1.0);
    vec4 normal = vec4(inNormal, 0.0);
    mat4 worldTransform = (
        instance.objects[gl_InstanceIndex].model
    );

    mat4 view = ubo.view;
    view[3][0] = 0;
    view[3][1] = 0;
    view[3][2] = 0;

    gl_Position = (
        ubo.proj * 
        view * 
        worldTransform *
        position
    );
} 
