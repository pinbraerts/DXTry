#define MAX_LIGHTS 10

struct LightConstant {
	float4 position_world; // premultiplied on world
	float4 eye_world; // premultiplied on world
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float4 attenuation;
	float4 direction_world; // premultiplied on world
};

cbuffer LightConstantBuffer: register(b1) {
	LightConstant lights[MAX_LIGHTS];
};

cbuffer MaterialConstantBuffer: register(b3) {
	float4 material_ambient;
	float4 material_diffuse;
	float4 material_specular;
	float4 shininess;
};

struct PS_INPUT {
	float4 position: SV_POSITION;
	float3 pos_world: POSITION;
	float3 normal: NORMAL;
};

float4 main(PS_INPUT input): SV_TARGET {
	float4 final_color = float4(0, 0, 0, 0);

	for (uint i = 0; i < MAX_LIGHTS; ++i) {
		if (all(lights[i].ambient == 0) && all(lights[i].diffuse == 0) && all(lights[i].specular == 0))
			break;

		float3 light_vec = lights[i].position_world.xyz;
		if (lights[i].position_world.w < 0.3f) { // light is point or flashlight
			light_vec -= input.pos_world;
		}

		float3 out_vec = input.pos_world - lights[i].eye_world.xyz;

		float3 L = normalize(light_vec);
		float3 V = normalize(out_vec);
		float3 R = normalize(reflect(-L, input.normal));

		float4 ambient = material_ambient * lights[i].ambient;
		float4 diffuse = max(dot(input.normal, L), 0.0f) * material_diffuse * lights[i].diffuse;
		float4 specular = pow(max(dot(R, V), 0.0f), 126 * shininess.x) * material_specular * lights[i].specular;

		if (lights[i].position_world.w < 0.5f) { // light is point or flashlight
			if (any(lights[i].direction_world.xyz != 0)) { // flashlight
				float theta = dot(-L.xyz, normalize(lights[i].direction_world.xyz));
				float cut_off = lights[i].attenuation.w;
				float outer = lights[i].direction_world.w;

				float epsilon = cut_off - outer;
				float intensity = clamp((theta - outer) / epsilon, 0.0, 1.0);

				diffuse *= intensity;
				specular *= intensity;
			}

			if (any(lights[i].attenuation.xyz != 0)) {
				float d = length(light_vec);
				float attenuation = lights[i].attenuation.x; // constant
				attenuation += lights[i].attenuation.y * d; // linear
				d *= d;
				attenuation += lights[i].attenuation.z * d; // quadratic
				attenuation = 1 / attenuation;

				ambient *= attenuation;
				diffuse *= attenuation;
				specular *= attenuation;
			}
		}

		final_color += ambient + diffuse + specular;
	}

	return final_color;
}
