#pragma once

#include "../Entity/Entity.h"
#include "../Entity/Signature.h"

namespace ecs
{

class IView
{
public:
	virtual ~IView() = default;
	virtual void OnEntitySignatureChanged(Entity entity, Signature signature) = 0;
	virtual void OnEntityDestroyed(Entity entity) = 0;
};

} // namespace ecs
