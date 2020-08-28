/**
 * Operators for char[N]
 */
template<size_t N>
FORCEINLINE IArchive& operator<<(IArchive& InArchive, const char InChar[N])
{
	if (!InArchive.IsSaving())
		return InArchive;

	InArchive.Serialize(InChar, sizeof(InChar));

	return InArchive;
}

template<size_t N>
FORCEINLINE IArchive& operator>>(IArchive& InArchive, const char InChar[N])
{
	if (InArchive.IsSaving())
		return InArchive;

	InArchive.Deserialize(InChar, sizeof(InChar));

	return InArchive;
}

/**
 * Operators for char*
 */
FORCEINLINE IArchive& operator<<(IArchive& InArchive, const char* InChar)
{
	if (!InArchive.IsSaving())
		return InArchive;

	InArchive.Serialize(InChar, strlen(InChar) * sizeof(char));

	return InArchive;
}