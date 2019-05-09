cbuffer WorldViewProjectionConstantBuffer: register(b0) {
	matrix world;
	matrix view;
	matrix projection;
};

cbuffer LightConstantBuffer: register(b1) {
	float4 position_world; // premultiplied on world
	float4 eye_world; // premultiplied on world
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float4 attenuation;
	float4 direction; // premultiplied on world
};

cbuffer ModelConstantBuffer: register(b2) {
	matrix model;
}

struct VS_INPUT {
	float3 position: POSITION;
};

struct VS_OUTPUT {
	float4 position: SV_POSITION;
	float3 light_vec: POSITION0;
	float3 out_vec: POSITION1;
};

VS_OUTPUT main(VS_INPUT input) {
	VS_OUTPUT output;

	float4 pos = float4(input.position, 1.0f);

	pos = mul(pos, model);
	output.position = pos;

	pos = mul(pos, world);

	float4 light_dir = float4(position_world.xyz, 0.0f);
	if (position_world.w < 0.6f) { // light is point or flash
		light_dir -= pos;
	}
	output.light_vec = light_dir.xyz;

	output.out_vec = pos.xyz - eye_world.xyz;

	return output;
}