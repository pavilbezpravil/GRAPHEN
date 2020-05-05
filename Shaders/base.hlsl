#include "lighting.hlsl"

cbuffer cbPass : register(b0) {
	float g_time : packoffset(c0.x);
	float3 g_eye : packoffset(c0.y);
	float4x4 projView : packoffset(c1);
}

cbuffer cbLights : register(b1) {
	Light light;
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
};

struct InstanceData {
	float4x4 model;
	float4x4 modelNormal;
};

StructuredBuffer<InstanceData> gInstanceData : register(t0, space1);

VS_OUT baseVS(VS_IN v, uint instanceID : SV_InstanceID) {
	VS_OUT vout;

	InstanceData instanceData = gInstanceData[instanceID];
	float4x4 model = instanceData.model;
	float4x4 modelNormal = instanceData.modelNormal;

	vout.posW = mul(model, float4(v.posL, 1.f)).xyz;
	vout.normalW = mul(modelNormal, float4(v.normalL, 1.f)).xyz;
	vout.posH = mul(projView, float4(vout.posW, 1.f));

	return vout;
}

float4 basePS(VS_OUT v) : SV_Target {
	float3 posW = v.posW;
	float3 normalW = normalize(v.normalW);
	float3 toEye = normalize(g_eye - posW);

	float distToLight = length(light.position - posW);
	float3 lightVec = normalize(light.position - posW);

	float lambertLaw = max(dot(lightVec, normalW), 0);
	float3 lightStrength = light.color * lambertLaw /*/ (0.01f + distToLight)*/;

	float3 color = BlinnPhong(lightStrength, lightVec, normalW, toEye);
	return float4(color, 1);
}
