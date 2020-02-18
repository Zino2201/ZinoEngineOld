#pragma once

#include "Render/Shader.h"

/**
 * Basic shader map
 */

/**
 * Get a basic shader by name
 */
template<typename T>
T* GetBasicShaderByName(const std::string& InName)
{
	return static_cast<T*>(CBasicShaderClass::BasicShaderMap[InName].get());
}

/**
 * Basic shader type
 */
class CBasicShaderClass : public CShaderClass
{
public:
	CBasicShaderClass(const std::string& InName,
		const std::string& InFilename,
		EShaderStage InStage,
		InstantiateFunctionType InInstantiateFunction);

	static void InstantiateBasicShaders();
	static void DestroyBasicShaders();

	static std::map<std::string, CShaderPtr> BasicShaderMap;
	static std::map<std::string, CShaderClass*> BasicShaderClassesMap;
};

/**
 * Basic shader class instance
 */
class CBasicShader : public CShader
{
public:
	CBasicShader(CShaderClass* InClass);
	virtual ~CBasicShader();
};