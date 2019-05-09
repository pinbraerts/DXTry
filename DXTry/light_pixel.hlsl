cbuffer LightConstantBuffer: register(b1) {
	float3 _light_position;
	float3 _eye;
	float4 light_color;
};

cbuffer MaterialConstantBuffer: register(b3) {
	float3 ambient;
	float3 diffuse;
	float3 specular;
	float3 shininess;
};

struct PS_INPUT {
	float4 position: SV_POSITION;
	float4 color: COLOR;
	float3 light_vec: POSITION0;
	float3 out_vec: POSITION1;
	float3 normal: NORMAL;
};

float4 main(PS_INPUT input): SV_TARGET {
	float3 L = normalize(input.light_vec);
	float3 V = normalize(input.out_vec);
	float3 R = normalize(reflect(-L, input.normal));

	float4 ambient_color = float4(ambient, 1.0f) * light_color;
	float4 diffuse_color = max(dot(input.normal, L), 0.0f) * float4(diffuse, 1.0f) * light_color;
	float4 specular_color = pow(max(dot(R, V), 0.0f), shininess.x) * float4(specular, 1.0f) * light_color;

	float4 color = ambient_color + diffuse_color + specular_color;

	//return float4(input.out_vec, 1.0f);
	return color * input.color;
}
