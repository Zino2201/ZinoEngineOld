#include "Serialization/BinaryArchive.h"
#include <istream>
#include <ostream>

namespace ZE::Serialization
{

void CIBinaryArchive::LoadBytes(void* InData, const uint64_t& InSize)
{
	Stream.read(reinterpret_cast<char*>(InData), InSize);
}

void COBinaryArchive::SaveBytes(const void* InData, const uint64_t& InSize)
{
	Stream.write(reinterpret_cast<const char*>(InData), InSize);
}

}