Texture2D gTex : register(t0);
SamplerState gSamTex : register(s0);

struct VS_IN {
	float2 posL : POSITION0;
	float2 uv : TEXCOORD0;
};

struct VS_OUT {
	float4 posH : SV_POSITION;
	float2 uv : TEXCOORD0;
};


VS_OUT quadVS(VS_IN v) {
	VS_OUT vout;
	vout.uv = v.uv;
	vout.posH = float4(v.posL, 0.5, 1);

	return vout;
}

float4 quadPS(VS_OUT v) : SV_Target {
	float4 texColor = gTex.SampleLevel(gSamTex, v.uv, 0);
	return texColor;
}
