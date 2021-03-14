#pragma once
  
#include <vector>
#include "boost/dynamic_bitset/dynamic_bitset.hpp"
#include <iterator>  

namespace ze
{

/**
 * A non-contigous array that guarantee element's positions to be fixed
 */
template<typename T>
class CoherentArray
{
public:
    template<typename U>
    class CoherentArrayIterator
    {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = U;
        using difference_type = std::ptrdiff_t;
        using pointer = U*;
        using reference = U&;

        CoherentArrayIterator(CoherentArray<T>& in_array,
            const size_t& in_current_idx) :
            current_idx(in_current_idx), 
            array(in_array) {}
  
        U& operator*()
        {
            return array[current_idx];
        }
  
        CoherentArrayIterator& operator++()
        {
            while (current_idx != array.get_capacity())
            {
                if(array.bitset[++current_idx])
                    return *this;
            }

            return *this;
        }

        CoherentArrayIterator& operator=(const CoherentArrayIterator& other)
	    {
		    array = other.array;
            current_idx = other.current_idx;
		    return *this;
	    }
  
	    CoherentArrayIterator& operator-(difference_type in_diff)
	    {
		    current_idx -= in_diff;
		    ZE_CHECK(current_idx > 0 && current_idx <= array.get_capacity() - 1);
	    }

	    friend difference_type operator-(const CoherentArrayIterator& left, const CoherentArrayIterator& right)
	    {
		    return right - left;
	    }

	    friend bool operator==(const CoherentArrayIterator& left, const CoherentArrayIterator& right)
	    {
		    return left.current_idx == right.current_idx;
	    }

        friend bool operator!=(const CoherentArrayIterator& left, const CoherentArrayIterator& right)
        {
            return left.current_idx != right.current_idx;
        }
    private:
        size_t current_idx;
        CoherentArray<T>& array;
    };

    struct ElementsDeleter
    {
        void operator()(T* ptr) const
        {
            free(ptr);
        }
    };

    using ElementType = T;
    using Iterator = CoherentArrayIterator<T>;
    using ConstIterator = const CoherentArrayIterator<const T>;
  
    CoherentArray() : size(0), capacity(0) {}
    ~CoherentArray() = default;
  
    CoherentArray(const CoherentArray& in_other)
    {
        elements = std::unique_ptr<T, ElementsDeleter>(reinterpret_cast<T*>(malloc(in_other.capacity * sizeof(T))));
        size = in_other.size;
        capacity = in_other.capacity;
        bitset = in_other.bitset;
        
        memcpy(elements, in_other.elements, in_other.capacity * sizeof(T));
    }
  
    CoherentArray(CoherentArray&& in_other)
        : elements(std::move(in_other.elements)), size(std::move(in_other.size)),
        capacity(std::move(in_other.capacity)), bitset(std::move(in_other.bitset)) {}

    size_t add(const T& in_element)
    {
        size_t idx = get_free_index_or_grow();
        ZE_ASSERTF(!bitset[Idx], "Index {} already contains a element!", idx); 
  
        new (elements.get() + idx) T(in_element);
        bitset.set(idx, true);
  
        size++;
        
        return idx;
    }
  
    size_t add(T&& in_element)
    {
        size_t idx = get_free_index_or_grow();
        ZE_ASSERTF(!bitset[idx], "Index {} already contains a element!", idx); 
  
        new (elements.get() + idx) T(std::move(in_element));
        bitset.set(idx, true);
  
        size++;

        return idx;
    }
  
    template<typename... Args>
    size_t emplace(Args... in_args)
    {
        size_t idx = get_free_index_or_grow();
        ZE_ASSERTF(!bitset[idx], "Index {} already contains a element!", idx); 
  
        new (elements.get() + idx) T(std::forward<Args>(in_args)...);
        bitset.set(idx, true);

        size++;
  
        return idx;
    }
  
    void remove(const size_t& in_index)
    {
        ZE_ASSERT(is_valid(in_index));
  
        at(in_index).~T();
        bitset.set(in_index, false);

        size--;
    }
  
    void reserve(const size_t& in_new_capacity)
    {
        if(in_new_capacity > capacity)
            realloc(in_new_capacity);
    }
  
    ZE_FORCEINLINE T& at(const size_t& in_index)
    {
        ZE_ASSERT(is_valid(in_index));
        return *(elements.get() + in_index);
    }

    ZE_FORCEINLINE const T& at(const size_t& in_index) const
    {
        ZE_ASSERT(is_valid(in_index));
        return *(elements.get() + in_index);
    }
  
    ZE_FORCEINLINE size_t get_size() const
    {
        return size;
    }
  
    ZE_FORCEINLINE size_t get_capacity() const
    {
        return capacity;
    }

    ZE_FORCEINLINE bool is_valid(const size_t& in_index) const
    {
        return in_index < capacity && bitset[in_index];
    }
  
    ZE_FORCEINLINE bool is_empty() const
    {
        return bitset.none();
    }
  
    Iterator begin()
    {
        return Iterator(*this, 0);
    }
  
    ConstIterator cbegin() const
    {
        return ConstIterator(*this, 0);
    }
  
    Iterator end()
    {
        return Iterator(*this, size);
    }
  
    ConstIterator cend() const
    {
        return ConstIterator(*this, size);
    }
  
    ElementType& operator[](const size_t& in_index)
    {
        return at(in_index);
    }
  
    const ElementType& operator[](const size_t& in_index) const
    {
         return at(in_index);
    }
  
    CoherentArray& operator=(const CoherentArray& in_other)
    {
        elements = std::unique_ptr<T, ElementsDeleter>(reinterpret_cast<T*>(malloc(in_other.capacity * sizeof(T)));
        size = in_other.size;
        capacity = in_other.capacity;
        bitset = in_other.bitset;
        
        memcpy(elements, in_other.elements, in_other.capacity * sizeof(T));
        
        return *this;
    }
private:
    void realloc(const size_t& in_new_capacity)
    {
        if(size > 0)
        {
            auto old_elements = std::move(elements);
            elements = std::unique_ptr<T, ElementsDeleter>(reinterpret_cast<T*>(malloc(in_new_capacity * sizeof(T))));
            memmove(elements.get(), old_elements.get(), capacity * sizeof(T));
        }
        else
        {
            elements = std::unique_ptr<T, ElementsDeleter>(reinterpret_cast<T*>(malloc(in_new_capacity * sizeof(T))));
        }

        bitset.resize(in_new_capacity);
        capacity = in_new_capacity;
    }

    size_t get_free_index_or_grow()
    {
        for(size_t i = 0; i < bitset.size(); ++i)
        {
            if(!bitset[i])
                return i;
        }

        /** At this point, we need to grow the elements array */
        realloc(++capacity);
  
        return capacity - 1;
    }
private:
    std::unique_ptr<T, ElementsDeleter> elements;
    size_t size;
    size_t capacity;
    boost::dynamic_bitset<uint8_t> bitset;
};
  
};