#include "pch.h"
#include "RigidBodyComponent.h"
#include "PhysicsManager.h"
#include "SceneComponent.h"
#include "GameObject.h"

void RigidBodyComponent::Initialize()
{
}

void RigidBodyComponent::Update(float deltaTime)
{
}

void RigidBodyComponent::GetOwnerTrasnform()
{
	m_pOwnerPosition = &m_pOwner.lock()->GetRootComponent().lock()->GetLocalPosition();
	m_pOwnerRotation = &m_pOwner.lock()->GetRootComponent().lock()->GetLocalRotation();
}

PxMaterial* RigidBodyComponent::CreateMaterial(float staticFriction, float dynamicFriction, float restitution)
{
	return PhysicsManager::GetInstance()->GetPhysics()->createMaterial(staticFriction, dynamicFriction, restitution);
}

void RigidBodyComponent::SetCollisionFilter(PxRigidActor* actor)
{
    PxFilterData filterData;

    filterData.word0 = 1 << static_cast<PxU32>(m_pOwner.lock()->GetObjectType());

    PxShape* shapes[10];
    actor->getShapes(shapes, 10);

    for (PxU32 i = 0; i < actor->getNbShapes(); ++i)
    {
        shapes[i]->setSimulationFilterData(filterData);
        shapes[i]->setQueryFilterData(filterData);
    }
}

PxTransform RigidBodyComponent::ConvertPxTransform()
{
	auto ownQuat = Math::Quaternion::CreateFromYawPitchRoll(XMConvertToRadians(m_pOwnerRotation->y), XMConvertToRadians(m_pOwnerRotation->x), XMConvertToRadians(m_pOwnerRotation->z));

	return PxTransform(PxVec3T(m_pOwnerPosition->x, m_pOwnerPosition->y, m_pOwnerPosition->z), PxQuat(ownQuat.x , ownQuat.y, ownQuat.z, ownQuat.w));
}
