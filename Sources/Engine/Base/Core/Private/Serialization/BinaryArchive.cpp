#include "Serialization/BinaryArchive.h"
#include <istream>
#include <ostream>

namespace ze::serialization
{

void BinaryInputArchive::load_bytes(void* data, const uint64_t& size)
{
	stream.read(reinterpret_cast<char*>(data), size);
}

void BinaryOutputArchive::save_bytes(const void* data, const uint64_t& size)
{
	stream.write(reinterpret_cast<const char*>(data), size);
}

}