cbuffer cbComputePass : register(b0) {
	uint gNSize : packoffset(c0.x);
	uint gMSize : packoffset(c0.y);
	uint gNParticles : packoffset(c0.z);
	uint gNConstrains : packoffset(c0.w);
	float gTime : packoffset(c1.x);
	float gDeltaTime : packoffset(c1.y);
	float gRestDist : packoffset(c1.z);
	float gKVelocityDump : packoffset(c1.w);
	float gKs : packoffset(c2.x);
	float gKs_diagonal : packoffset(c2.y);
	float gKs_bend : packoffset(c2.z);
	uint gUseDiagonal : packoffset(c2.w);
	uint gUseBend : packoffset(c3.x);
	// float4x4 gModel : packoffset(c2.x);
	// float4x4 gModelInv : packoffset(c6.x);
	// float4x4 gModelNormal : packoffset(c10.x);
	// float4x4 gModelNormalInv : packoffset(c14.x);
}

const static uint CONSTRAINT_TYPE_ATTACH           = 1;
const static uint CONSTRAINT_TYPE_SPHERE_COLLISION = 2;

struct Constraint {
	uint type;
	float3 data0;
	float data1;
};

StructuredBuffer<float3> gPrevPosition : register(t0);
StructuredBuffer<Constraint> gConstrains : register(t1);

RWStructuredBuffer<float3> gPosition : register(u0);
RWStructuredBuffer<float3> gVelocity : register(u1);
RWStructuredBuffer<float3> gNormal : register(u2);

RWStructuredBuffer<float3> gTmpPosition : register(u3);


// float3 GetPosition(uint idx) {
// 	return gPosition[idx];
// }

// float3 SetPosition(uint idx, float3 pos) {
// 	gPosition[idx] = pos;
// }

bool ValidThread(uint3 threadID) {
	return threadID.x < gNSize && threadID.y < gMSize;
}

uint GetNodeID(uint2 threadID) {
	return threadID.y * gNSize + threadID.x;
}

void DumpVelocity(inout float3 velocity) {
	velocity *= gKVelocityDump;
}

const static float mass = 100.f;
const static float invMass = 1.f / mass;

#define GROUP_X_SIZE (8)
[numthreads(GROUP_X_SIZE, GROUP_X_SIZE, 1)]
void applyForcesDumpVelocityAssignPositionCS(uint3 dispatchThreadID : SV_DispatchThreadID) {
	if (!ValidThread(dispatchThreadID)) {
		return;
	}
	const uint nodeID = GetNodeID(dispatchThreadID.xy);

	const float3 externalForce = float3(0, -9.82f, 0) * mass;
	
	float3 velocityW = gVelocity[nodeID]; // velocity always in world space

	// todo:
	if (dispatchThreadID.y != 0) {
		velocityW += externalForce * invMass * gDeltaTime;
	}
	
	DumpVelocity(velocityW);
	gVelocity[nodeID] = velocityW;

	// move to world space, next computation happend in world space and in recordCS move in local space
	// float3 posW = mul(gModel, float4(gPrevPosition[id], 1.f)).xyz;
	gPosition[nodeID] = gPrevPosition[nodeID] + velocityW * gDeltaTime;

	// gNormal[id] = mul(gModelNormal, float4(gNormal[id], 0.f)).xyz;
	// todo: tangent
}

void ResolveDistanceConstraint(inout float3 p1, uint2 neightID, float restDist, float k) {
	float3 p2 = gPosition[GetNodeID(neightID)];
	float3 pDiff = p2 - p1;
	float pDiffLength = length(pDiff) + 0.0001f;
	float3 dp1 = 0.5f * k * (pDiffLength - restDist) * pDiff / pDiffLength;
	p1 += dp1;
}

