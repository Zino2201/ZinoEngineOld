#include "StatSystem.h"
#include "Core/EngineCore.h"
#include <iostream>
#include <iomanip>

std::set<CStatGroup*>* CStatManager::StatGroupSet;

CStatGroup::CStatGroup(const std::string& InName,
	EStatGroupCategory InCategory) : Name(InName), Category(InCategory) 
{
	if(!CStatManager::StatGroupSet)
		CStatManager::StatGroupSet = new std::set<CStatGroup*>;

	CStatManager::StatGroupSet->insert(this);
}

DEFINE_STAT_GROUP(Rendering, EStatGroupCategory::Rendering);
DEFINE_STAT_GROUP(GameThread, EStatGroupCategory::Simulation);

void PrintStats(const CStatGroup& InGroup)
{
	LOG(ELogSeverity::Info, "Stat Group: %s", InGroup.GetName().c_str());
	using std::setw;
	std::cout << std::left;
	std::cout <<
		setw(30) << "Name" << setw(7) << "Current" << std::endl;
	for (IStatBase* Stat : InGroup.GetStats())
	{
		std::string Name = Stat->GetID();

		switch (Stat->GetDataType())
		{
		case EStatDataType::Float:
		{
			float* FltPtr = reinterpret_cast<float*>(Stat->GetData());
			std::cout <<
				setw(30) << Name << setw(7) << 
				*FltPtr << " ms" << std::endl;
			break;
		}
		case EStatDataType::Uint32:
		{
			uint32_t* UintPtr = reinterpret_cast<uint32_t*>(Stat->GetData());
			std::cout <<
				setw(30) << Name << setw(7) << 
				*UintPtr << std::endl;
			break;
		}
		}
	}
}

void CStatManager::PrintCategory(EStatGroupCategory InCategory)
{
	for (const auto& Group : *CStatManager::StatGroupSet)
	{
		if (Group->GetCategory() == InCategory)
			PrintStats(*Group);
	}
}

void CStatManager::ResetStats(EStatGroupCategory InCategory)
{
	for (const auto& Group : *CStatManager::StatGroupSet)
	{
		if (Group->GetCategory() == InCategory)
		{
			for (IStatBase* Stat : Group->GetStats())
			{
				if(Stat->ShouldReset())
					Stat->Reset();
			}
		}
	}
}