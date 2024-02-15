#pragma once

#include "Object.h"

#include <memory>
#include <unordered_map>

#include "Component.h"

class World;
class Component;
class SceneComponent;

enum class eObjectType
{
	TEST,
	PLANE,
	LEVEL,
	PLAYER,
	CAMERA,

	END
};

/// <summary>
/// GameObject들의 원형
/// 게임오브젝트 자체로는 아무것도 가지지 않는다.
/// 컴포넌트 중 대표 컴포넌트를 지정하여 해당 컴포넌트의 위치값을 대표위치값으로 가진다.
/// </summary>
class GameObject
	: public Object
	, public std::enable_shared_from_this<GameObject>
{
public:
	GameObject() = default;
	virtual ~GameObject();

public:
	void SetName(std::string_view name) { m_name = name; }
	void SetObjectType(eObjectType type) { m_objectType = type; }
	void SetOwnerWorld(std::shared_ptr<World> pWorld) { m_pOwnerWorld = pWorld; }
	void SetOwnerObject(std::shared_ptr<GameObject> pObject) { m_pOwnerObj = pObject; }
	void SetRootComponent(std::shared_ptr<SceneComponent> pRootcomponent) { m_pRootComponent = pRootcomponent; }
	void SetPosition(Vector3 position);
	void SetRotation(Vector3 rotation);
	void SetScale(Vector3 scale);

	std::string GetName() const { return m_name; }
	eObjectType GetObjectType() const { return m_objectType; }
	std::weak_ptr<World> GetOwnerWorld() const { return m_pOwnerWorld; }
	std::weak_ptr<SceneComponent> GetRootComponent() const { return m_pRootComponent; }
	std::weak_ptr<Component> GetComponent(std::string_view name) const;
	const Vector3& GetPosition() const;
	const Vector3& GetRotation() const;
	const Vector3& GetScale() const;
	const Matrix& GetTransform() const;

public:
	virtual void Initialize();
	virtual void Update(float deltaTime);


protected:
	std::string m_name;
	eObjectType m_objectType = eObjectType::END;

	std::weak_ptr<World> m_pOwnerWorld;
	std::weak_ptr<GameObject> m_pOwnerObj;

	std::unordered_map<std::string, int> m_componentMap; // GetComponent 속도 향상을 위한 Query용 자료구조
	std::vector<std::shared_ptr<Component>> m_ownComponents;
	std::shared_ptr<SceneComponent> m_pRootComponent;	// m_pRootComponent는 m_OwnedComponent 중 하나로 설정, 대표 위치값

public:
	template <typename T>
	std::weak_ptr<T> CreateComponent(std::string_view name)
	{
		std::shared_ptr<T> component = std::make_shared<T>();
		Component* inputComponent = dynamic_cast<Component*>(component.get());
		assert(inputComponent);

		component->SetName(name);
		component->SetOwner(shared_from_this());

		int keyIdx = m_ownComponents.size();
		m_componentMap.emplace(name, keyIdx);

		m_ownComponents.emplace_back(component);

		return component;
	}

	template <typename T>
	std::weak_ptr<T> GetComponentByTypeName() const
	{
		for (const auto& component : m_ownComponents)
		{
			std::shared_ptr<T> typedComponent = std::dynamic_pointer_cast<T>(component);
			if (typedComponent)
			{
				return typedComponent;
			}
		}
		return std::weak_ptr<T>();
	}
};