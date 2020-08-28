/**
 * Operators for std::string
 */
ZE_FORCEINLINE IArchive& operator<<(IArchive& InArchive, const std::string& InValue)
{
	if (!InArchive.IsSaving())
		return InArchive;

	if(InArchive.IsBinary())
		InArchive << InValue.size();

	InArchive.Serialize(InValue.c_str(), InValue.length());
	return InArchive;
}

ZE_FORCEINLINE IArchive& operator>>(IArchive& InArchive, std::string& InValue)
{
	must(InArchive.IsBinary()); // Not supported for text

	if (InArchive.IsSaving() || !InArchive.IsBinary())
		return InArchive;

	/** Resize string */
	size_t Size = 0;
	InArchive >> Size;
	InValue.resize(Size);

	/** Copy to string */
	InArchive.Deserialize(InValue.data(), InValue.size());

	return InArchive;
}