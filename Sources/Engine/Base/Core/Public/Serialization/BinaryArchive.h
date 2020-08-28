#pragma once

#include "Archive.h"

namespace ZE::Serialization
{

/**
 * Input binary archive
 */
class ENGINECORE_API CIBinaryArchive : public CInputArchive
{
public:
	CIBinaryArchive(std::istream& InStream) : Stream(InStream) {}

	void Load(void* InData, const uint64_t& InSize)
	{
		Stream.read(reinterpret_cast<char*>(InData), InSize);
	}
private:
	std::istream& Stream;
};

/**
 * Output binary archive
 */
class ENGINECORE_API COBinaryArchive : public COutputArchive
{
public:
	COBinaryArchive(std::ostream& InStream) : Stream(InStream) {}

	void Save(const void* InData, const uint64_t& InSize)
	{
		Stream.write(reinterpret_cast<const char*>(InData), InSize);
	}
private:
	std::ostream& Stream;
};

}