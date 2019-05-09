cbuffer WorldViewProjectionConstantBuffer : register(b0) {
	matrix world;
	matrix view;
	matrix projection;
};

cbuffer LightConstantBuffer: register(b1) {
	float3 light_vec;
	float3 eye;
	float4 light_color;
};

cbuffer ModelConstantBuffer : register(b2) {
	matrix model;
}

struct VS_INPUT {
	float3 position: POSITION;
};

struct VS_OUTPUT {
	float4 position: SV_POSITION;
};

VS_OUTPUT main(VS_INPUT input) {
	VS_OUTPUT output;

	float4 pos = float4(input.position, 1.0f);

	// Transform the position from object space to homogeneous projection space
	pos = mul(pos, model);
	pos = mul(pos, world);
	pos += float4(light_vec, 0.0f);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.position = pos;

	return output;
}