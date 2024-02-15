#pragma once
#include "PxPhysicsAPI.h"
/// <summary>
/// CollisionHandler
/// �浹 ���� �����ϴ� ���� ���� Ŭ����
/// �̰� ��ӽ�Ų Ŭ�������� �� �Լ� ���� ���ְ�
/// ������ٵ� ������Ʈ���� AddCollisionHandler�� �ٿ��ָ� ��.
/// </summary>
class GameObject;

class CollisionHandler
{
public:
	virtual ~CollisionHandler() {}

	virtual void OnEnter(std::weak_ptr<GameObject> actor1, std::weak_ptr<GameObject> actor2) abstract;
	virtual void OnStay(std::weak_ptr<GameObject> actor1, std::weak_ptr<GameObject> actor2) abstract;
	virtual void OnExit(std::weak_ptr<GameObject> actor1, std::weak_ptr<GameObject> actor2) abstract;
};