[numthreads(GROUP_X_SIZE, GROUP_X_SIZE, 1)]
void solveCS(uint3 dispatchThreadID : SV_DispatchThreadID) {
	if (!ValidThread(dispatchThreadID)) {
		return;
	}
	const uint nodeID = GetNodeID(dispatchThreadID.xy);

	// todo: tmp
	if (dispatchThreadID.y == 0) {
		return;
	}

	float3 pos = gPosition[nodeID];

	const uint2 id = dispatchThreadID.xy;
	if (id.x > 0) {
		ResolveDistanceConstraint(pos, id + uint2(-1, 0), gRestDist, gKs);
	}
	if (id.x + 1 < gNSize) {
		ResolveDistanceConstraint(pos, id + uint2(1, 0), gRestDist, gKs);
	}
	if (id.y > 0) {
		ResolveDistanceConstraint(pos, id + uint2(0, -1), gRestDist, gKs);
	}
	if (id.y + 1 < gMSize) {
		ResolveDistanceConstraint(pos, id + uint2(0, 1), gRestDist, gKs);
	}

	if (gUseDiagonal) {
		float restDistDiag = 1.41421356f * gRestDist;
		if (id.x > 0 && id.y + 1 < gMSize) {
			ResolveDistanceConstraint(pos, id + uint2(-1, 1), restDistDiag, gKs_diagonal);
		}
		if (id.x > 0  && id.y > 0) {
			ResolveDistanceConstraint(pos, id + uint2(-1, -1), restDistDiag, gKs_diagonal);
		}
		if (id.x + 1 < gNSize && id.y > 0) {
			ResolveDistanceConstraint(pos, id + uint2(1, -1), restDistDiag, gKs_diagonal);
		}
		if (id.x + 1 < gNSize && id.y + 1 < gMSize) {
			ResolveDistanceConstraint(pos, id + uint2(1, 1), restDistDiag, gKs_diagonal);
		}
	}
	
	if (gUseBend) {
		float restDist = 2 * gRestDist;
		if (id.x > 1) {
			ResolveDistanceConstraint(pos, id + uint2(-2, 0), restDist, gKs_bend);
		}
		if (id.x + 2 < gNSize) {
			ResolveDistanceConstraint(pos, id + uint2(2, 0), restDist, gKs_bend);
		}
		if (id.y > 1) {
			ResolveDistanceConstraint(pos, id + uint2(0, -2), restDist, gKs_bend);
		}
		if (id.y + 2 < gMSize) {
			ResolveDistanceConstraint(pos, id + uint2(0, 2), restDist, gKs_bend);
		}
	}

	// todo:
	for (uint i = 0; i < 1; ++i) {
		// todo:
		Constraint constraint = gConstrains[i];
		float3 sPos = constraint.data0;
		float r = constraint.data1 + 0.05f;

		float3 diff = pos - sPos;
		float dist = length(diff);
		pos += normalize(diff) * max(r - dist, 0);
	}

	gTmpPosition[nodeID] = pos;
}

void UpdateVelocity(inout float3 velocity) {
	// velocity *= 0.95f; // todo:
}

[numthreads(GROUP_X_SIZE, GROUP_X_SIZE, 1)]
void recordCS(uint3 dispatchThreadID : SV_DispatchThreadID) {
	if (!ValidThread(dispatchThreadID)) {
		return;
	}
	const uint nodeID = GetNodeID(dispatchThreadID.xy);

	float3 posW = gPosition[nodeID];
	float3 velocity = (posW - gPrevPosition[nodeID]) / gDeltaTime;
	UpdateVelocity(velocity);
	gVelocity[nodeID] = velocity;

	// float3 posL = mul(gModelInv, float4(posW, 1)).xyz;
	// gTmpPosition[id] = posL;
	gTmpPosition[nodeID] = posW;
}


float3 ComputeNormal(in float3 p0, uint2 p1ID, uint2 p2ID) {
	float3 p1 = gPosition[p1ID.y * gNSize + p1ID.x];
	float3 p2 = gPosition[p2ID.y * gNSize + p2ID.x];

	float3 e1 = p1 - p0;
	float3 e2 = p2 - p0;

	float3 normal = normalize(cross(e1, e2));
	return normal;
}

[numthreads(GROUP_X_SIZE, GROUP_X_SIZE, 1)]
void computeNormalCS(uint3 dispatchThreadID : SV_DispatchThreadID) {
	if (!ValidThread(dispatchThreadID)) {
		return;
	}
	const uint nodeID = GetNodeID(dispatchThreadID.xy);

	const float3 pos = gPosition[nodeID];

	float3 normal = 0;
	const uint2 id = dispatchThreadID.xy;
	if (id.x > 0 && id.y + 1 < gMSize) {
		normal += ComputeNormal(pos, id + uint2(0, 1), id + uint2(-1, 0));
	}
	if (id.x > 0  && id.y > 0) {
		normal += ComputeNormal(pos, id + uint2(-1, 0), id + uint2(0, -1));
	}
	if (id.x + 1 < gNSize && id.y > 0) {
		normal += ComputeNormal(pos, id + uint2(0, -1), id + uint2(1, 0));
	}
	if (id.x + 1 < gNSize && id.y + 1 < gMSize) {
		normal += ComputeNormal(pos, id + uint2(1, 0), id + uint2(0, 1));
	}

	normal = normalize(normal);
	float3 normalW = normal;
	// float3 normalL = mul(gModelNormalInv, float4(normalW, 0.f)).xyz;

	gNormal[nodeID] = normalW;
}
