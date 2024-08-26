#pragma once
#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>
#include <memory>
#include <deque>
#include "../Logger/Logger.hpp"


const unsigned int MAX_COMPONENTS = 32;

/*
 a bitset (1s and 0s) to keep track of which components an enity has,
 and also helps keep track of which entities a given sytem is interested in.
*/
typedef std::bitset<MAX_COMPONENTS> Signature;

class Entity {
public:
	Entity(int id) : id(id) {};
	Entity(const Entity& entity) = default;
	void Kill();
	int GetId() const;

	void Tag(const std::string& tag);
	bool HasTag(const std::string& tag) const;
	void Group(const std::string& group);
	bool BelongsToGroup(const std::string& group) const;

	// operator overloads
	Entity& operator = (const Entity& other) = default;
	bool operator == (const Entity& other) const { return id == other.id; }
	bool operator != (const Entity& other) const { return id != other.id; }
	bool operator > (const Entity& other) const { return id > other.id; }
	bool operator < (const Entity& other) const { return id < other.id; }

	template <typename TComponent, typename ...TArgs> void AddComponent(TArgs&& ...args);
	template <typename TComponent> void RemoveComponent();
	template <typename TComponent> bool HasComponent() const;
	template <typename TComponent> TComponent& GetComponent() const;

	class Registry* registry = nullptr;

private:
	int id;
};

struct IComponent {
protected:
	static int nextId;
};

// assign a unique id to a component type
template <typename T>
class Component : public IComponent{
public:
	// returns unique id of Component<T>
	static int GetId() {
		static int id = nextId++;
		return id;
	}
};

/*
 System
 The system processes entities that contain a specific signature
*/
class System {
public:
	System() = default;
	~System() = default;

	void AddEntityToSystem(Entity entity);
	void RemoveEntityFromSystem(Entity entity);
	std::vector<Entity> GetSystemEntities() const;
	const Signature& GetComponentSignature() const;

	// define component type entity must have to be considered by system
	template <typename TComponent> void RequireComponent();

private:
	Signature componentSignature;
	std::vector<Entity> entities;
};

/*
 IPool
 The IPool class is a parent class to pool (wrapper)
*/
class IPool {
public:
	virtual ~IPool() = default;
	virtual void RemoveEntityFromPool(int entityId) = 0;
};

/*
 Pool
 A pool is just a vector of objects of type T
*/
template <typename T>
class Pool : public IPool{
public:
	Pool(int capacity = 100) {
		size = 0;
		data.resize(capacity);
	}

	virtual ~Pool() = default;

	bool IsEmpty() const {
		return size == 0;
	}

	int GetSize() const {
		return size;
	}

	void Resize(int capacity) {
		data.resize(capacity);
	}

	void Clear() {
		data.clear();
		size = 0;
	}

	void Add(T object) {
		data.push_back(object);
	}

	void Set(int entityId, T object) {
		if (entityIdToIndex.find(entityId) != entityIdToIndex.end()) {
			// if element already exists, replace the object
			int index = entityIdToIndex[entityId];
			data[index] = object;
		}
		else {
			// add new object, track entity id and vector index
			int index = size;
			entityIdToIndex.emplace(entityId, index);
			indexToEntityId.emplace(index, entityId);
			if (index >= data.capacity()) {
				// resize if data is not big enough
				data.resize(size * 2);
			}
			data[index] = object;
			size++;
		}
	}

	void Remove(int entityId) {
		// copy last element to the deleted position to keep the array packed
		int indexOfRemoved = entityIdToIndex[entityId];
		int indexOfLast = size - 1;
		data[indexOfRemoved] = data[indexOfLast];

		// update the index-entity maps to point to correct elements
		int entityIdOfLastElement = indexToEntityId[indexOfLast];
		entityIdToIndex[entityIdOfLastElement] = indexOfRemoved;
		indexToEntityId[indexOfRemoved] = entityIdOfLastElement;

		entityIdToIndex.erase(entityId);
		indexToEntityId.erase(indexOfLast);

		size--;
	}

	void RemoveEntityFromPool(int entityId) override {
		if (entityIdToIndex.find(entityId) != entityIdToIndex.end()) {
			Remove(entityId);
		}
	}

	T& Get(int entityId) {
		int index = entityIdToIndex[entityId];
		return static_cast<T&>(data[index]);
	}

	T& operator [] (unsigned int index) {
		return data[index];
	}
private:
	// track vector of objects and current number of elements
	std::vector<T> data;
	int size;

	// maps to track entity ids per index so vector is always packed
	std::unordered_map<int, int> entityIdToIndex;
	std::unordered_map<int, int> indexToEntityId;
};

/*
 Registry
 The registry manages the creation and destruction of entities, as well as
 adding systems and adding components to entities
*/
class Registry {
public:
	Registry() { Logger::Log("Registry constuctor called"); }
	~Registry() { Logger::Log("Registry deconstuctor called"); }

	void Update();

	/*
	 Create an entity and add it to a set of entities
	 that will be added in the next registry Update()
	 @return entity
	*/
	Entity CreateEntity();
	void KillEntity(Entity entity);

	void TagEntity(Entity entity, const std::string& tag);
	bool EntityHasTag(Entity entity, const std::string& tag) const;
	Entity GetEntityByTag(const std::string& tag) const;
	void RemoveEntityTag(Entity entity);

	void GroupEntity(Entity entity, const std::string& group);
	bool EntityBelongsToGroup(Entity entity, const std::string& group) const;
	std::vector<Entity> GetEntitiesByGroup(const std::string& group) const;
	void RemoveEntityGroup(Entity entity);

