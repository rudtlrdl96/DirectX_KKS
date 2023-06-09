#include "PrecompileHeader.h"
#include "BaseContentActor.h"
#include "ContentLevel.h"

static UINT NextCode = 1;

std::shared_ptr<BaseContentActor> BaseContentActor::IsFocusPtr = nullptr;

BaseContentActor::BaseContentActor() :
	Code(++NextCode)
{
}

BaseContentActor::~BaseContentActor()
{
}

ContentLevel* BaseContentActor::GetContentLevel() const
{
	return static_cast<ContentLevel*>(GetLevel());
}

