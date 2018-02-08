cbuffer globals
{
	float2 size;
};

struct VS_INPUT
{
    float3 pos : POSITION;
	float2 tex : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

VS_OUTPUT vs_main(VS_INPUT In)
{
	VS_OUTPUT Out;
	Out.pos.x = ((In.pos.x - 0.5f) / (size.x * 0.5f)) - 1.0f;
	Out.pos.y = -(((In.pos.y - 0.5f) / (size.y * 0.5f)) - 1.0f);
	Out.pos.z = 0.f;
	Out.pos.w = 1.f;
	Out.tex = In.tex;
	return Out;
}

Texture2D tex;
SamplerState TexSampler;

float4 ps_main(VS_OUTPUT In) : SV_TARGET
{
	return tex.Sample(TexSampler, In.tex);
}
