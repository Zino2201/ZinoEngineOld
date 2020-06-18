#pragma once

#include "Type.h"
#include "Struct.gen.h"

namespace ZE::Refl
{

/**
 * A property
 */
ZCLASS()
class REFLECTION_API CProperty
{
    REFL_BODY()

public:
	CProperty(const char* InName,
		const uint64_t& InSize,
		const size_t& InOffset) :
		Name(InName), Size(InSize), Offset(InOffset) {}

	void* GetData(void* InPtr) const
	{
		uint8_t* Dst = reinterpret_cast<uint8_t*>(InPtr);
		return Dst + Offset;
	}

	template<typename T>
	T* GetData(void* InPtr) const
	{
		return reinterpret_cast<T*>(GetData(InPtr));
	}
private:
    const char* Name;
    size_t Size;
	size_t Offset;
};

/**
 * A C++ struct
 * Can hold properties and have oop
 */
ZCLASS()
class CStruct : public CType
{
	REFL_BODY()

	class IInstantiateFunc {};

	template<typename... Args>
	class CInstantiateFunc : public IInstantiateFunc
	{
    public:
        CInstantiateFunc(const std::function<void*(Args...)>& InFunc) :
            Func(InFunc) {}

        std::function<void*(Args...)> Func;
	};

	template<typename... Args>
	class CPlacementNewFunc : public IInstantiateFunc
	{
	public:
		CPlacementNewFunc(const std::function<void(void*, Args...)>& InFunc) :
			Func(InFunc) {}

		std::function<void(void*, Args...)> Func;
	};
public:
	CStruct(const char* InName,
		const uint64_t& InSize)
        : CType(InName, InSize) 
    {
        
    }

    ~CStruct()
    {

    }

    template<typename... Args>
    void AddInstantiateFunc(const std::function<void*(Args...)>& InFunc)
    {
        InstantiateFunc = std::make_unique<CInstantiateFunc<Args...>>(InFunc);
    }   
    
    template<typename... Args>
    void AddPlacementNewFunc(const std::function<void(void*, Args...)>& InFunc)
    {
        PlacementNewFunc = std::make_unique<CPlacementNewFunc<Args...>>(InFunc);
    }

    /**
     * Instantiate the struct
     */
    template<typename T, typename... Args>
    T* Instantiate(Args&&... InArgs)
    {
        must(bIsInstanciable);

        CInstantiateFunc<Args...>* Func = 
            static_cast<CInstantiateFunc<Args...>*>(InstantiateFunc.get());
        T* Obj = reinterpret_cast<T*>(Func->Func(std::forward<Args>(InArgs)...));
        return Obj;
    }

    /**
     * Placement new
     */
    template<typename... Args>
	void PlacementNew(void* InP, Args&&... InArgs)
	{
		must(bIsInstanciable);

		CPlacementNewFunc<Args...>* Func =
			static_cast<CPlacementNewFunc<Args...>*>(PlacementNewFunc.get());
		Func->Func(InP, std::forward<Args>(InArgs)...);
	}

    REFLECTION_API static void AddStruct(CStruct* InStruct);

    void AddProperty(const CProperty& InProperty)
    {
        Properties.push_back(InProperty);
    }

    REFLECTION_API void AddParent(CStruct* InParent);
    REFLECTION_API bool IsDerivedFrom(CStruct* InParent) const;

	template<typename T>
	static CStruct* Get()
	{
		CStruct* Struct = Get(TTypeName<T>::Name);
		must(Struct); // This struct is not registered

		return Struct;
	}

    REFLECTION_API static CStruct* Get(const char* InName);

    const std::vector<CStruct*>& GetParents() { return Parents; }
    REFLECTION_API static const std::vector<CStruct*>& GetStructs() { return Structs; }
protected:
    std::unique_ptr<IInstantiateFunc> InstantiateFunc;
    std::unique_ptr<IInstantiateFunc> PlacementNewFunc;
    std::vector<CProperty> Properties;
    std::vector<CStruct*> Parents;
    REFLECTION_API inline static std::vector<CStruct*> Structs;
    bool bIsInstanciable;
public:
    std::vector<const char*> Refl_ParentsWaitingAdd;
};

}