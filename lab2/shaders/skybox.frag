#version 330 core

in vec3 color;

// TODO: To add UV input to this fragment shader
in vec2 uv;
// TODO: To add the texture sampler
uniform sampler2D textureSampler;

out vec3 finalColor;

void main()
{
    finalColor = texture(textureSampler, uv).rgb;

    // TODO: texture lookup.
    //finalColor = color * texture(textureSampler, uv).rgb;
}