#pragma once

#include "Struct.h"

/**
 * A C++ class
 */
class CClass : public CStruct
{
public:
	CClass(const std::string& InName, const std::string& InNamespace,
		const size_t& InDeclLine) : 
		CStruct(InName, InNamespace, InDeclLine) {}
};