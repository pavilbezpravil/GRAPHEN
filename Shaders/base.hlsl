cbuffer cbPass : register(b0) {
	float gTime : packoffset(c0.x);
	float3 g_eye : packoffset(c0.y);
	float4x4 projView : packoffset(c1);
	float4x4 modelToShadow : packoffset(c5);
	uint lightCount : packoffset(c9);
}

struct InstanceData {
	float4x4 model;
	float4x4 modelNormal;
};

SamplerState gsamPointClamp : register(s0);

Texture2D gShadowMap : register(t0);
StructuredBuffer<InstanceData> gInstanceData : register(t0, space1);

#include "lighting.hlsl"

static const uint SCENE_MAX_LIGHTS = 16;
cbuffer cbLights : register(b1) {
	Light directionalLight;
	Light lights[SCENE_MAX_LIGHTS];
}

struct VS_IN {
	float3 posL : POSITION0;
	float3 normalL : NORMAL0;
	float3 tangentL : TANGENT0;
	float3 uv : TEXCOORD0;
};

struct VS_OUT {
	float4 posH : SV_Position;
	float3 posW : POSITION0;
	float3 normalW : NORMAL0;
	float3 shadowCoord : TexCoord2;
};

VS_OUT baseVS(VS_IN v, uint instanceID : SV_InstanceID) {
	VS_OUT vout;

	InstanceData instanceData = gInstanceData[instanceID];
	float4x4 model = instanceData.model;
	float4x4 modelNormal = instanceData.modelNormal;

	vout.posW = mul(model, float4(v.posL, 1.f)).xyz;
	vout.normalW = mul(modelNormal, float4(v.normalL, 1.f)).xyz;
	vout.posH = mul(projView, float4(vout.posW, 1.f));
	vout.shadowCoord = mul(modelToShadow, float4(vout.posW, 1.f)).xyz;
	vout.shadowCoord.xy = float2(vout.shadowCoord.x * 0.5 + 0.5f, -vout.shadowCoord.y * 0.5f + 0.5f);

	return vout;
}

struct PS_OUT {
	float4 target1 : SV_Target;
};

#ifdef Z_PASS
void basePS(VS_OUT v) {}
#else
PS_OUT basePS(VS_OUT v) {
	PS_OUT psout;

	float3 posW = v.posW;
	float3 normalW = normalize(v.normalW);
	float3 toEye = normalize(g_eye - posW);

	Light light = directionalLight;

	// float distToLight = length(light.position - posW);
	float3 lightVec = light.type == LIGHT_DIRECTIONAL ? -light.positionOrDir : normalize(light.positionOrDir - posW);

	float lambertLaw = max(dot(lightVec, normalW), 0);
	float3 lightStrength = light.color * light.strength * lambertLaw /*/ (0.01f + distToLight)*/;

	float3 color = BlinnPhong(lightStrength, lightVec, normalW, toEye,v.shadowCoord);

	psout.target1 = float4(color, 1);
	return psout;
}
#endif
