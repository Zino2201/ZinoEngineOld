/**
 * Serialize() for primitives
 */

/**
 * Unsigned integers
 */
template<typename Archive>
FORCEINLINE void Serialize(Archive& InArchive, uint8_t& InData)
{
	if constexpr(TIsOutputArchive<Archive>)
		InArchive.Save(&InData, sizeof(uint8_t));
	else
		InArchive.Load(&InData, sizeof(uint8_t));
}

template<typename Archive>
FORCEINLINE void Serialize(Archive& InArchive, uint16_t& InData)
{
	if constexpr(TIsOutputArchive<Archive>)
		InArchive.Save(&InData, sizeof(uint16_t));
	else
		InArchive.Load(&InData, sizeof(uint16_t));
}

template<typename Archive>
FORCEINLINE void Serialize(Archive& InArchive, uint32_t& InData)
{
	if constexpr(TIsOutputArchive<Archive>)
		InArchive.Save(&InData, sizeof(uint32_t));
	else
		InArchive.Load(&InData, sizeof(uint32_t));
}

template<typename Archive>
FORCEINLINE void Serialize(Archive& InArchive, uint64_t& InData)
{
	if constexpr(TIsOutputArchive<Archive>)
		InArchive.Save(&InData, sizeof(uint64_t));
	else
		InArchive.Load(&InData, sizeof(uint64_t));
}

/** Signed integers */
template<typename Archive>
FORCEINLINE void Serialize(Archive& InArchive, int8_t& InData)
{
	if constexpr(TIsOutputArchive<Archive>)
		InArchive.Save(&InData, sizeof(int8_t));
	else
		InArchive.Load(&InData, sizeof(int8_t));
}

template<typename Archive>
FORCEINLINE void Serialize(Archive& InArchive, int16_t& InData)
{
	if constexpr(TIsOutputArchive<Archive>)
		InArchive.Save(&InData, sizeof(int16_t));
	else
		InArchive.Load(&InData, sizeof(int16_t));
}

template<typename Archive>
FORCEINLINE void Serialize(Archive& InArchive, int32_t& InData)
{
	if constexpr(TIsOutputArchive<Archive>)
		InArchive.Save(&InData, sizeof(int32_t));
	else
		InArchive.Load(&InData, sizeof(int32_t));
}

template<typename Archive>
FORCEINLINE void Serialize(Archive& InArchive, int64_t& InData)
{
	if constexpr(TIsOutputArchive<Archive>)
		InArchive.Save(&InData, sizeof(int64_t));
	else
		InArchive.Load(&InData, sizeof(int64_t));
}

template<typename Archive>
FORCEINLINE void Serialize(Archive& InArchive, bool& InData)
{
	if constexpr(TIsOutputArchive<Archive>)
		InArchive.Save(&InData, sizeof(bool));
	else
		InArchive.Load(&InData, sizeof(bool));
}

template<typename Archive, typename SizeType>
FORCEINLINE void Serialize(Archive& InArchive, TContainerSize<SizeType>& InData)
{
	if constexpr (TIsOutputArchive<Archive>)
		InArchive.Save(&InData, sizeof(SizeType));
	else
		InArchive.Load(&InData, sizeof(SizeType));
}