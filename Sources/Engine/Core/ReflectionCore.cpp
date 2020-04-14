#include "Reflection/ReflectionCore.h"
#include "Object/Object.h"

ENGINECORE_API std::vector<CType*> CType::Types;
ENGINECORE_API std::unordered_map<CClass*, std::unordered_set<CClass*>> CClass::ClassChildsMap;