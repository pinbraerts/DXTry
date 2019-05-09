cbuffer WorldViewProjectionConstantBuffer: register(b0) {
	matrix world;
	matrix view;
	matrix projection;
};

struct GS_INPUT {
	float4 position: SV_POSITION;
	float4 color: COLOR;
	float3 light_vec: POSITION0;
	float3 out_vec: POSITION1;
};

struct GS_OUTPUT {
	float4 position: SV_POSITION;
	float4 color: COLOR;
	float3 light_vec: POSITION0;
	float3 out_vec: POSITION1;
	float3 normal: NORMAL;
};

[maxvertexcount(3)]
void main(
	triangle GS_INPUT input[3], 
	inout TriangleStream<GS_OUTPUT> output
) {
	float3	a = (input[1].position - input[0].position).xyz,
			b = (input[2].position - input[0].position).xyz;
	float3 n = normalize(cross(b, a));

	matrix wvp = mul(world, view);
	wvp = mul(wvp, projection);

	for (uint i = 0; i < 3; i++) {
		GS_OUTPUT element;
		element.position = mul(input[i].position, wvp);
		element.color = input[i].color;
		element.out_vec = input[i].out_vec;
		element.light_vec = input[i].light_vec;
		element.normal = n;
		output.Append(element);
	}
}