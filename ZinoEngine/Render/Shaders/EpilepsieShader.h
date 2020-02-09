#pragma once

#include "BasicShader.h"

/**
 * Epilepsie shader VS
 */
class CEpilepsieShaderVS : public CBasicShader
{
    DECLARE_SHADER(CEpilepsieShaderVS, CBasicShaderClass)

    CEpilepsieShaderVS(CShaderClass* InClass);
};

/**
 * Epilepsie shader FS
 */
class CEpilepsieShaderFS : public CBasicShader
{
    DECLARE_SHADER(CEpilepsieShaderFS, CBasicShaderClass)

    CEpilepsieShaderFS(CShaderClass* InClass);
};