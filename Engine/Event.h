#pragma once

#include <variant>
#include <typeinfo>

class EventListener;
enum class eEventType;
enum class eWorldType;
enum class eObjectType;

using eventVariant = std::variant<eWorldType, eObjectType, bool, const char*>;

class Event
{
public:
	template <typename T>
	T GetData()
	{
		std::string key = typeid(T).name();
		int idx = variantMap.find(key)->second;
		return std::get<T>(variants[idx]);
	}

	template <typename... Args>
	Event(Args... args)
	{
		(InsertEventData(args), ...);
	}

private:
	template <typename T>
	void InsertEventData(T data)
	{
		variantMap.emplace(typeid(data).name(), variants.size());
		variants.emplace_back(data);
	}


public:
	eEventType eventID;
	EventListener* eventListener = nullptr;

private:
	std::vector<eventVariant> variants;
	std::unordered_map<std::string, int> variantMap;
};
