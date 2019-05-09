Texture2D tex;
SamplerState sam;

struct PS_INPUT {
	float4 position: SV_POSITION;
	float2 tex_pos: TEXCOORD;
};

float4 main(PS_INPUT input) : SV_TARGET {
	return tex.Sample(sam, input.tex_pos);
}
