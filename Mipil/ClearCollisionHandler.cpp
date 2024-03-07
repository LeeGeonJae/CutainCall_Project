#include "pch.h"
#include "ClearCollisionHandler.h"
#include "../Engine/GameObject.h"
#include "../Engine/WorldManager.h"
#include "EndingFreeWorld.h"
#include "StartFreeWorld.h"

void ClearCollisionHandler::OnContactEnter(std::weak_ptr<GameObject> actor1, std::weak_ptr<GameObject> actor2, const physx::PxContactPair& cp)
{
}

void ClearCollisionHandler::OnContactStay(std::weak_ptr<GameObject> actor1, std::weak_ptr<GameObject> actor2, const physx::PxContactPair& cp)
{
}

void ClearCollisionHandler::OnContactExit(std::weak_ptr<GameObject> actor1, std::weak_ptr<GameObject> actor2, const physx::PxContactPair& cp)
{
}

void ClearCollisionHandler::OnTriggerEnter(std::weak_ptr<GameObject> actor1, std::weak_ptr<GameObject> actor2, const physx::PxTriggerPair& tp)
{
	if (actor1.lock()->GetObjectType() == eObjectType::READYTRIGGER &&
		actor2.lock()->GetObjectType() == eObjectType::PLAYER)
	{
		auto world = actor1.lock()->GetOwnerWorld().lock();
		assert(world);

		auto endworld = std::dynamic_pointer_cast<EndingFreeWorld>(world);

		if (endworld)
		{
			endworld->TriggerDim(true);
			endworld->DeletePlayerController();

			WorldManager::GetInstance()->PushSendQueue(
				WorldManager::GetInstance()->SerializeBuffer(sizeof(PacketC2S_IsAllEnd), C2S_IS_ALL_END, nullptr),
				sizeof(PacketC2S_IsAllEnd));
		}
	}
}

void ClearCollisionHandler::OnTriggerExit(std::weak_ptr<GameObject> actor1, std::weak_ptr<GameObject> actor2, const physx::PxTriggerPair& tp)
{
}
