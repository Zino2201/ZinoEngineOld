#pragma once

#include "Serialization/Archive.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/ostreamwrapper.h>
#include <ostream>
#include <stack>
#include <uuid.h>
#include <robin_hood.h>

namespace ze::serialization
{

/**
 * Base class for json archives
 */
class JsonArchive {};

class JsonInputArchive : public JsonArchive,
	public InputArchive<JsonInputArchive>,
	public traits::TextArchive
{
public:
	rapidjson::Document& get_document() { return document; }
private:
	rapidjson::Document document;
};

class JsonOutputArchive : public JsonArchive,
	public OutputArchive<JsonOutputArchive>,
	public traits::TextArchive
{
public:
	enum class ObjectType
	{
		None,
		Root,
		Object,
		Array,

		/** We are inside an array */
		InsideArray,
		InsideObject
	};

	JsonOutputArchive(std::ostream& in_stream) : OutputArchive(*this), stream(in_stream), writer(stream)
	{
		writer.StartObject();
		object_stack.push(ObjectType::Root);
		object_stack.push(ObjectType::None);
		name_counter.push(0);
	}

	~JsonOutputArchive()
	{
		end_object();
		writer.EndObject();
	}

	template<typename T>
		requires (std::is_same_v<T, long> && !std::is_same_v<T, int> && !std::is_same_v<T, int64_t>)
	void write(T in_data) { writer.Uint64(in_data); }
	
	template<typename T>
		requires (std::is_same_v<T, unsigned long> && !std::is_same_v<T, unsigned int> && !std::is_same_v<T, uint64_t>)
	void write(int64_t in_data) { writer.Int64(in_data); }

	void write(int in_data) { writer.Int(in_data); }
	void write(unsigned int in_data) { writer.Uint(in_data); }
	void write(const std::string& in_data) { writer.String(in_data.c_str()); }

	auto& get_writer() { return writer; }

	/** Json manip */
	void set_next_name(const std::string& in_name) { next_name = in_name; }
	
	void start_object()
	{
		flush();
		object_stack.push(ObjectType::Object);
		name_counter.push(0);
	}

	void mark_object_as_array()
	{
		object_stack.top() = ObjectType::Array;
	}

	void end_object()
	{
		if(object_stack.top() == ObjectType::InsideArray)
		{
			writer.EndArray();
		}
		else if(object_stack.top() == ObjectType::InsideObject || object_stack.top() == ObjectType::Root)
		{
			writer.EndObject();
		}
		object_stack.pop();
		name_counter.pop();
	}

	/** 
	 * Flush object/array state and name
	 */
	void flush()
	{
		auto& object = object_stack.top();

		if(!object_stack.empty())
		{
			if(object == ObjectType::Object)
			{
				writer.StartObject();
				object = ObjectType::InsideObject;
			}
			else if(object == ObjectType::Array)
			{
				writer.StartArray();
				object = ObjectType::InsideArray;
			}
		}

		if(object != ObjectType::InsideArray && object != ObjectType::Root)
		{
			if(next_name.empty())
			{
				write(std::to_string(name_counter.top()++));
			}
			else
			{
				write(next_name);
				next_name.clear();
			}
		}
	}
private:
	std::stack<ObjectType> object_stack;
	std::stack<uint32_t> name_counter;
	std::string next_name;
	rapidjson::OStreamWrapper stream;
	rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer;
};

/**
 * Arithmetics types
 */
template<typename T>
	requires std::is_arithmetic_v<T>
inline void serialize(JsonOutputArchive& archive, const T& data)
{
	archive.write(data);
}

template<typename T>
	requires std::is_arithmetic_v<T>
inline void pre_serialize(JsonOutputArchive& archive, const T& data) 
{
	archive.flush();
}

template<typename T>
	requires std::is_arithmetic_v<T>
inline void post_serialize(JsonOutputArchive& archive, const T& data) {}

/** Non arithmetic types */
template<typename T>
	requires (!std::is_arithmetic_v<T>)
inline void pre_serialize(JsonOutputArchive& archive, const T& data)
{
	archive.start_object();
}

template<typename T>
	requires (!std::is_arithmetic_v<T>)
inline void post_serialize(JsonOutputArchive& archive, const T& data)
{
	archive.end_object();
}

/**
 * Start an array if there is a Size<T>
 */
template<typename T>
inline void pre_serialize(JsonOutputArchive& archive, const Size<T>& data) 
{
	archive.mark_object_as_array();
}

template<typename T>
inline void serialize(JsonOutputArchive& archive, const Size<T>& data) {}

template<typename T>
inline void post_serialize(JsonOutputArchive& archive, const Size<T>& data) {}

/** String */
inline void serialize(JsonOutputArchive& archive, const std::string& data)
{
	archive.write(data);
}

inline void pre_serialize(JsonOutputArchive& archive, const std::string& data) 
{
	archive.flush();
}

inline void post_serialize(JsonOutputArchive& archive, const std::string& data) {}

/**
 * Named data
 */
template<typename T>
inline void serialize(JsonOutputArchive& archive, const NamedData<T>& data)
{
	archive.set_next_name(data.name);
	const T& cdata = data.data;
	archive <=> cdata;
}

template<typename T>
inline void pre_serialize(JsonOutputArchive& archive, const NamedData<T>& data) {}

template<typename T>
inline void post_serialize(JsonOutputArchive& archive, const NamedData<T>& data) {}

/**
 * Pair
 */
template<typename T1, typename T2>
inline void serialize(JsonOutputArchive& archive, const std::pair<T1, T2>& data)
{
	archive.set_next_name(data.first);
	const T2& cdata = data.second;
	archive <=> data.second;
}

template<typename T1, typename T2>
inline void pre_serialize(JsonOutputArchive& archive, const std::pair<T1, T2>& data) {}

template<typename T1, typename T2>
inline void post_serialize(JsonOutputArchive& archive, const std::pair<T1, T2>& data) {}

/** Don't turn uuids into objects */
inline void pre_serialize(JsonOutputArchive& archive, const uuids::uuid& data) {}
inline void post_serialize(JsonOutputArchive& archive, const uuids::uuid& data) {}

}