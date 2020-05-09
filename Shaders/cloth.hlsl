cbuffer cbComputePass : register(b0) {
	float gTime;
	float gDeltaTime;
}

StructuredBuffer<float3> gPosition : register(t0);
StructuredBuffer<float3> gNormal : register(t1);

RWStructuredBuffer<float3> gNextPosition : register(u0);
RWStructuredBuffer<float3> gNextNormal : register(u1);

[numthreads(32 * 32, 1, 1)]
void clothCS(int3 dispatchTreadID : SV_DispatchThreadID) {
	uint id = dispatchTreadID.x;
	float3 pos = gPosition[id];
	pos.y = 0.5 * sin(pos.x + pos.z + 3 * gTime);
	gNextPosition[id] = pos;
}
