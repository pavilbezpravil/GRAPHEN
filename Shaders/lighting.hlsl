static const int LIGHT_INVALID = -1;
static const int LIGHT_DIRECTIONAL = 0;
static const int LIGHT_POINT = 1;
static const int LIGHT_SPOT = 2;

struct Light {
    float3 positionOrDir;
    int type;
    float3 color;
    float strength;
    float pad1;
    float pad2;
    float pad3;
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
    float v = 45;
	return float3(v, v, v) / 255.f;
}

float3 GetAlbedo() {
    float v = 170;
	return float3(v, v, v) / 255.f;
}

float GetShadow(float3 shadowCoord) {
    const float gShadowMapTexelSize = 1.f / 1000.f; // todo:
    const float dx = gShadowMapTexelSize;
    float bias = 0.005f;
    float d0 = gShadowMap.Sample(gsamPointClamp, shadowCoord.xy).r < shadowCoord.z + bias;
    float d1 = gShadowMap.Sample(gsamPointClamp, shadowCoord.xy + float2(dx, 0)).r < shadowCoord.z + bias;
    float d2 = gShadowMap.Sample(gsamPointClamp, shadowCoord.xy + float2(0, dx)).r < shadowCoord.z + bias;
    float d3 = gShadowMap.Sample(gsamPointClamp, shadowCoord.xy + float2(-dx, 0)).r < shadowCoord.z + bias;
    float d4 = gShadowMap.Sample(gsamPointClamp, shadowCoord.xy + float2(0, -dx)).r < shadowCoord.z + bias;

    float d5 = gShadowMap.Sample(gsamPointClamp, shadowCoord.xy + float2(dx, dx)).r < shadowCoord.z + bias;
    float d6 = gShadowMap.Sample(gsamPointClamp, shadowCoord.xy + float2(-dx, dx)).r < shadowCoord.z + bias;
    float d7 = gShadowMap.Sample(gsamPointClamp, shadowCoord.xy + float2(-dx, -dx)).r < shadowCoord.z + bias;
    float d8 = gShadowMap.Sample(gsamPointClamp, shadowCoord.xy + float2(dx, -dx)).r < shadowCoord.z + bias;

    float res = (d0 * 2 + d1 + d2 + d3 + d4 + d5 + d6 + d7 + d8 ) / 10.f;
    // res *= 1 - dot(min(pow(abs(shadowCoord.xy - 0.5f) * 2, 2), 1.f), float2(1, 1));
    // return d0;
    return res;
}

float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye, float3 shadowCoord) {
    const float m = 32;
    const float fresnelR0 = 0.05f;

    float3 halfVec = normalize(toEye + lightVec);

	float roughnessFactor = RoughnessFactor(normal, halfVec, m);
    float3 fresnelFactor = SchlickFresnel(fresnelR0, halfVec, lightVec);

    float3 specAlbedo = fresnelFactor * roughnessFactor;

    specAlbedo = specAlbedo / (specAlbedo + 1.0f);

    return (GetAlbedo() + specAlbedo) * lightStrength * GetShadow(shadowCoord) + GetAmbient();
}
