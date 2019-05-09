struct GS_INPUT {
	float4 position: SV_POSITION;
	float3 pos_world: POSITION; // multiplied only on world
};

struct GS_OUTPUT {
	float4 position: SV_POSITION;
	float3 pos_world: POSITION;
	float3 normal: NORMAL;
};

[maxvertexcount(3)]
void main(
	triangle GS_INPUT input[3], 
	inout TriangleStream<GS_OUTPUT> output
) {
	float3	a = (input[1].pos_world - input[0].pos_world).xyz,
			b = (input[2].pos_world - input[0].pos_world).xyz;
	float3 n = normalize(cross(b, a));

	for (uint i = 0; i < 3; i++) {
		GS_OUTPUT element;
		element.position = input[i].position;
		element.pos_world = input[i].pos_world;
		element.normal = n;
		output.Append(element);
	}
}