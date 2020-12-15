#pragma once
  
#include <vector>
#include "boost/dynamic_bitset/dynamic_bitset.hpp"
#include <iterator>  
#include "Assertions.h"

namespace ze
{
  
template<typename ElementType>
struct SparseArrayIterator
{
    using iterator_category = std::random_access_iterator_tag;
    using value_type = ElementType;
    using difference_type = std::ptrdiff_t;
    using pointer = ElementType*;
    using reference = ElementType&;
  
    SparseArrayIterator(std::vector<ElementType>& in_elements,
        boost::dynamic_bitset<uint8_t>& in_bitset, const size_t& in_current_idx) :
        current_idx(in_current_idx), 
        elements(in_elements), bitset(in_bitset) {}

    SparseArrayIterator(const SparseArrayIterator& other) : current_idx(other.current_idx),
        elements(other.elements), bitset(other.bitset) {}

    void operator=(const SparseArrayIterator& other)
    {
        current_idx = other.current_idx;
        elements = other.elements;
        bitset = other.bitset;
    }

    void operator=(SparseArrayIterator&& other)
    {
        current_idx = std::move(other.current_idx);
        elements = std::move(other.elements);
        bitset = std::move(other.bitset);
    }

    SparseArrayIterator operator+(const SparseArrayIterator& other)
    {
        return SparseArrayIterator(elements, bitset, current_idx + other.current_idx);
    }

    SparseArrayIterator operator+(const difference_type& other)
    {
        return SparseArrayIterator(elements, bitset, current_idx + other);
    }

    ZE_FORCEINLINE ElementType& operator*()
    {
        return elements[current_idx];
    }
  
    ZE_FORCEINLINE ElementType& operator*() const
    {
        return elements[current_idx];
    }

    SparseArrayIterator& operator++()
    {
        while (current_idx != bitset.size())
        {
            if(bitset[++current_idx])
                return *this;
        }

        return *this;
    }

    SparseArrayIterator& operator--()
    {
        while (current_idx != 0)
        {
            if(bitset[--current_idx])
                return *this;
        }

        return *this;
    }
  
	ZE_FORCEINLINE SparseArrayIterator& operator-(difference_type diff)
	{
		current_idx -= diff;
		ZE_CHECK(current_idx > 0 && current_idx <= elements.size() - 1);
	}

    ZE_FORCEINLINE bool operator<(const SparseArrayIterator& other) const
    {
        return current_idx < other.current_idx;
    }

    ZE_FORCEINLINE bool operator>(const SparseArrayIterator& other) const
    {
        return current_idx > other.current_idx;
    }

	ZE_FORCEINLINE friend difference_type operator-(const SparseArrayIterator& left, const SparseArrayIterator& right)
	{
		return right.current_idx - left.current_idx;
	}

	ZE_FORCEINLINE friend bool operator==(const SparseArrayIterator& left, const SparseArrayIterator& right)
	{
		return left.current_idx == right.current_idx;
	}

    ZE_FORCEINLINE friend bool operator!=(const SparseArrayIterator& left, const SparseArrayIterator& right)
    {
        return left.current_idx != right.current_idx;
    }
private:
    size_t current_idx;
    std::vector<ElementType>& elements;
    boost::dynamic_bitset<uint8_t>& bitset;
};

/**
 * A array that ensure that a object will always keep its indice even when the array is modified
 * Allows O(1) deletion but introduces holes in memory
 */
template<typename T, typename Container = std::vector<T>>
class SparseArray
{
public:
    using ElementType = T;
    using Iterator = SparseArrayIterator<T>;
    using ConstIterator = const SparseArrayIterator<const T>;
  
    SparseArray() = default;
    ~SparseArray() = default;
  
    SparseArray(const SparseArray& other) :
        elements(other.elements), 
        allocated_bitset(other.allocated_bitset) {}
  
    SparseArray(SparseArray&& other) :
        elements(std::move(other.elements)), 
        allocated_bitset(std::move(other.allocated_bitset)) {}
  
    size_t add(const T& element)
    {
        size_t idx = get_free_index_or_grow();
  
        if(idx >= elements.size())
            elements.emplace_back(element);
        else
            elements[idx] = element;

        allocated_bitset.set(idx, true);
  
        return idx;
    }
  
    size_t add(T&& element)
    {
        size_t idx = get_free_index_or_grow();
  
        if(idx >= elements.size())
            elements.emplace_back(std::move(element));
        else
            elements[idx] = std::move(element));
        
        allocated_bitset.set(idx, true);
  
        return idx;
    }
  
    template<typename... Args>
    size_t emplace(Args&&... InArgs)
    {
        size_t idx = get_free_index_or_grow();
        if(idx >= elements.size())
            elements.emplace_back(std::forward<Args>(InArgs)...);
        else
            elements[idx] = T(std::forward<Args>(InArgs)...);
        allocated_bitset.set(idx, true);
  
        return idx;
    }
  
    void remove(const size_t& index)
    {
        ZE_ASSERT(InIndex < Elements.size());
  
        allocated_bitset.set(index, false);
    }
  
    void reserve(const size_t& capacity)
    {
        elements.reserve(capacity);
        allocated_bitset.reserve(capacity);
    }
  
    ZE_FORCEINLINE T& at(const size_t& idx)
    {
        ZE_CHECK(idx < elements.size());
        ZE_CHECK(allocated_bitset[idx]);
  
        return elements[idx];
    }
  
    size_t get_size() const
    {
        return elements.size();
    }
  
    size_t get_capacity() const
    {
        return elements.capacity();
    }
  
    bool is_empty() const
    {
        return elements.empty();
    }
  
    Iterator begin()
    {
        return Iterator(elements, allocated_bitset, 0);
    }
  
    ConstIterator cbegin() const
    {
        return ConstIterator(elements, allocated_bitset, 0);
    }
  
    Iterator end()
    {
        return Iterator(elements, allocated_bitset, 
            elements.size() == 0 ? 0 : elements.size());
    }
  
    ConstIterator cend() const
    {
        return ConstIterator(elements, allocated_bitset, 
            elements.size() == 0 ? 0 : elements.size());
    }
  
    ZE_FORCEINLINE ElementType& operator[](const size_t& index)
    {
        return at(index);
    }
  
    ZE_FORCEINLINE const ElementType& operator[](const size_t& index) const
    {
        return at(index);
    }
  
    SparseArray& operator=(const SparseArray& other)
    {
        elements = other.elements;
        allocated_bitset = other.allocated_bitset;
        return *this;
    }

    const Container& get_container() const { return container; }
private:
    size_t get_free_index_or_grow()
    {
        for(size_t i = 0; i < allocated_bitset.size(); ++i)
        {
            if(!allocated_bitset[i])
                return i;
        }
  
        elements.reserve(elements.capacity() + 1);
        allocated_bitset.push_back(false);
  
        return elements.size();
    }
private:
    Container elements; 
    boost::dynamic_bitset<uint8_t> allocated_bitset;
};
  
};