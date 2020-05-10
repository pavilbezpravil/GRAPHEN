cbuffer cbPass : register(b0) {
	float gTime : packoffset(c0.x);
	float3 g_eye : packoffset(c0.y);
	float4x4 projView : packoffset(c1);
	float4x4 gViewProjShadow : packoffset(c5);
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
	float2 uv : TEXCOORD0;
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

	#ifdef POS_NORMAL_IN_WORLD_SPACE
		vout.posW = v.posL;
		vout.normalW = v.normalL;
	#else
		vout.posW = mul(model, float4(v.posL, 1.f)).xyz;
		vout.normalW = mul(modelNormal, float4(v.normalL, 1.f)).xyz;
	#endif
	vout.posH = mul(projView, float4(vout.posW, 1.f));
	vout.shadowCoord = mul(gViewProjShadow, float4(vout.posW, 1.f)).xyz;
	vout.shadowCoord.xy = float2(vout.shadowCoord.x * 0.5 + 0.5f, vout.shadowCoord.y * -0.5f + 0.5f);

	return vout;
}

struct PS_OUT {
	#ifndef Z_PASS
		float4 target0 : SV_Target0;
	#endif
};

PS_OUT basePS(VS_OUT v) {
	PS_OUT psout;

	#ifndef Z_PASS
		float3 posW = v.posW;
		float3 normalW = normalize(v.normalW);
		float3 toEye = normalize(g_eye - posW);

		#ifdef PRERECORD
			psout.target0 = float4(v.normalW, 1);
		#else
			Light light = directionalLight;

			// float distToLight = length(light.position - posW);
			float3 lightVec = light.type == LIGHT_DIRECTIONAL ? -light.positionOrDir : normalize(light.positionOrDir - posW);

			float lambertLaw = max(dot(lightVec, normalW), 0);
			float3 lightStrength = light.color * light.strength * lambertLaw /*/ (0.01f + distToLight)*/;

			float3 color = BlinnPhong(lightStrength, lightVec, normalW, toEye,v.shadowCoord);

			psout.target0 = float4(color, 1);
		#endif // !PRERECORD
	#endif // Z_PASS

	return psout;
}
