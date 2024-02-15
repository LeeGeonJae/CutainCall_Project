#include "pch.h"
#include "RigidStaticComponent.h"
#include "PhysicsManager.h"

PxRigidStatic* RigidStaticComponent::CreateStaticRigidBody(Geometry geometryType, const std::vector<float>& geometryArgs, const std::vector<float>& materialArgs, bool isTrigger)
{
	std::shared_ptr<PxGeometry> geometry;
	switch (geometryType)
	{
	case RigidBodyComponent::Geometry::Sphere:
	{
		geometry = CreateGeometry<Geometry::Sphere>(geometryArgs[0]);
	}
	break;
	case RigidBodyComponent::Geometry::Capsule:
	{
		geometry = CreateGeometry<Geometry::Capsule>(geometryArgs[0], geometryArgs[1]);
	}
	break;
	case RigidBodyComponent::Geometry::Box:
	{
		geometry = CreateGeometry<Geometry::Box>(geometryArgs[0], geometryArgs[1], geometryArgs[2]);
	}
	break;
	default:
	{
		LOG_ENGINE("CreateStaticRigidBody Argument Error");
		assert(false);
		return nullptr;
	}
	break;
	}

	GetOwnerTrasnform();

	PxMaterial* material = CreateMaterial(materialArgs[0], materialArgs[1], materialArgs[2]);

	m_rigidStatic = PxCreateStatic(*PhysicsManager::GetInstance()->GetPhysics(), ConvertPxTransform(), *geometry, *material);

	if (m_rigidStatic == nullptr)
	{
		LOG_ENGINE("CreateStaticRigidBody Error");
		assert(false);
		return nullptr;
	}

	SetCollisionFilter(m_rigidStatic);

	if (isTrigger)
	{
		PxShape* sensorShape;
		m_rigidStatic->getShapes(&sensorShape, 1);
		sensorShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		sensorShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	}

#ifdef _DEBUG
	m_rigidStatic->setActorFlag(PxActorFlag::eVISUALIZATION, true);
#endif

	material->release();

	PhysicsManager::GetInstance()->AddCollisionOwner(m_rigidStatic, GetOwner());
	return m_rigidStatic;
}

void RigidStaticComponent::AddTriggerShape(Geometry geometryType, const std::vector<float>& geometryArgs, const Math::Vector3& offsetTrasnform, const Math::Vector3& offsetRotation)
{
	if (m_rigidStatic == nullptr)
	{
		LOG_ENGINE("RigidDynamicComponent not initialized");
		assert(false);
		return;
	}

	std::shared_ptr<PxGeometry> geometry;
	switch (geometryType)
	{
	case RigidBodyComponent::Geometry::Sphere:
	{
		geometry = CreateGeometry<Geometry::Sphere>(geometryArgs[0]);
	}
	break;
	case RigidBodyComponent::Geometry::Capsule:
	{
		geometry = CreateGeometry<Geometry::Capsule>(geometryArgs[0], geometryArgs[1]);
	}
	break;
	case RigidBodyComponent::Geometry::Box:
	{
		geometry = CreateGeometry<Geometry::Box>(geometryArgs[0], geometryArgs[1], geometryArgs[2]);
	}
	break;
	default:
	{
		LOG_ENGINE("CreateDynamicRigidBody Argument Error");
		assert(false);
		return;
	}
	break;
	}

	PxMaterial* triggerMaterial = CreateMaterial(0.0f, 0.0f, 0.0f);
	const PxShapeFlags shapeFlags = PxShapeFlag::eVISUALIZATION | PxShapeFlag::eTRIGGER_SHAPE;
	PxShape* triggerShape = PhysicsManager::GetInstance()->GetPhysics()->createShape(*geometry, *triggerMaterial, false, shapeFlags);

	if (triggerShape)
	{

		if (offsetTrasnform != Math::Vector3(0.f, 0.f, 0.f)
			|| offsetRotation != Math::Vector3(0.f, 0.f, 0.f))
		{
			auto quat = Math::Quaternion::CreateFromYawPitchRoll(XMConvertToRadians(offsetRotation.y), XMConvertToRadians(offsetRotation.x), XMConvertToRadians(offsetRotation.z));

			triggerShape->setLocalPose(PxTransform(physx::PxVec3T(offsetTrasnform.x, offsetTrasnform.y, offsetTrasnform.z), physx::PxQuat(quat.x, quat.y, quat.z, quat.w)));
		}

		m_rigidStatic->attachShape(*triggerShape);
	}

	triggerMaterial->release();
	triggerShape->release();
}

PxRigidStatic* RigidStaticComponent::CreatePlane(float nx, float ny, float nz, float distance, const std::vector<float>& materialArgs)
{
	PxMaterial* material = CreateMaterial(materialArgs[0], materialArgs[1], materialArgs[2]);

	GetOwnerTrasnform();

	m_rigidStatic = PxCreatePlane(*PhysicsManager::GetInstance()->GetPhysics(), PxPlane(nx, ny, nz, distance).transform(ConvertPxTransform()), *material);

	if (m_rigidStatic == nullptr)
	{
		LOG_ENGINE("CreatePlane Error");
		assert(false);
		return nullptr;
	}

	SetCollisionFilter(m_rigidStatic);

#ifdef _DEBUG
	m_rigidStatic->setActorFlag(PxActorFlag::eVISUALIZATION, true);
#endif

	material->release();

	PhysicsManager::GetInstance()->AddCollisionOwner(m_rigidStatic, GetOwner());
	return m_rigidStatic;
}

void RigidStaticComponent::Initialize()
{
	PhysicsManager::GetInstance()->AddRigidStaticComponent(shared_from_this());
	PhysicsManager::GetInstance()->GetScene()->addActor(*m_rigidStatic);
}

void RigidStaticComponent::Update(float deltaTime)
{
}
