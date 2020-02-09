#pragma once

#include "Render/Shader.h"

/**
 * Basic shader map
 */
extern std::map<std::string, CShader*> g_BasicShaderMap;

/**
 * Get a basic shader by name
 */
template<typename T>
T* GetBasicShaderByName(const std::string& InName)
{
	return static_cast<T*>(g_BasicShaderMap[InName]);
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