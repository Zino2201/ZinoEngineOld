#pragma once

#include "serialization/Archive.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/reader.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/istreamwrapper.h>
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
	using JsonValue = rapidjson::GenericValue<rapidjson::UTF8<>>;
	using MemberIterator = JsonValue::ConstMemberIterator;
	using ValueIterator = JsonValue::ConstValueIterator;

	class Iterator
	{
		enum Type 
		{
			Null,
			Value,
			Member,
		};
	public:
		Iterator(MemberIterator in_beg, MemberIterator in_end)
			: begin(in_beg), end(in_end), index(0), type(Member)
		{
			if(std::distance(begin, end) == 0)
				type = Null;
		}

		Iterator(ValueIterator in_begin, ValueIterator in_end)
			: val_begin(in_begin), index(0), type(Value)
		{
			if(std::distance(val_begin, in_end) == 0)
				type = Null;
		}

		Iterator& operator++()
		{
			++index;
			return *this;
		}

		void jump_to(const std::string& in_name)
		{
			size_t idx = 0;
			for(auto it = begin; it != end; ++it, ++idx)
			{
				if(std::strncmp(in_name.c_str(), it->name.GetString(), std::strlen(in_name.c_str())) == 0)
				{
					index = idx;
					return;
				}
			}

			ZE_CHECKF(false, "Can't find {}!", in_name);
		}

		const rapidjson::Document::GenericValue& get_value()
		{
			switch(type)
			{
			case Value:
				return val_begin[index];
			case Member:
				return begin[index].value;
			default:
				ZE_CHECKF(false, "Null iterator!");
				return begin[0].value; // To remove warning
			}
		}

		const char* get_name() const
		{
			auto current_name = begin[index].name.GetString();

 			if(type == Member && (begin + index) != end)
			{
				current_name = begin[index].name.GetString();
				return begin[index].name.GetString();
			}

			return nullptr;
		}
	private:
		MemberIterator begin;
		MemberIterator end;
		ValueIterator val_begin;
		size_t index;
		Type type;
	};

	JsonInputArchive(std::istream& in_stream) : InputArchive(*this), stream(in_stream), current_member_count(0)
	{
		document.ParseStream(stream);
		it_stack.emplace_back(document.MemberBegin(), document.MemberEnd());
	}

	~JsonInputArchive()
	{
	}

	template<typename T>
		requires (std::is_same_v<T, long> && !std::is_same_v<T, int> && !std::is_same_v<T, int64_t>)
	void read(T& in_data) { jump_to(); in_data = it_stack.back().get_value().GetUint64(in_data); ++it_stack.back(); }

	template<typename T>
		requires (std::is_same_v<T, unsigned long> && !std::is_same_v<T, unsigned int> && !std::is_same_v<T, uint64_t>)
	void read(int64_t& in_data) { jump_to(); in_data = it_stack.back().get_value().GetInt64(); ++it_stack.back(); }

	void read(int8_t& in_data) { jump_to(); in_data = it_stack.back().get_value().GetInt(); ++it_stack.back(); }
	void read(int16_t& in_data) { jump_to(); in_data = it_stack.back().get_value().GetInt(); ++it_stack.back(); }
	void read(int32_t& in_data) { jump_to(); in_data = it_stack.back().get_value().GetInt(); ++it_stack.back(); }
	void read(uint8_t& in_data) { jump_to(); in_data = it_stack.back().get_value().GetUint(); ++it_stack.back(); }
	void read(uint16_t& in_data) { jump_to(); in_data = it_stack.back().get_value().GetUint(); ++it_stack.back(); }
	void read(uint32_t& in_data) { jump_to(); in_data = it_stack.back().get_value().GetUint(); ++it_stack.back(); }
	void read(std::string& in_data) { jump_to(); in_data = it_stack.back().get_value().GetString(); ++it_stack.back(); }
	void read(bool& in_data) { jump_to(); in_data = it_stack.back().get_value().GetBool(); ++it_stack.back(); }

	void start_object()
	{
		jump_to();

		if(it_stack.back().get_value().IsArray())
		{
			it_stack.emplace_back(it_stack.back().get_value().Begin(), it_stack.back().get_value().End());
		}
		else
		{
			current_member_count = it_stack.back().get_value().MemberCount();
			it_stack.emplace_back(it_stack.back().get_value().MemberBegin(), it_stack.back().get_value().MemberEnd());
		}
	}
	
	void end_object()
	{
		current_member_count = 0;
		it_stack.pop_back();
		++it_stack.back();
	}

	void set_next_name(const std::string& in_next_name)
	{
		next_name = in_next_name;
	}

	[[nodiscard]] const size_t get_current_member_count() const { return current_member_count; }
	[[nodiscard]] std::string get_current_name() const { return it_stack.back().get_name(); }
private:
	void jump_to()
	{
		if(!next_name.empty())
		{
			const char* cur_name = it_stack.back().get_name();

			if(!cur_name || std::strcmp(next_name.c_str(), cur_name) != 0)
				it_stack.back().jump_to(next_name);
		}

		next_name = "";
	}
private:
	std::vector<Iterator> it_stack;
	std::string next_name;
	rapidjson::IStreamWrapper stream;
	rapidjson::Document document;
	size_t current_member_count;
	std::string current_name;
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
	void write(bool in_bool) { writer.Bool(in_bool); }

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
inline void serialize(JsonInputArchive& archive, T& data)
{
	archive.read(data);
}

