cbuffer cbComputePass : register(b0) {
	float gTime;
	float gDeltaTime;
}

struct ClothVertex {
	float3 posL;
	float3 normalL;
	float3 tangentL;
	float2 uv;
};

StructuredBuffer<ClothVertex> gCurrentClothVertex : register(t0);
RWStructuredBuffer<ClothVertex> gNextClothVertex : register(u0);

[numthreads(32 * 32, 1, 1)]
void clothCS(int3 dispatchTreadID : SV_DispatchThreadID) {
	uint id = dispatchTreadID.x;
	ClothVertex clothVertex = gCurrentClothVertex[id];
	clothVertex.posL.y = 0.5 * sin(clothVertex.posL.x + clothVertex.posL.z + 3 * gTime);
	gNextClothVertex[id] = clothVertex;
}
