

struct VSOutput
{
	float4 position : SV_Position;
	float2 uv : TEXCOORD0;
};


VSOutput VSMain(float2 position : POSITION, float2 uv : UV)
{
	VSOutput output;
	output.position = float4(position, 0, 1);
	output.uv = uv;
	return output;
}