template<typename T>
	requires std::is_arithmetic_v<T>
inline void serialize(JsonOutputArchive& archive, const T& data)
{
	archive.write(data);
}

template<typename T>
	requires std::is_arithmetic_v<T>
inline void pre_serialize(JsonInputArchive& archive, const T& data) {}

template<typename T>
	requires std::is_arithmetic_v<T>
inline void pre_serialize(JsonOutputArchive& archive, const T& data) 
{
	archive.flush();
}

template<typename T>
	requires std::is_arithmetic_v<T>
inline void post_serialize(JsonInputArchive& archive, const T& data) {}

template<typename T>
	requires std::is_arithmetic_v<T>
inline void post_serialize(JsonOutputArchive& archive, const T& data) {}

/** Non arithmetic types */
template<typename T>
	requires (!std::is_arithmetic_v<T>)
inline void pre_serialize(JsonInputArchive& archive, const T& data)
{
	archive.start_object();
}

template<typename T>
	requires (!std::is_arithmetic_v<T>)
inline void pre_serialize(JsonOutputArchive& archive, const T& data)
{
	archive.start_object();
}

template<typename T>
	requires (!std::is_arithmetic_v<T>)
inline void post_serialize(JsonInputArchive& archive, const T& data)
{
	archive.end_object();
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
inline void pre_serialize(JsonInputArchive& archive, const Size<T>& data) {}

template<typename T>
inline void pre_serialize(JsonOutputArchive& archive, const Size<T>& data) 
{
	archive.mark_object_as_array();
}

template<typename T>
inline void serialize(JsonInputArchive& archive, Size<T>& data) {}

template<typename T>
inline void serialize(JsonOutputArchive& archive, const Size<T>& data) {}

template<typename T>
inline void post_serialize(JsonInputArchive& archive, const Size<T>& data) {}

template<typename T>
inline void post_serialize(JsonOutputArchive& archive, const Size<T>& data) {}

/** String */

inline void serialize(JsonInputArchive& archive, std::string& data)
{
	archive.read(data);
}

inline void serialize(JsonOutputArchive& archive, const std::string& data)
{
	archive.write(data);
}

inline void pre_serialize(JsonInputArchive& archive, const std::string& data) {}

inline void pre_serialize(JsonOutputArchive& archive, const std::string& data) 
{
	archive.flush();
}

inline void post_serialize(JsonInputArchive& archive, const std::string& data) {}
inline void post_serialize(JsonOutputArchive& archive, const std::string& data) {}

/**
 * Named data
 */

template<typename T>
inline void serialize(JsonInputArchive& archive, NamedData<T>& data)
{
	archive.set_next_name(data.name);
	T& rdata = data.data;
	archive <=> rdata;
}

template<typename T>
inline void serialize(JsonOutputArchive& archive, const NamedData<T>& data)
{
	archive.set_next_name(data.name);
	const T& cdata = data.data;
	archive <=> cdata;
}

template<typename T>
inline void pre_serialize(JsonInputArchive& archive, const NamedData<T>& data) {}

template<typename T>
inline void pre_serialize(JsonOutputArchive& archive, const NamedData<T>& data) {}

template<typename T>
inline void post_serialize(JsonInputArchive& archive, const NamedData<T>& data) {}

template<typename T>
inline void post_serialize(JsonOutputArchive& archive, const NamedData<T>& data) {}

/**
 * Pair
 */
template<typename T1, typename T2>
inline void serialize(JsonInputArchive& archive, std::pair<T1, T2>& data)
{
	archive <=> data.second;
}

template<typename T1, typename T2>
inline void serialize(JsonOutputArchive& archive, const std::pair<T1, T2>& data)
{
	archive.set_next_name(data.first);
	archive <=> data.second;
}

template<typename T1, typename T2>
inline void pre_serialize(JsonInputArchive& archive, const std::pair<T1, T2>& data) {}

template<typename T1, typename T2>
inline void pre_serialize(JsonOutputArchive& archive, const std::pair<T1, T2>& data) {}

template<typename T1, typename T2>
inline void post_serialize(JsonInputArchive& archive, const std::pair<T1, T2>& data) {}

template<typename T1, typename T2>
inline void post_serialize(JsonOutputArchive& archive, const std::pair<T1, T2>& data) {}

/** Hashmaps */
template<typename T1, typename T2>
inline void serialize(JsonInputArchive& archive, robin_hood::unordered_map<T1, T2>& data)
{

	for(int i = 0; i < archive.get_current_member_count(); ++i)
	{
		std::pair<T1, T2> pair;
		auto name = archive.get_current_name();
		archive <=> pair;
		data[name] = pair.second;
	}
}

template<typename T1, typename T2>
inline void serialize(JsonOutputArchive& archive, const robin_hood::unordered_map<T1, T2>& data)
{
	for(const auto& [key, val] : data)
		archive <=> std::make_pair(key, val);
}

/** Don't turn uuids into objects */
inline void pre_serialize(JsonInputArchive& archive, const uuids::uuid& data) {}
inline void pre_serialize(JsonOutputArchive& archive, const uuids::uuid& data) {}
inline void post_serialize(JsonInputArchive& archive, const uuids::uuid& data) {}
inline void post_serialize(JsonOutputArchive& archive, const uuids::uuid& data) {}

}