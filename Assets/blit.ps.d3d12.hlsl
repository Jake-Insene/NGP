sampler point_sampler : register(s0, space1);
Texture2D<float4> framebuffer_texture : register(t0, space0);

struct PSInput
{
	float4 position : SV_Position;
	float2 uv : TEXCOORD0;
};

float4 PSMain(PSInput input) : SV_Target
{
	float4 color = framebuffer_texture.Sample(point_sampler, input.uv);
	return color;
}