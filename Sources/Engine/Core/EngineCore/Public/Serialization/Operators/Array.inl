/**
 * Operators for std::array
 */
template<typename T, size_t Count>
FORCEINLINE IArchive& operator<<(IArchive& InArchive, const std::array<T, Count>& InValue)
{
	if (!InArchive.IsSaving())
		return InArchive;

	for(const auto& Elem : InValue)
		InArchive << Elem;

	return InArchive;
}

template<typename T, size_t Count>
FORCEINLINE IArchive& operator>>(IArchive& InArchive, std::array<T, Count>& InValue)
{
	if (InArchive.IsSaving())
		return InArchive;

	for (auto& Elem : InValue)
		InArchive >> Elem;

	return InArchive;
}
