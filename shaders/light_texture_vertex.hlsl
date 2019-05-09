cbuffer WorldViewProjectionConstantBuffer: register(b0) {
	matrix world;
	matrix view;
	matrix projection;
};

cbuffer ModelConstantBuffer: register(b2) {
	matrix model;
}

struct VS_INPUT {
	float3 position: POSITION;
	float2 tex_pos: TEXCOORD;
};

struct VS_OUTPUT {
	float4 position: SV_POSITION;
	float3 pos_world: POSITION; // multiplied only on world
	float2 tex_pos: TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input) {
	VS_OUTPUT output;

	float4 pos = float4(input.position, 1.0f);
	pos = mul(pos, model);
	pos = mul(pos, world);
	output.pos_world = pos.xyz;

	pos = mul(pos, world);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.position = pos;

	output.tex_pos = input.tex_pos;

	return output;
}