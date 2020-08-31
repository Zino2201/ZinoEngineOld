#pragma once

#include "Archive.h"

namespace ZE::Serialization
{

/**
 * Input binary archive
 */
class CORE_API CIBinaryArchive : public TInputArchive<CIBinaryArchive>
{
public:
	CIBinaryArchive(std::istream& InStream) : TInputArchive<CIBinaryArchive>(*this),
		Stream(InStream) {}

	void LoadBytes(void* InData, const uint64_t& InSize);
private:
	std::istream& Stream;
};

/**
 * Output binary archive
 */
class CORE_API COBinaryArchive : public TOutputArchive<COBinaryArchive>
{
public:
	COBinaryArchive(std::ostream& InStream) : TOutputArchive<COBinaryArchive>(*this), 
		Stream(InStream) {}

	void SaveBytes(const void* InData, const uint64_t& InSize);
private:
	std::ostream& Stream;
};

/** Serialize functions for binary archives */

/** Arithmetic types */
template<typename T>
	requires std::is_arithmetic_v<T>
ZE_FORCEINLINE void Serialize(CIBinaryArchive& InArchive, T& InData)
{
	InArchive.LoadBytes(&InData, sizeof(T));
}

template<typename T>
	requires std::is_arithmetic_v<T>
ZE_FORCEINLINE void Serialize(COBinaryArchive& InArchive, const T& InData)
{
	InArchive.SaveBytes(&InData, sizeof(T));
}

/** Binary archives */
template<typename T>
ZE_FORCEINLINE void Serialize(CIBinaryArchive& InArchive, TBinaryData<T>& InData)
{
	InArchive.LoadBytes(InData.Data, InData.Size);
}

template<typename T>
ZE_FORCEINLINE void Serialize(COBinaryArchive& InArchive, const TBinaryData<T>& InData)
{
	InArchive.SaveBytes(InData.Data, InData.Size);
}

/** Containers size */
template<typename T>
ZE_FORCEINLINE void Serialize(CIBinaryArchive& InArchive, TSize<T>& InData)
{
	InArchive <=> InData.Size;
}

template<typename T>
ZE_FORCEINLINE void Serialize(COBinaryArchive& InArchive, const TSize<T>& InData)
{
	InArchive <=> InData.Size;
}

}