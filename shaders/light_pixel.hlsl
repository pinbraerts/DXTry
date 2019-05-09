cbuffer LightConstantBuffer: register(b1) {
	float3 _light_position;
	float3 _eye;

	float4 light_ambient;
	float4 light_diffuse;
	float4 light_specular;
	float4 _align;
};

cbuffer MaterialConstantBuffer: register(b3) {
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float4 shininess;
};

struct PS_INPUT {
	float4 position: SV_POSITION;
	float3 light_vec: POSITION0;
	float3 out_vec: POSITION1;
	float3 normal: NORMAL;
};

float4 main(PS_INPUT input): SV_TARGET {
	float3 L = normalize(input.light_vec);
	float3 V = normalize(input.out_vec);
	float3 R = normalize(reflect(-L, input.normal));

	float4 ambient_color = ambient * light_ambient;
	float4 diffuse_color = max(dot(input.normal, L), 0.0f) * diffuse * light_diffuse;
	float4 specular_color = pow(max(dot(R, V), 0.0f), 126 * shininess.x) * specular * light_specular;

	float4 color = ambient_color + diffuse_color + specular_color;

	return color;
}
