cbuffer cbPass : register(b0) {
	float g_time : packoffset(c0.x);
	float3 g_eye : packoffset(c0.y);
	float4x4 projView : packoffset(c1);
}

struct Light {
	float3 position;
	float3 color;
};

cbuffer cbLights : register(b1) {
	Light light;
}

cbuffer cbPerInstance : register(b4) {
	float4x4 model;
	float4x4 modelNormal;	
}

struct VS_IN {
	float3 posL : POSITION0;
	float3 normalL : NORMAL0;
	float3 tangentL : TANGENT0;
	float3 uv : TEXCOORD0;
};

struct VS_OUT {
	float4 posH : SV_POSITION;
	float3 posW : POSITION0;
	float3 normalW : NORMAL0;
};

VS_OUT baseVS(VS_IN v) {
	VS_OUT vout;
	vout.posW = mul(model, float4(v.posL, 1.f)).xyz;
	vout.normalW = mul(modelNormal, float4(v.normalL, 1.f)).xyz;
	vout.posH = mul(projView, float4(vout.posW, 1.f));

	return vout;
}

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

float4 basePS(VS_OUT v) : SV_Target {
	float3 posW = v.posW;
	float3 normalW = normalize(v.normalW);
	float3 toEye = normalize(g_eye - posW);

	float3 lightVec = normalize(light.position - posW);

	float lambertLaw = max(dot(lightVec, normalW), 0);
	float3 lightStrength = light.color * lambertLaw;

	float3 color = BlinnPhong(lightStrength, lightVec, normalW, toEye);
	return float4(color, 1);
}
