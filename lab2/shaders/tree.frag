#version 330 core

in vec3 worldPosition;
in vec3 worldNormal;
in vec2 uv;

out vec3 finalColor;

uniform vec3 lightPosition;
uniform vec3 lightIntensity;
uniform sampler2D textureSampler;
uniform sampler2D normalMapSampler;

void main() {

    // TODO: normal map
    // vec3 normalMap = texture(normalMapSampler, uv).rgb * 2.0 - 1.0; // Convert to [-1, 1]
    // vec3 adjustedNormal = normalize(worldNormal + normalMap);

    vec3 lightDirection = normalize(lightPosition - worldPosition);
    float distance = length(lightPosition - worldPosition);

    // Attenuation
    float constant = 1.0;
    float linear = 0.2;
    float quadratic = 0.03;
    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

    // Diffuse
    float diff = max(dot(worldNormal, lightDirection), 0.0);
    // TODO:
    // float diff = max(dot(adjustedNormal, lightDirection), 0.0);
    vec3 albedo = texture(textureSampler, uv).rgb;

    // Ambient
    vec3 ambient = 0.05 * albedo;

    vec3 lighting = ambient + (diff * lightIntensity * attenuation * albedo);
    finalColor = pow(lighting, vec3(1.0 / 2.2));
}
