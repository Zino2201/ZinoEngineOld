#pragma once

#include <memory>
#include <array>

template<typename ChunkType, typename BlockType, typename DataType>
struct TPoolIterator
{
	using iterator_category = std::forward_iterator_tag;
	using value_type = DataType;
	using difference_type = std::ptrdiff_t;
	using pointer = DataType*;
	using reference = DataType&;

    TPoolIterator(ChunkType* InChunk, ChunkType* InLastChunk, BlockType* InBlock,
        BlockType* InEnd) : 
        Chunk(InChunk), LastChunk(InLastChunk), Block(InBlock), End(InEnd)
    {
    }

    DataType& operator*() const
    {
        return Block->Data;
    }

    TPoolIterator& operator++()
    {
		while (true)
		{
            if(Block && Block != End)
            {
			    Block = Block->Next;

                if(Block && Block->bAllocated)
                    return *this;
            }           
            else
            {
                return *this;
            }
		}
    }

    ChunkType* Chunk;
    ChunkType* LastChunk;
    BlockType* Block;
    BlockType* End;

	friend bool operator!=(const TPoolIterator& Left, const TPoolIterator& Right)
	{
		return Left.Block != Right.Block;
	}
};

/**
 * Simple object pool implementation
 */
template<typename T, uint32_t BlockPerChunk = 100>
struct TPool
{
    struct SBlock
    {
        SBlock* Next;
        bool bAllocated;
        T Data;

        SBlock() : Next(nullptr), bAllocated(false), Data() {}
    };

    struct SChunk
    {
        std::unique_ptr<SChunk> Previous;
        std::array<SBlock, BlockPerChunk> Blocks;

		SChunk()
		{
			for (size_t i = 0; i < BlockPerChunk; ++i)
			{
				SBlock* Block = GetBlockAt(i);

				if (i + 1 == BlockPerChunk)
					Block->Next = nullptr;
				else
					Block->Next = GetBlockAt(i + 1);
			}
		}

		SBlock* GetBlockAt(const size_t& InIdx)
		{
			return reinterpret_cast<SBlock*>(Blocks.data() + InIdx);
		}
    };

    using Iterator = TPoolIterator<SChunk, SBlock, T>;
    using ConstIterator = TPoolIterator<const SChunk, const SBlock, const T>;

	TPool() : FreeBlock(nullptr), LastChunk(nullptr)
	{
		CurrentChunk = std::make_unique<SChunk>();
		FreeBlock = CurrentChunk->GetBlockAt(0);
        LastChunk = CurrentChunk.get();
        FirstChunk = CurrentChunk.get();
	}

    /** Copy not supported for now */
    template<typename U>
    TPool(const TPool<U>& InOther) = delete;

    template<typename U>
    void operator=(const TPool<U>& InOther) = delete;

    /**
     * Iterators
     */
    Iterator begin()
    {
        return Iterator(FirstChunk, LastChunk, FirstChunk->Blocks.data(),
            LastChunk->Blocks.data() + (BlockPerChunk - 1));
    }

	ConstIterator cbegin() const
	{
		return ConstIterator(FirstChunk, LastChunk,  LastChunk->Blocks.data() + (BlockPerChunk - 1),
            LastChunk->Blocks.data() + (BlockPerChunk - 1));
	}

    Iterator end()
    {
        return Iterator(FirstChunk, LastChunk, LastChunk->Blocks.data() + (BlockPerChunk - 1),
            LastChunk->Blocks.data() + (BlockPerChunk - 1));
    }

	ConstIterator cend() const
	{
		return ConstIterator(FirstChunk, LastChunk, FirstChunk->Blocks.data(),
            LastChunk->Blocks.data() + (BlockPerChunk - 1));
	}

    /**
     * Allocate without calling ctor
     */
	T* Allocate()
	{
		/** Check if chunk is full */
		if (!FreeBlock)
		{
			/** Create a new chunk */
            /** link last block to the new */
			TOwnerPtr<SChunk> Chunk = new SChunk();
            CurrentChunk->GetBlockAt(BlockPerChunk - 1)->Next = Chunk->Blocks.data();
			Chunk->Previous = std::move(CurrentChunk);
            LastChunk = Chunk;
			CurrentChunk.reset(Chunk);
			FreeBlock = CurrentChunk->GetBlockAt(0);
		}

		/** "Allocate" a block */
		SBlock* Block = FreeBlock;
        Block->bAllocated = true;
		FreeBlock = Block->Next;

		T* Data = reinterpret_cast<T*>(&Block->Data);

		return Data;
	}