	// Component management
	template <typename TComponent, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);
	// remove component from entity
	template <typename TComponent> void RemoveComponent(Entity entity);
	// check if entity has a component
	template <typename TComponent> bool HasComponent(Entity entity) const;
	template <typename TComponent> TComponent& GetComponent(Entity entity) const;

	// System management

	/*
	 Add system to systems map
	*/
	template <typename TSystem, typename ...TArgs> void AddSystem(TArgs&& ...args);
	/*
	 Remove system from systems map
	*/
	template <typename TSystem> void RemoveSystem();
	/*
	 Check if systems map contains system
	 @return bool
	*/
	template <typename TSystem> bool HasSystem() const;
	/*
	 Gets reference to a system from systems map
	 @return TSystem&
	*/
	template <typename TSystem> TSystem& GetSystem() const;

	// Add and remove entities from systems
	void AddEntityToSystems(Entity entity);
	void RemoveEntityFromSystems(Entity entity);

private:
	int numEntities = 0;

	// Vector of component pools, each pool contains all the data for a certain component type
	// Vector index = component type id
	// Pool index = entity id
	std::vector<std::shared_ptr<IPool>> componentPools;

	// Vector of component signatures per entity, saying which component is used for each entity
	// Vector index = entity id
	std::vector<Signature> entityComponentSignatures;

	// Unordered map of systems
	std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

	// Set of entities that are flagged to be added in the next registry Update()
	std::set<Entity> entitiesToBeAdded;
	// Set of entities that are flagged to be removed in the next registry Update()
	std::set<Entity> entitiesToBeKilled;

	std::unordered_map<std::string, Entity> entityPerTag;
	std::unordered_map<int, std::string> tagPerEntity;

	std::unordered_map<std::string, std::set<Entity>> entitiesPerGroup;
	std::unordered_map<int, std::string> groupPerEntity;


	// List of free ids
	std::deque<int> freeIds;
};

/*
* ********************************
* Registry Component Template defintions
* ********************************
*/

template <typename TComponent>
void System::RequireComponent() {
	const int componentId = Component<TComponent>::GetId();
	componentSignature.set(componentId);
}

template <typename TComponent, typename ...TArgs>
void Registry::AddComponent(Entity entity, TArgs&& ...args) {
	const int componentId = Component<TComponent>::GetId();
	const int entityId = entity.GetId();

	// If component id is greater than the current size of the componentPool, resize the vector
	if (componentId >= componentPools.size())
		componentPools.resize(componentId + 1, nullptr);

	// If we don't have a Pool for that component type, create one
	if (!componentPools[componentId]) {
		std::shared_ptr<Pool<TComponent>> newComponentPool = std::make_shared<Pool<TComponent>>();
		componentPools[componentId] = newComponentPool;
	}

	// Get the pool of component values for that component type
	std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

	// Create a new component object of the type T and forward the parameters to the constructor
	TComponent newComponent(std::forward<TArgs>(args)...);

	// Add the new component to the component pool list
	componentPool->Set(entityId, newComponent);

	// Change the component signature of the entity and set the component id on the bitset to 1
	entityComponentSignatures[entityId].set(componentId);

	Logger::Log("Component id: " + std::to_string(componentId) + " was added to entity id " + std::to_string(entityId));
}

template <typename TComponent>
void Registry::RemoveComponent(Entity entity) {
	const int componentId = Component<TComponent>::GetId();
	const int entityId = entity.GetId();

	std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
	componentPool->Remove(entityId);

	entityComponentSignatures[entityId].set(componentId, false);

	Logger::Log("Component id: " + std::to_string(componentId) + " was removed from entity id " + std::to_string(entityId));
}

template <typename TComponent>
bool Registry::HasComponent(Entity entity) const {
	const int componentId = Component<TComponent>::GetId();
	const int entityId = entity.GetId();
	return entityComponentSignatures[entityId].test(componentId);
}

template <typename TComponent>
TComponent& Registry::GetComponent(Entity entity) const {
	const int componentId = Component<TComponent>::GetId();
	const int entityId = entity.GetId();
	auto componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
	return componentPool->Get(entityId);
 }

/*
* ********************************
* Entity Component Template defintions
* ********************************
*/

template <typename TComponent, typename ...TArgs>
void Entity::AddComponent(TArgs&& ...args) {
	registry->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}

template <typename TComponent>
void Entity::RemoveComponent() {
	registry->RemoveComponent<TComponent>(*this);
}

template <typename TComponent>
bool Entity::HasComponent() const {
	return registry->HasComponent<TComponent>(*this);
}

template <typename TComponent>
TComponent& Entity::GetComponent() const {
	return registry->GetComponent<TComponent>(*this);
}

/*
* ********************************
* System Template defintions
* ********************************
*/

template <typename TSystem, typename ...TArgs>
void Registry::AddSystem(TArgs&& ...args) {
	std::shared_ptr<TSystem> newSystem = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
	systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}

template <typename TSystem>
void Registry::RemoveSystem() {
	auto system = systems.find(std::type_index(typeid(TSystem)));
	systems.erase(system);
}

template <typename TSystem>
bool Registry::HasSystem() const {
	return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template <typename TSystem>
TSystem& Registry::GetSystem() const {
	auto system = systems.find(std::type_index(typeid(TSystem)));
	return *(std::static_pointer_cast<TSystem>(system->second));
}