#pragma once
#include "CollisionHandler.h"
class TestCollisionHandler :
    public CollisionHandler
{
    // CollisionHandler을(를) 통해 상속됨
    void OnEnter(std::weak_ptr<GameObject> actor1, std::weak_ptr<GameObject> actor2) override;
    void OnStay(std::weak_ptr<GameObject> actor1, std::weak_ptr<GameObject> actor2) override;
    void OnExit(std::weak_ptr<GameObject> actor1, std::weak_ptr<GameObject> actor2) override;
};

