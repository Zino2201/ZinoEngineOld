/**
 * Operators for std::vector
 */
template<typename T>
FORCEINLINE IArchive& operator<<(IArchive& InArchive, const std::vector<T>& InValue)
{
	if (!InArchive.IsSaving())
		return InArchive;

	InArchive << InValue.size();

	for(const auto& Elem : InValue)
		InArchive << Elem;

	return InArchive;
}

template<typename T>
FORCEINLINE IArchive& operator>>(IArchive& InArchive, std::vector<T>& InValue)
{
	if (InArchive.IsSaving())
		return InArchive;
	
	/** Resize vector */
	size_t Size = 0;
	InArchive >> Size;
	InValue.resize(Size);
	
	/** Copy to vector */
	for(auto& Elem : InValue)
		InArchive >> Elem;

	return InArchive;
}