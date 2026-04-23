#version 450

#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec3 fragTexCoords;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform samplerCube skybox;

layout(set = 2, binding = 0) readonly uniform UniformBufferObject {
    mat4 view;
    mat4 proj;

    vec4 lightDirection;
    vec4 lightColor;
    vec4 cameraPos;
} ubo;

struct ObjectData {
    uint index;
    mat4 model;
};

struct Materials {
    vec4 Ka;  /* Ambient */
    vec4 Kd;  /* Diffuse */
    vec4 Ks;  /* Specular */
    vec4 Ke;  /* Emission */
    vec4 Kt;  /* Transmittance */
    vec4 Tf;  /* Transmission filter */
    float Ns;     /* Shininess */
    float Ni;     /* Index of refraction */
    float d;      /* Disolve (alpha) */
    int illum;  /* Illumination model */

    /* Set for materials that don't come from the associated mtllib */
    int fallback;

    /* Texture map indices in fastObjMesh textures array */
    uint map_ka;
    uint map_kd;
    uint map_ks;
    uint map_ke;
    uint map_kt;
    uint map_ns;
    uint map_ni;
    uint map_d;
    uint map_bump;
    int pad0;
    int pad1;
};

layout(std140, set = 0, binding = 1) readonly buffer ObjectBuffer{
	Materials nr[];
} material;

layout(push_constant) uniform constants {
	int textureOffset;
} PushConstants;

void main() {
    outColor = texture(skybox, fragTexCoords);
}
