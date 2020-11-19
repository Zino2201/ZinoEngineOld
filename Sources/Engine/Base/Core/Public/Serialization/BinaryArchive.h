#pragma once

#include "Archive.h"
#include <type_traits>

namespace ze::serialization
{

/**
 * Input binary archive
 */
class CORE_API BinaryInputArchive : public InputArchive<BinaryInputArchive>
{
public:
	BinaryInputArchive(std::istream& in_stream) : InputArchive<BinaryInputArchive>(*this),
		stream(in_stream) {}

	void load_bytes(void* data, const uint64_t& size);
private:
	std::istream& stream;
};

/**
 * Output binary archive
 */
class CORE_API BinaryOutputArchive : public OutputArchive<BinaryOutputArchive>
{
public:
	BinaryOutputArchive(std::ostream& in_stream) : OutputArchive<BinaryOutputArchive>(*this), 
		stream(in_stream) {}

	void save_bytes(const void* data, const uint64_t& size);
private:
	std::ostream& stream;
};

/** Serialize functions for binary archives */

/** Arithmetic & enum types */
template<typename T>
	requires std::is_arithmetic_v<T> || std::is_enum_v<T>
ZE_FORCEINLINE void serialize(BinaryInputArchive& archive, T& data)
{
	archive.load_bytes(&data, sizeof(T));
}

template<typename T>
	requires std::is_arithmetic_v<T> || std::is_enum_v<T>
ZE_FORCEINLINE void serialize(BinaryOutputArchive& archive, const T& data)
{
	archive.save_bytes(&data, sizeof(T));
}

/** Binary archives */
template<typename T>
ZE_FORCEINLINE void serialize(BinaryInputArchive& archive, BinaryData<T>& data)
{
	archive.load_bytes(data.data, data.size);
}

template<typename T>
ZE_FORCEINLINE void serialize(BinaryOutputArchive& archive, const BinaryData<T>& data)
{
	archive.save_bytes(data.data, data.size);
}

/** Containers size */
template<typename T>
ZE_FORCEINLINE void serialize(BinaryInputArchive& archive, Size<T>& data)
{
	archive <=> data.size;
}

template<typename T>
ZE_FORCEINLINE void serialize(BinaryOutputArchive& archive, const Size<T>& data)
{
	archive <=> data.size;
}

}