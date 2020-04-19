
cbuffer cbPerInstance : register(b0) {
	float4x4 model;
	float4x4 modelInv;
	float4x4 projView;
}

struct VS_IN {
	float3 posL : POSITION0;
};

struct VS_OUT {
	float4 posH : SV_POSITION;
	float3 posW : POSITION0;
};

VS_OUT baseVS(VS_IN vin) {
	VS_OUT vout;
	vout.posW = mul(model, float4(vin.posL, 1.f)).xyz;
	vout.posH = mul(projView, float4(vout.posW, 1.f));

	return vout;
}

float4 basePS(VS_OUT pin) : SV_Target {
	return float4(1, 0, 0, 1);
}
