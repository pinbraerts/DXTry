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
	float4	a = input[1].position - input[0].position,
			b = input[2].position - input[0].position;
	float4 n4 = float4(normalize(cross(a, b)), 0.0f);
	n4 = mul(world, n4);
	n4 = mul(view, n4);
	float3 n3 = n4.xyz;

	for (uint i = 0; i < 3; i++) {
		GS_OUTPUT element;
		element.position = input[i].position;
		element.color = input[i].color;
		element.out_vec = input[i].out_vec;
		element.light_vec = input[i].light_vec;
		element.normal = n3;
		output.Append(element);
	}
}