#version 450

#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in flat uint fragTexIndex;
layout(location = 3) in flat uint shadow;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D texSampler[];

void main() {
/*
    vec4 textureColor = texture(texSampler[nonuniformEXT(fragTexIndex)], fragTexCoord);

    outColor = (textureColor.a < 0.1) ?
        fragColor :
        vec4(textureColor.rgb, 1.0) / (shadow + 1)
    ;
    */
    outColor = vec4(fragTexCoord, 0.0, 1.0);
}
