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

float4 basePS(VS_OUT v) : SV_Target {
	// return float4(v.normalW, 1);

	float3 posW = v.posW;
	float3 normalW = normalize(v.normalW);
	float3 toEye = normalize(g_eye - posW);

	float3 albedo = float3(1, 0, 0);
	float3 ambient = float3(1, 0, 0);

	float3 L = normalize(light.position - posW);

	float lambertLaw = max(dot(L, normalW), 0);
	float3 diffuse = albedo * lambertLaw * light.color;

	float3 color = diffuse;
	float alpha = 1;

	// color = lambertLaw;

	return float4(color, alpha);
}
