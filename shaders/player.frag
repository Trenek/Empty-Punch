#version 450

#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in flat uint fragTexIndex;
layout(location = 3) in flat uint shadow;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D texSampler[];

struct Materials {
	vec4 base_color_factor;
	float metallic_factor;
	float roughness_factor;
    uint baseColorID;
    uint pad0;
};

layout(std140, set = 0, binding = 3) readonly buffer ObjectBuffer{
	Materials nr[];
} material;

layout(push_constant) uniform constants {
	int nodeID;
    int materialID;
} PushConstants;

void main() {
    vec4 textureColor = texture(texSampler[nonuniformEXT(fragTexIndex)], fragTexCoord);

    vec3 outColor3 = (textureColor.a < 0.1) ? fragColor : textureColor.rgb;

    outColor = vec4(outColor3, 1.0);

    if (shadow == 1) {
        outColor[0] = 1 - outColor[0];
        outColor[1] = 1 - outColor[1];
        outColor[2] = 1 - outColor[2];
    }
}
