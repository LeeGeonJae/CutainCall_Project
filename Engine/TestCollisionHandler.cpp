#include "pch.h"
#include "TestCollisionHandler.h"

void TestCollisionHandler::OnEnter(std::weak_ptr<GameObject> actor1, std::weak_ptr<GameObject> actor2)
{
	LOG_ENGINE("Enter");
}

void TestCollisionHandler::OnStay(std::weak_ptr<GameObject> actor1, std::weak_ptr<GameObject> actor2)
{
	LOG_ENGINE("Stay");
}

void TestCollisionHandler::OnExit(std::weak_ptr<GameObject> actor1, std::weak_ptr<GameObject> actor2)
{
	LOG_ENGINE("Exit");
}
