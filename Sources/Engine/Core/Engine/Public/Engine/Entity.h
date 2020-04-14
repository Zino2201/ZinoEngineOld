//#pragma once
//
//#include "EngineCore.h"
//#include "Object/Object.h"
//
//namespace ZE::ECS
//{
//
///**
// * An entity ID
// */
//using EntityID = uint32_t;
//
///**
// * An entity
// * Contains a list of components
// */
//class CEntity : public CObject
//{
//private:
//    /** Index to the entity's component list */
//    size_t ComponentListIndex;
//};
//
///**
// * Component type
// */
//enum class EEntityComponentType
//{
//    /** 
//     * This component can be instanced multiple times,
//     * Should be the default type for most components 
//     */
//    Instancable,
//
//    /** 
//     * This component should be created only once,
//     * Useful for global logic
//     * Globals components are stored in another array that normal components
//     */
//    Global
//};
//
///**
// * An entity component
// */
//struct SEntityComponent
//{
//    DECLARE_STRUCT(SEntityComponent);
//};
//
///**
// * A component system
// * Loaded automatically
// */
//class IEntityComponentSystem : public CObject
//{
//    DECLARE_OBJECT(IEntityComponentSystem, CObject);
//
//public:
//    virtual const char* GetName() = 0;
//    virtual CStruct* GetComponentStruct() = 0;
//    virtual EEntityComponentType GetComponentType() = 0;
//};
//
///**
// * Global manager for systems
// */
//class CEntityComponentSystemManager : public CObject
//{
//    DECLARE_OBJECT(CEntityComponentSystemManager, CObject);
//
//public:
//    void Initialize();
//private:
//    std::unordered_map<CStruct*, std::unique_ptr<CEntityComponentSystemManager*>> Systems;
//};
//
//} /* namespace ZE */