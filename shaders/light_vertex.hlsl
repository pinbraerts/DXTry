cbuffer WorldViewProjectionConstantBuffer: register(b0) {
	matrix world;
	matrix view;
	matrix projection;
};

cbuffer LightConstantBuffer: register(b1) {
	float4 light_vec;
	float4 eye;

	float4 _light_ambient;
	float4 _light_diffuse;
	float4 _light_specular;

	float4 _light_attenuation;
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

	float4 light_dir = mul(float4(light_vec.xyz, 0.0f), world);

	if (light_vec.w < 0.5f) { // light is point
		light_dir -= pos;
	}
	output.light_vec = light_dir.xyz;

	float4 out_vec = pos - mul(eye, world);
	//out_vec = mul(out_vec, view);
	output.out_vec = out_vec.xyz;

	return output;
}