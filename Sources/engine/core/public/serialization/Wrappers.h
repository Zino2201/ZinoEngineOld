#pragma once

namespace ze::serialization
{

/** Wrapper for a size, used to omit size for special archives (e.g JSON) */
template<typename T>
struct Size
{
	T size;
	
	Size(T&& in_size) : size(std::forward<T>(in_size)) {}
};

template<typename T>
ZE_FORCEINLINE Size<T> make_size(T&& in_size)
{
	return Size<T>(std::forward<T>(in_size));
}

/** Wrapper for contiguous binary data */
template<typename T>
struct BinaryData
{
	void* data;
	uint64_t size;

	BinaryData(T* in_data, const uint64_t& in_size) : data(in_data), size(in_size) {}

	operator bool() const { return !!data;  }
};

template<typename T>
ZE_FORCEINLINE BinaryData<T> make_binary_data(T* data, const uint64_t& size)
{
	return BinaryData<T>(data, size);
}

/**
 * Data with a name, used for json/xml serializations
 */
template<typename T>
struct NamedData
{
	std::string name;
	T& data;
	
	NamedData(const std::string& in_name, T& in_data) : name(in_name), data(in_data) {}
};

template<typename T>
ZE_FORCEINLINE NamedData<T> make_named_data(const std::string& in_view, T& in_data)
{
	return NamedData<T>(in_view, in_data);
}

}