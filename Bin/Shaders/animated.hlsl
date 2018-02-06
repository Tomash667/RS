cbuffer globals
{
	matrix matWorldViewProj;
	matrix matBones[32];
};

struct VS_INPUT
{
    float3 pos : POSITION;
	half weight : BLENDWEIGHT0;
	int2 indices : BLENDINDICES0;
	float3 normal : NORMAL;
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
		
	float3 pos = (mul(float4(In.pos,1), matBones[In.indices[0]]) * In.weight).xyz;
	pos += (mul(float4(In.pos,1), matBones[In.indices[1]]) * (1-In.weight)).xyz;
	Out.pos = mul(float4(pos,1), matWorldViewProj);
	
	//float3 normal = mul(float4(In.normal,1), matBones[In.indices[0]]) * In.weight;
	//normal += mul(float4(In.normal,1), matBones[In.indices[1]]) * (1-In.weight);
	//Out.normal = mul(normal, matWorld);
	
	Out.tex = In.tex;
	
	return Out;
}

Texture2D tex;
SamplerState TexSampler;

float4 ps_main(VS_OUTPUT In) : SV_TARGET
{
	return tex.Sample(TexSampler, In.tex);
}
