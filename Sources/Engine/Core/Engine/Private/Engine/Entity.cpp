//#include "Engine/Entity.h"
//
//namespace ZE
//{
//
//
//REFL_INIT_FUNC()
//{
//	TClassBuilder<IEntityComponentSystem>("IEntityComponentSystem")
//		.End();
//
//	TStructBuilder<SEntityComponent>("SEntityComponent")
//		.End();
//}
//
//void CEntityComponentSystemManager::Initialize()
//{
//	CClass* tio = IEntityComponentSystem::GetClass();
//	auto& SystemsClasses = CObjectLists::GetObjectsForClass(IEntityComponentSystem::GetClass());
//	
//	std::vector<std::string> Classes;
//
//	for(const auto& Obj : SystemsClasses)
//		Classes.push_back(Obj->GetClass()->GetName());
//	
//	__debugbreak();
//}
//
//} /* namespace ZE */