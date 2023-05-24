// ���̴��� ¥�ԵǸ� ������ ��Ģ�� ���Ѿ� �Ѵ�.

// 0~ 16�� ���� 
// ������ ���ٰ� ���°� �ƴϿ���.
cbuffer TransformData : register(b0)
{
    float4 Scale;
    float4 Rotation;
    float4 Quaternion;
    float4 Position;

    float4 LocalScale;
    float4 LocalRotation;
    float4 LocalQuaternion;
    float4 LocalPosition;

    float4 WorldScale;
    float4 WorldRotation;
    float4 WorldQuaternion;
    float4 WorldPosition;

    float4x4 ScaleMatrix;
    float4x4 RotationMatrix;
    float4x4 PositionMatrix;
    float4x4 LocalWorldMatrix;
    float4x4 WorldMatrix;
    float4x4 View;
    float4x4 Projection;
    float4x4 ViewPort;
    float4x4 WorldViewProjectionMatrix;
}

struct Input
{
    float4 Pos : POSITION;
    float4 UV : TEXCOORD;
};

struct OutPut
{
    // �����Ͷ������� �� ��������
    // w���� ����  ����Ʈ ���ϰ� �ȼ� �������� �������� ������ ���� �����ž�.
    float4 Pos : SV_Position;
    float4 UV : TEXCOORD;
};

OutPut Texture_VS(Input _Value)
{
    OutPut OutPutValue = (OutPut) 0;
	
    _Value.Pos.w = 1.0f;
    OutPutValue.Pos = mul(_Value.Pos, WorldViewProjectionMatrix);
    OutPutValue.UV = _Value.UV;
    
    return OutPutValue;
}

cbuffer ProgressBuffer : register(b1)
{
    float4 OutColor;    
    float2 ColorProgressStart;
    float2 ColorProgressEnd;    
}

Texture2D DiffuseTex : register(t0);
SamplerState WRAPSAMPLER : register(s0);

float4 Texture_PS(OutPut _Value) : SV_Target0
{    
    float2 Uv = _Value.UV.xy;
    float4 Color = DiffuseTex.Sample(WRAPSAMPLER, Uv);
    
    if (Uv.x < ColorProgressStart.x || Uv.x > ColorProgressEnd.x)
    {
        Color.a = 0;
    }
    
    if (Uv.y < ColorProgressStart.y || Uv.y > ColorProgressEnd.y)
    {
        Color.a = 0;
    }
        
    Color.xyz += OutColor.xyz;
    Color *= OutColor.a;
        
    return Color;
}