#pragma once
  
 #include <vector>
 #include "boost/dynamic_bitset/dynamic_bitset.hpp"
  
 namespace ZE
 {
  
 template<typename ElementType>
 struct TCoherentArrayIterator
 {
     using iterator_category = std::contiguous_iterator_tag;
     using value_type = ElementType;
     using difference_type = std::ptrdiff_t;
     using pointer = ElementType*;
     using reference = ElementType&;
  
     TCoherentArrayIterator(std::vector<ElementType>& InElements,
         boost::dynamic_bitset<uint8_t>& InBitset, const size_t& InCurrentIdx) :
         CurrentIdx(InCurrentIdx), 
         Elements(InElements), Bitset(InBitset) {}
  
     ElementType& operator*()
     {
         return Elements[CurrentIdx];
     }
  
     TCoherentArrayIterator& operator++()
     {
         while (CurrentIdx != Bitset.size())
         {
             if(Bitset[++CurrentIdx])
                 return *this;
         }

         return *this;
     }

	 TCoherentArrayIterator& operator=(const TCoherentArrayIterator& InOther)
	 {
		 CurrentIdx = InOther.CurrentIdx;
		 Elements = InOther.Elements;
		 Bitset = InOther.Bitset;
		 return *this;
	 }
  
	 friend bool operator==(const TCoherentArrayIterator& Left, const TCoherentArrayIterator& Right)
	 {
		 return Left.CurrentIdx == Right.CurrentIdx;
	 }

     friend bool operator!=(const TCoherentArrayIterator& Left, const TCoherentArrayIterator& Right)
     {
         return Left.CurrentIdx != Right.CurrentIdx;
     }
 private:
     size_t CurrentIdx;
     std::vector<ElementType>& Elements;
     boost::dynamic_bitset<uint8_t>& Bitset;
 };
  
 template<typename T>
 class TCoherentArray
 {
 public:
     using ElementType = T;
     using Iterator = TCoherentArrayIterator<T>;
     using ConstIterator = const TCoherentArrayIterator<const T>;
  
     TCoherentArray() = default;
     ~TCoherentArray() = default;
  
     TCoherentArray(const TCoherentArray& InOther) :
         Elements(InOther.Elements), 
         AllocatedBitset(InOther.AllocatedBitset) {}
  
     TCoherentArray(TCoherentArray&& InOther) :
         Elements(std::move(InOther.Elements)), 
         AllocatedBitset(std::move(InOther.AllocatedBitset)) {}
  
     size_t Add(const T& InElement)
     {
         size_t Idx = GetFreeIndexOrGrow();
  
         Elements.insert(Elements.begin(), InElement);
         AllocatedBitset.set(Idx, true);
  
         return Idx;
     }
  
     size_t Add(T&& InElement)
     {
         size_t Idx = GetFreeIndexOrGrow();
  
         Elements.insert(Elements.begin(), std::move(InElement));
         AllocatedBitset.set(Idx, true);
  
         return Idx;
     }
  
     template<typename... Args>
     size_t Emplace(Args&&... InArgs)
     {
         size_t Idx = GetFreeIndexOrGrow();
  
         Elements.emplace(Elements.begin() + Idx, std::forward<Args>(InArgs)...);
         AllocatedBitset.set(Idx, true);
  
         return Idx;
     }
  
     void Remove(const size_t& InIndex)
     {
         must(InIndex < Elements.size());
  
         AllocatedBitset.set(InIndex, false);
     }
  
     void Reserve(const size_t& InNewCapacity)
     {
         Elements.reserve(InNewCapacity);
         AllocatedBitset.reserve(InNewCapacity);
     }
  
     T& At(const size_t& InIndex)
     {
         return operator[](InIndex);
     }
  
     size_t GetSize() const
     {
         return Elements.size();
     }
  
     size_t GetCapacity() const
     {
         return Elements.capacity();
     }
  
     bool IsEmpty() const
     {
         return Elements.empty();
     }
  
     Iterator begin()
     {
         return Iterator(Elements, AllocatedBitset, 0);
     }
  
     ConstIterator cbegin() const
     {
         return ConstIterator(Elements, AllocatedBitset, 0);
     }
  
     Iterator end()
     {
         return Iterator(Elements, AllocatedBitset, 
             Elements.size() == 0 ? 0 : Elements.size());
     }
  
     ConstIterator cend() const
     {
         return ConstIterator(Elements, AllocatedBitset, 
             Elements.size() == 0 ? 0 : Elements.size());
     }
  
     ElementType& operator[](const size_t& InIndex)
     {
         verify(InIndex < Elements.size());
         verify(AllocatedBitset[InIndex]);
  
         return Elements[InIndex];
     }
  
     const ElementType& operator[](const size_t& InIndex) const
     {
         verify(InIndex < Elements.size());
         verify(AllocatedBitset[InIndex]);
  
         return Elements[InIndex];
     }
  
     TCoherentArray& operator=(const TCoherentArray& InOther)
     {
         Elements = InOther.Elements;
         AllocatedBitset = InOther.AllocatedBitset;
         return *this;
     }
 private:
     size_t GetFreeIndexOrGrow()
     {
         for(size_t i = 0; i < AllocatedBitset.size(); ++i)
         {
             if(!AllocatedBitset[i])
                 return i;
         }
  
         Elements.reserve(Elements.capacity() + 1);
         AllocatedBitset.push_back(false);
  
         return Elements.size();
     }
 private:
     std::vector<ElementType> Elements; 
     boost::dynamic_bitset<uint8_t> AllocatedBitset;
 };
  
 };