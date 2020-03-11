#pragma once

#include <chrono>
#include <string>
#include <map>
#include <vector>
#include <set>
#include <SDL2/SDL_timer.h>

/**
 * Stat system
 * Used to collect timing data and other
 */
enum class EStatGroupCategory
{
    Simulation,
    Rendering,
};

/**
 * A stat group
 */
class CStatGroup
{
public:
	CStatGroup(const std::string& InName,
		EStatGroupCategory InCategory);

    std::vector<class IStatBase*>& GetStats() { return Stats; }
    const std::vector<class IStatBase*>& GetStats() const { return Stats; }
    const std::string& GetName() const { return Name; }
    EStatGroupCategory GetCategory() const { return Category; }
private:
	std::string Name;
	EStatGroupCategory Category;
    std::vector<class IStatBase*> Stats;
};

#define DECLARE_STAT_GROUP(Name) extern CStatGroup StatGroup##Name;
#define DEFINE_STAT_GROUP(Name, Category) CStatGroup StatGroup##Name = CStatGroup(#Name, Category);

/**
 * Type of data stat contains
 */
enum class EStatDataType
{
    Uint32,
    Float
};

/** Dummy interface so CStatGroup can manage a list of stats */
class IStatBase 
{
public:
    virtual void Reset() = 0;
    virtual std::string GetID() const = 0;
    virtual void* GetData() const = 0;
    virtual EStatDataType GetDataType() const = 0;
    virtual bool ShouldReset() const = 0;
};

/**
 * Represents a stat
 */
template<typename T>
class TStat : public IStatBase
{
public:
    TStat(const std::string& InID,
        CStatGroup& InGroup,
        EStatDataType InDataType,
        bool bInShouldReset = false) : ID(InID), Group(InGroup), DataType(InDataType),
            bShouldReset(bInShouldReset)
    {
        Group.GetStats().push_back(this);
    }

    virtual std::string GetID() const override { return ID; }
    virtual void* GetData() const override { return reinterpret_cast<void*>(
        const_cast<T*>(&Data)); }
	const T& GetDataTemplate() const { return Data; }
    virtual EStatDataType GetDataType() const override { return DataType; }
    virtual bool ShouldReset() const override { return bShouldReset; }

    void SetData(const T& InData)
    {
        Data = InData;
    }

    /**
     * Increment stat
     */
    void Increment()
    {
        Data++;
    }

    void Reset() override
    {
        Data = 0;
    }
private:
    std::string ID;
    CStatGroup& Group;
    EStatDataType DataType;
    T Data;
    bool bShouldReset;
};

/**
 * A basic scoped timer that will set the stat one destroyed
 */
template<typename T>
class TScopedStatTimer
{
public:
	TScopedStatTimer::TScopedStatTimer(TStat<T>& InStat)
		: Stat(InStat)
	{
		Start = SDL_GetPerformanceCounter();
	}

	TScopedStatTimer::~TScopedStatTimer()
	{
		End = SDL_GetPerformanceCounter();
		Stat.SetData(((End - Start) * 1000 / (float) SDL_GetPerformanceFrequency()));
	}
private:
    TStat<T>& Stat;
    uint64_t Start;
    uint64_t End;
};

/** Macros to define a stat */
#define DEFINE_STAT(ID, DataType, Group, DataEnum, Reset) \
    TStat<DataType> Stat##ID = TStat<DataType>(#ID, StatGroup##Group, DataEnum, Reset)

/** Macros to declare a stat */
#define DECLARE_STAT(ID, DataType, Group, DataEnum, Reset) TStat<DataType> Stat##ID; \
    DEFINE_STAT(ID, DataType, Group, DataEnum, Reset)
#define DECLARE_COUNTER_STAT(Group, ID) DEFINE_STAT(ID, uint32_t, Group, EStatDataType::Uint32, true)
#define DECLARE_TIMER_STAT(Group, ID) DEFINE_STAT(ID, float, Group, EStatDataType::Float, false)

/** Reference a stat that is declared in another place */
#define DECLARE_STAT_EXTERN(ID, DataType) extern TStat<DataType> Stat##ID
#define DECLARE_COUNTER_STAT_EXTERN(ID) DECLARE_STAT_EXTERN(ID, uint32_t)
#define DECLARE_TIMER_STAT_EXTERN(ID) DECLARE_STAT_EXTERN(ID, float)

/** Macros to manipulate stats */
#define INCREMENT_STAT(ID) Stat##ID.Increment()
#define SCOPED_TIMER_STAT(ID) TScopedStatTimer<float> ScopedStatTimer##ID(Stat##ID)

/**
 * Stat manager class
 */
class CStatManager
{
public:
    static void PrintCategory(EStatGroupCategory InCategory);
    static void ResetStats(EStatGroupCategory InCategory);

    static std::set<CStatGroup*>* StatGroupSet;
};

/** Default groups */
DECLARE_STAT_GROUP(Rendering);
DECLARE_STAT_GROUP(GameThread);