    template<typename... Args>
    T& Allocate(Args&&... InArgs)
    {
        T* Data = Allocate();
        new (Data) T(std::forward<Args>(InArgs)...);

        return *Data;
    }

    void Free(T& InElem)
    {
        /** Call the dtor, then mark the block as free */
		InElem.T::~T();

		SBlock* Block = reinterpret_cast<SBlock*>(std::addressof(InElem)) - 1;
    	Block->Next = FreeBlock;
        Block->bAllocated = false;
		FreeBlock = Block;
    }

private:
    std::unique_ptr<SChunk> CurrentChunk;
    SChunk* FirstChunk;
    SChunk* LastChunk;
    SBlock* FreeBlock;
};

/**
 * Simple object pool implementation using linked lists allowing different data size than T
 * but uses heap
 * A pool contains multiple chunks, each chunk contain a fixed number of blocks
 */
template<typename T>
struct TDynamicPool
{
    struct SBlock
    {
        SBlock* Next;
        char* Data;

        SBlock() : Next(nullptr), Data(nullptr) {}
    };

    struct SChunk
    {
        std::unique_ptr<SChunk> Previous;
        std::unique_ptr<char[]> Blocks;
        size_t BlockSize;

        SChunk(const size_t& InChunkSize,
            const size_t& InBlockSize) : Previous(nullptr), 
            Blocks(new char[InChunkSize * InBlockSize]),
            BlockSize(InBlockSize)
        {
            for(size_t i = 0; i < InChunkSize; ++i)
            {
                SBlock* Block = GetBlockAt(i);
				new (Block) SBlock();

				Block->Data = reinterpret_cast<char*>(Block + 1);
 
				if (i + 1 == InChunkSize)
					Block->Next = nullptr;
				else
					Block->Next = GetBlockAt(i + 1);
            }
        }

		SBlock* GetBlockAt(const size_t& InIdx)
		{
            return reinterpret_cast<SBlock*>(Blocks.get() + InIdx * BlockSize);
		}
    };

    TDynamicPool(const size_t& InChunkSize = 100, const size_t& InDataSize = sizeof(T))
        : ChunkSize(InChunkSize), BlockSize(InDataSize + sizeof(SBlock)), DataSize(InDataSize)
    {
        CurrentChunk = std::make_unique<SChunk>(ChunkSize, BlockSize);
        FreeBlock = CurrentChunk->GetBlockAt(0);
    }

    /** Copy not supported for now */
    template<typename U>
    TDynamicPool(const TDynamicPool<U>& InOther) = delete;

    template<typename U>
    void operator=(const TDynamicPool<U>& InOther) = delete;

    /**
     * Allocate without calling ctor
     */
	T* Allocate()
	{
		/** Check if chunk is full */
		if (!FreeBlock)
		{
			/** Create a new chunk */
			SChunk* Chunk = new SChunk(ChunkSize, BlockSize);
			Chunk->Previous = std::move(CurrentChunk);
			CurrentChunk.reset(Chunk);
			FreeBlock = CurrentChunk->GetBlockAt(0);
		}

		/** "Allocate" a block */
		SBlock* Block = FreeBlock;
		FreeBlock = Block->Next;

		T* Data = reinterpret_cast<T*>(Block->Data);

		return Data;
	}

    template<typename... Args>
    T& Allocate(Args&&... InArgs)
    {
        T* Data = Allocate();
        new (Data) T(std::forward<Args>(InArgs)...);

        return *Data;
    }

    void Free(T& InElem)
    {
        /** Call the dtor, then mark the block as free */
		InElem.T::~T();

		SBlock* Block = reinterpret_cast<SBlock*>(std::addressof(InElem)) - 1;
    	Block->Next = FreeBlock;
		FreeBlock = Block;
    }
private:
    size_t ChunkSize;
    size_t BlockSize;
    size_t DataSize;
    std::unique_ptr<SChunk> CurrentChunk;
    SBlock* FreeBlock;
};
