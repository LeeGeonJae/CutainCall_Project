#pragma once
#include "../Engine/CollisionHandler.h"
class ClearCollisionHandler :
    public CollisionHandler
{
    // CollisionHandler을(를) 통해 상속됨
    void OnContactEnter(std::weak_ptr<GameObject> actor1, std::weak_ptr<GameObject> actor2, const physx::PxContactPair& cp) override;
    void OnContactStay(std::weak_ptr<GameObject> actor1, std::weak_ptr<GameObject> actor2, const physx::PxContactPair& cp) override;
    void OnContactExit(std::weak_ptr<GameObject> actor1, std::weak_ptr<GameObject> actor2, const physx::PxContactPair& cp) override;
    void OnTriggerEnter(std::weak_ptr<GameObject> actor1, std::weak_ptr<GameObject> actor2, const physx::PxTriggerPair& tp) override;
    void OnTriggerExit(std::weak_ptr<GameObject> actor1, std::weak_ptr<GameObject> actor2, const physx::PxTriggerPair& tp) override;
};

