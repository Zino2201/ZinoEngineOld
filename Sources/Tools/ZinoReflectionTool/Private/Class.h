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
		CStruct(InName, InNamespace, InDeclLine), bIsAbstract(false) 
		{
			if(InName.starts_with("I"))
				bIsAbstract = true;
		}

	bool IsAbstract() const { return bIsAbstract; }
private:
	bool bIsAbstract;
};