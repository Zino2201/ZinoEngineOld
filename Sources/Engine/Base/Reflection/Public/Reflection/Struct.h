#pragma once

#include "Type.h"
#include "Property.h"
#include "Struct.gen.h"

namespace ZE::Refl
{

/**
 * A C++ struct
 * Can hold properties and have oop
 */
ZCLASS()
class REFLECTION_API CStruct : public CType
{
	ZE_REFL_BODY()

	class IInstantiateFunc {};

	template<typename... Args>
	struct CInstantiateFunc : public IInstantiateFunc
	{
        CInstantiateFunc(const std::function<void*(Args...)>& InFunc) :
            Func(InFunc) {}

        std::function<void*(Args...)> Func;
	};

	template<typename... Args>
	struct CPlacementNewFunc : public IInstantiateFunc
	{
		CPlacementNewFunc(const std::function<void(void*, Args...)>& InFunc) :
			Func(InFunc) {}

		std::function<void(void*, Args...)> Func;
	};
public:
	CStruct(const char* InName,
		const uint64_t& InSize)
        : CType(InName, InSize) {}

    template<typename... Args>
    void AddInstantiateFunc(const std::function<void*(Args...)>& InFunc)
    {
        InstantiateFunc = std::make_unique<CInstantiateFunc<Args...>>(InFunc);
        bIsInstanciable = true;
    }   
    
    template<typename... Args>
    void AddPlacementNewFunc(const std::function<void(void*, Args...)>& InFunc)
    {
        PlacementNewFunc = std::make_unique<CPlacementNewFunc<Args...>>(InFunc);
        bIsInstanciable = true;
    }

    /**
     * Instantiate the struct
     */
    template<typename T, typename... Args>
    T* Instantiate(Args&&... InArgs) const
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
	void PlacementNew(void* InP, Args&&... InArgs) const
	{
		must(bIsInstanciable);

		CPlacementNewFunc<Args...>* Func =
			static_cast<CPlacementNewFunc<Args...>*>(PlacementNewFunc.get());
		Func->Func(InP, std::forward<Args>(InArgs)...);
	}

    void AddProperty(const CProperty& InProperty)
    {
        Properties.push_back(InProperty);
    }

    void AddParent(const std::string& InParent);
    bool IsDerivedFrom(const CStruct* InParent) const;

	const auto& GetParents() { return Parents; }
protected:
    std::unique_ptr<IInstantiateFunc> InstantiateFunc;
    std::unique_ptr<IInstantiateFunc> PlacementNewFunc;
    std::vector<CProperty> Properties;
    std::vector<TLazyTypePtr<CStruct>> Parents;
    bool bIsInstanciable;
};

REFLECTION_API void RegisterStruct(const CStruct* InStruct);
REFLECTION_API const CStruct* GetStructByName(const std::string& InName);
REFLECTION_API const std::vector<const CStruct*> GetStructs();

template<typename T>
ZE_FORCEINLINE const CStruct* GetStruct()
{
    return GetStructByName(TTypeName<T>::Name);
}

}