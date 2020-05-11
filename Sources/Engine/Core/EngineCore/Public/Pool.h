#pragma once

#include <memory>

/**
 * Simple object pool implementation using linked lists
 * A pool contains multiple chunks, each chunk contain a fixed number of blocks
 */
template<typename T>
struct TPool
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

                auto s = alignof(SBlock);

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

    TPool(const size_t& InChunkSize = 100, const size_t& InDataSize = sizeof(T))
        : ChunkSize(InChunkSize), BlockSize(InDataSize + sizeof(SBlock)), DataSize(InDataSize)
    {
        CurrentChunk = std::make_unique<SChunk>(ChunkSize, BlockSize);
        FreeBlock = CurrentChunk->GetBlockAt(0);
    }

    /** Copy not supported for now */
    template<typename U>
    TPool(const TPool<U>& InOther) = delete;

    template<typename U>
    void operator=(const TPool<U>& InOther) = delete;

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
