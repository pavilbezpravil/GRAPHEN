struct Light {
    float3 position;
    float3 color;
};

float3 SchlickFresnel(float3 R0, float3 normal, float3 lightVec) {
    float cosIncidentAngle = saturate(dot(normal, lightVec));
    float f0 = 1.0f - cosIncidentAngle;
    return R0 + (1.0f - R0) * pow(f0, 4);
}

float RoughnessFactor(float3 n, float3 h, float m) {
	return (m + 8) / 8 * pow(max(dot(n, h), 0), m);
}

float3 GetAmbient() {
	return float3(0.1, 0.1, 0.1);
}

float3 GetAlbedo() {
	return float3(1, 0, 0);
}

float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye) {
    const float m = 128;
    const float fresnelR0 = 0.5f;

    float3 halfVec = normalize(toEye + lightVec);

	float roughnessFactor = RoughnessFactor(normal, halfVec, m);
    float3 fresnelFactor = SchlickFresnel(fresnelR0, halfVec, lightVec);

    float3 specAlbedo = fresnelFactor * roughnessFactor;

    // Our spec formula goes outside [0,1] range, but we are 
    // doing LDR rendering.  So scale it down a bit.
    specAlbedo = specAlbedo / (specAlbedo + 1.0f);

    return (GetAlbedo() + specAlbedo) * lightStrength;
}