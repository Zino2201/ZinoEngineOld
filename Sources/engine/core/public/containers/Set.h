#pragma once

#include <vector>

namespace ZE
{

/**
 * An index to a set
 */
struct SSetElementId
{
	uint64_t Index;

	SSetElementId() : Index(-1) {}
	SSetElementId(const uint64_t& InIndex) : Index(InIndex) {}
	
	operator uint64_t() const
	{
		return Index;
	}

	static SSetElementId& GetNull()
	{
		static SSetElementId Null;
		return Null;
	}
};

template<typename T>
struct SSetElement
{
	/** Index of the element in the vector */
	SSetElementId Index;

	/** Element ref */
	T Element;

	/** Hash of the element */
	uint64_t Hash;

	SSetElement() : Index(), Hash(0) {}

	SSetElement(const uint64_t& InIndex, const T& InElement,
		const uint64_t& InHash)
		: Index(InIndex), Element(InElement), Hash(InHash) {}

	bool operator==(const T& InOther) const
	{
		uint64_t test = std::hash<T>()(InOther);
		return Hash == std::hash<T>()(InOther);
	}

	static SSetElement& GetNull()
	{
		static SSetElement Null;
		return Null;
	}
};

/**
 * A hash-set
 */
template<typename T>
class TSet
{
public:
	SSetElementId Add(const T& InElement)
	{
		return Emplace(InElement);
	}

	void Remove(const T& InElement)
	{
		uint64_t Idx = 0;

		for (auto& Element : Elements)
		{
			if (Element.Hash == std::hash<T>()(InElement))
			{
				Elements.erase(Elements.begin() + Idx);
				break;
			}

			Idx++;
		}

		/** Shift indices */
		Idx = 0;
		for (auto& Element : Elements)
		{
			Element.Index = Idx;
			Idx++;
		}
	}

	void Empty()
	{
		Elements.clear();
	}

	T& operator[](const SSetElementId& InIndex)
	{
		return GetElementByIndex(InIndex).Element;
	}

	const T& operator[](const SSetElementId& InIndex) const
	{
		return GetElementByIndex(InIndex).Element;
	}

	T& operator[](const T& InElement)
	{
		return GetElementByValue(InElement).Element;
	}

	SSetElementId operator[](T& InElement)
	{
		return GetElementByValue(InElement).Index;
	}

	SSetElementId operator[](const T& InElement) const
	{
		return GetElementByValue(InElement).Index;
	}

	bool IsEmpty() const
	{
		return Elements.empty();
	}

	bool Contains(const T& InElement) const
	{
		for (auto& Element : Elements)
		{
			if (Element.Hash == std::hash<T>()(InElement))
			{
				return true;
			}
		}

		return false;
	}

	uint64_t GetCount() const
	{
		return Elements.size();
	}
public:
	SSetElementId Emplace(const T& InElement)
	{
		/** Search if the element is already inside */
		for(const auto& Element : Elements)
		{
			if(Element == InElement)
			{
				return Element.Index;
			}
		}

		/** Not found any element, insert it */
		Elements.emplace_back(Elements.size(), InElement, std::hash<T>()(InElement));

		return Elements.back().Index;
	}

	SSetElement<T>& GetElementByIndex(const SSetElementId& InIndex)
	{
		for (auto& Element : Elements)
		{
			if (Element.Index == InIndex)
			{
				return Element;
			}
		}

		return SSetElement<T>::GetNull();
	}

	const SSetElement<T>& GetElementByIndex(const SSetElementId& InIndex) const
	{
		for (const auto& Element : Elements)
		{
			if (Element.Index == InIndex)
			{
				return Element;
			}
		}

		return SSetElement<T>::GetNull();
	}

	SSetElement<T>& GetElementByValue(const T& InValue)
	{
		for (auto& Element : Elements)
		{
			if (Element.Hash == std::hash<T>()(InValue))
			{
				return Element;
			}
		}

		return SSetElement<T>::GetNull();
	}

	const SSetElement<T>& GetElementByValue(const T& InValue) const
	{
		for (const auto& Element : Elements)
		{
			if (Element.Hash == std::hash<T>()(InValue))
			{
				return Element;
			}
		}

		return SSetElement<T>::GetNull();
	}
private:
    std::vector<SSetElement<T>> Elements;
};

} /* namespace ZE */