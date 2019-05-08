cbuffer WorldViewProjectionConstantBuffer : register(b0) {
	matrix world;
	matrix view;
	matrix projection;
};

cbuffer ModelConstantBuffer : register(b1) {
	matrix model;
}

struct VS_INPUT {
	float3 position: POSITION;
	float3 color: COLOR;
};

struct VS_OUTPUT {
	float4 position: SV_POSITION;
	float4 color: COLOR;
};

VS_OUTPUT main(VS_INPUT input) {
	VS_OUTPUT output;

	float4 pos = float4(input.position, 1.0f);

	// Transform the position from object space to homogeneous projection space
	pos = mul(pos, model);
	pos = mul(pos, world);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.position = pos;

	// Just pass through the color data
	output.color = float4(input.color, 1.0f);

	return output;
}