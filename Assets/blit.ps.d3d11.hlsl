
Texture2D tex : register(t0);
SamplerState near_sampler : register(s0);

struct PSInput
{
	float4 position : SV_Position;
	float2 uv : TEXCOORD0;
};

float4 PSMain(PSInput input) : SV_Target
{
	float4 color = tex.Sample(near_sampler, input.uv);
	return color;
}

