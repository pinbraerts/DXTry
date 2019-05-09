cbuffer WorldViewProjectionConstantBuffer: register(b0) {
	matrix world;
	matrix view;
	matrix projection;
};

cbuffer LightConstantBuffer: register(b1) {
	float3 light_vec;
	float3 eye;
	float4 light_color;
};

cbuffer ModelConstantBuffer: register(b2) {
	matrix model;
}

struct VS_INPUT {
	float3 position: POSITION;
	float3 color: COLOR;
};

struct VS_OUTPUT {
	float4 position: SV_POSITION;
	float4 color: COLOR;
	float3 light_vec: POSITION0;
	float3 out_vec: POSITION1;
};

VS_OUTPUT main(VS_INPUT input) {
	VS_OUTPUT output;

	float4 pos = float4(input.position, 1.0f);

	// Transform the position from object space to homogeneous projection space
	matrix mw = mul(model, world);
	matrix wv = mul(world, view);
	matrix mwv = mul(model, wv);
	matrix mwvp = mul(mwv, projection);
	output.position = mul(pos, mwvp);

	// Just pass through the color data
	output.color = float4(input.color, 1.0f);

	float4 out_vec = mul(pos, mwv) - float4(eye, 0.0f);
	//out_vec = mul(out_vec, view);
	output.out_vec = out_vec.xyz;

	float4 light_dir = mul(float4(light_vec, 0.0f), wv);
	//light_vec = mul(light_vec, view);
	output.light_vec = light_dir.xyz;

	return output;
}