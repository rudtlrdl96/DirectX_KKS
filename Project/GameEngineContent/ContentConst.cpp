#include "PrecompileHeader.h"
#include "ContentConst.h"

//Physics
const float4 ContentConst::Gravity = float4(0.0f, -9.807f, 0.0f, 0.0f);

// Player
// �� Arm
const float ContentConst::ArmChainInterval = 30.0f;
const float ContentConst::ArmShotSpeed = 800.0f;
const int ContentConst::ArmChainCount = 20;


ContentConst::ContentConst()
{
}

ContentConst::~ContentConst()
{
}
