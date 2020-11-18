#include "Parser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

/**
* https://stackoverflow.com/questions/6089231/getting-std-ifstream-to-handle-lf-cr-and-crlf
*/
std::istream& safeGetline(std::istream& is, std::string& t)
{
    t.clear();

    //The characters in the stream are read one-by-one using a std::streambuf.
    //That is faster than reading them one-by-one using the std::istream.
    //Code that uses streambuf this way must be guarded by a sentry object.
    //The sentry object performs various tasks,
    //such as thread synchronization and updating the stream state.

    std::istream::sentry se(is, true);
    std::streambuf* sb = is.rdbuf();

    for(;;) {
        int c = sb->sbumpc();
        switch (c) {
        case '\n':
            return is;
        case '\r':
            if(sb->sgetc() == '\n')
                sb->sbumpc();
            return is;
        case std::streambuf::traits_type::eof():
            //Also handle the case when the last line has no line ending
            if(t.empty())
                is.setstate(std::ios::eofbit);
            return is;
        default:
            t += (char)c;
        }
    }
}

std::vector<std::string> ReadTextFile(const std::string_view& InFilename)
{
	std::vector<std::string> Lines;
	Lines.reserve(50);

	std::string Line;
	std::ifstream File(InFilename.data());
	if (!File.is_open())
		Fatal("Failed to read file %s", InFilename.data());

	while (!safeGetline(File, Line).eof())
	{
		Lines.emplace_back(Line);
	}

	File.close();

	return Lines;
}

std::vector<std::string> Tokenize(const std::string& InString, 
	const char& InDelimiter)
{
	std::vector<std::string> Tokens;
	Tokens.reserve(5);

	std::stringstream Stream(InString);
	std::string Token;
	while (std::getline(Stream, Token, InDelimiter)) 
	{
		/** Remove tabs */
		Token.erase(std::remove(Token.begin(), Token.end(), '\t'), Token.end());
		if(!Token.empty())
			Tokens.push_back(std::move(Token));
	}

	return Tokens;
}

CParser::CParser(CHeader* InHeader, const std::string_view& InPathHeader,
	const bool& bInIgnorePropertiesAndFunctions)
	: Header(InHeader), Path(InPathHeader), Filename(std::filesystem::path(InPathHeader).filename().string()), 
	CurrentType(EType::None), CurrentAccess(EAccess::Public), 
	CurrentStruct(nullptr), NestedEncounters(0), CurrentClass(nullptr), ScopeCounter(0), 
	bIgnorePropertiesAndFunctions(bInIgnorePropertiesAndFunctions)
{
	std::vector<std::string> Lines = ReadTextFile(InPathHeader);
	
	/**
	 * Search for .gen.h include
	 */
	bool bFoundGen = false;
	bool bShouldPrintErr = false;
	std::filesystem::path NoExt = std::filesystem::path(InPathHeader).filename();
	NoExt.replace_extension();
	std::string GenInc = NoExt.string() + ".gen.h";
	for (size_t i = 0; i < Lines.size(); ++i)
	{
		/** Check if we should skip this file */
		if (Lines[i].find("ZRT:SKIP") != std::string::npos)
		{
			return;
		}

		if (Lines[i].find(GenInc) != std::string::npos)
		{
			bFoundGen = true;
		}
			
		if (Lines[i].rfind("/*", 0) != std::string::npos ||
			Lines[i].rfind("*", 0) != std::string::npos ||
			Lines[i].rfind("#define", 0) != std::string::npos) 
		{
			if (Lines[i].find("ZCLASS") != std::string::npos ||
				Lines[i].find("ZSTRUCT") != std::string::npos ||
				Lines[i].find("ZENUM") != std::string::npos)
			{
				bShouldPrintErr = true;
			}
		}
	}

	if(!bFoundGen && bShouldPrintErr)
	{
		Fatal("%s : error: You must include at the end of includes %s !",
			Filename.c_str(),
			GenInc.c_str());
	}

	for(size_t i = 0; i < Lines.size(); ++i)
	{
		const std::string& Line = Lines[i];
		std::vector<std::string> Words = Tokenize(Line);

		if(Words.empty())
			continue;

		/**
		 * Check if it's a namespace or function
		 */
		if(Words[0].find("{") != std::string::npos)
		{
			const std::string& PrevLine = Lines[i - 1];
			std::vector<std::string> PrevWords = Tokenize(PrevLine);
			ScopeCounter++;
			if(PrevWords.size() < 2)
				continue;

			if (PrevLine.find("namespace") != std::string::npos)
			{
				NamespacesAtScope.emplace_back(ScopeCounter - 1);
				OpenNamespace(PrevWords[1]);
			}
		}
		else if(Words[0].find("}") != std::string::npos)
		{
			ScopeCounter--;
			if(Words[0].find("};") == std::string::npos)
			{
				for(const auto& Scope : NamespacesAtScope)
				{
					if(Scope == ScopeCounter)
						CloseNamespace();
				}

				NamespacesAtScope.erase(std::remove(NamespacesAtScope.begin(),
					NamespacesAtScope.end(), ScopeCounter), NamespacesAtScope.end());
			}
		}

		if(CurrentType != EType::None)
		{
			ParseLine(Lines, Words, i);
		}

		/**
		 * Search for structs or classes
		 */
		if (Words[0].find("struct") != std::string::npos) 
		{
			BeginStructOrClass(EType::Struct, Lines, Words, i);
		}
		else if(Words[0].find("class") != std::string::npos)
		{
			BeginStructOrClass(EType::Class, Lines, Words, i);
		}
		else if(Words[0].find("enum") != std::string::npos)
		{
			BeginEnum(Lines, Words, i);
		}
	}
}

void CParser::BeginStructOrClass(const EType& InNewType,
	const std::vector<std::string>& InLines,
	const std::vector<std::string>& InWords, const size_t& InLine)
{
	/**
	 * Detect if this struct/class has a ZSTRUCT or ZCLASS macro
	 */
	if(!IsValidReflectedType(InNewType, InLines, InLine))
	{
		if(CurrentType != EType::None)
			NestedEncounters++;
		return;
	}

	size_t NameIdx = InLine;
	size_t NameWordIdx = 0;
	CurrentObjectName = GetObjectName(InLines, InWords, InLine, NameIdx,
		NameWordIdx);

	/**
	 * Parse parent structures/classes
	 */
	size_t ParentLine = NameIdx;
	/**
	 * By default NameWordIdx + 1 should be ":"
	 *	and + 2 "public" or "private"
	 */
	size_t PossibleParentWordIdx = NameWordIdx + 3;
	const std::string& Line = InLines[ParentLine];
	std::vector<std::string> Words = Tokenize(Line);
	if(NameWordIdx + 1 < Words.size() && Words[NameWordIdx + 1] == "final")
		PossibleParentWordIdx++;

	std::vector<std::string> Parents;

	EStructFlags Flags = EStructFlags::None;

	if(!bIgnorePropertiesAndFunctions)
	{
		/**
		 * Parse macro metadata
		 */
		std::vector<std::string> MetaWords = Tokenize(InLines[NameIdx - 1], '(');
		if(MetaWords.size() > 1)
		{
			MetaWords = Tokenize(MetaWords[1], ',');
			for(auto& Word : MetaWords)
			{
				/** Sanitize word */
				Word.erase(std::remove_if(Word.begin(), Word.end(), isspace), Word.end());
				Word.erase(std::remove(Word.begin(), Word.end(), '('), Word.end());
				Word.erase(std::remove(Word.begin(), Word.end(), ')'), Word.end());
				
				auto Flag = GStructClassMap.find(Word);
				if(Flag != GStructClassMap.end())
					Flags |= Flag->second;
			}
		}

		while(true)
		{
			const std::string& Line = InLines[ParentLine];
			std::vector<std::string> Words = Tokenize(Line);
			if(PossibleParentWordIdx > Words.size() - 1)
				break;

			std::string Parent = Words[PossibleParentWordIdx];

			Parent.erase(std::remove(Parent.begin(), Parent.end(), ','), Parent.end());
			size_t LastNamespacePos = Parent.find_last_of("::");
			if(LastNamespacePos != std::string::npos)
				Parent = Parent.substr(LastNamespacePos + 1, Parent.size() - (LastNamespacePos + 1));
			Parents.emplace_back(std::move(Parent));

			if(Words[PossibleParentWordIdx].find(",") != std::string::npos)
			{
				/**
				 * Look at next line
				 */
				if(PossibleParentWordIdx == Words.size() - 1)
				{
					ParentLine++;
					PossibleParentWordIdx = 1;
				}
				else
				{
					PossibleParentWordIdx += 2;
				}
			}
			else
			{
				break;
			}
		}
	}
	
	if(CurrentType != EType::None)
	{
		Fatal("%s(%d) : error: Nested types are not supported! (%s)",
			Filename.c_str(),
			InLine + 1,
			CurrentObjectName.c_str());
	}

	/** Update parser state */
	CurrentType = InNewType;

	/** Access is always private in C++ if it's a class */
	CurrentAccess = InNewType == EType::Struct ? EAccess::Public : EAccess::Private;

	if(!bIgnorePropertiesAndFunctions)
	{
		if (CurrentType == EType::Struct)
		{
			CurrentStruct = &Header->AddStruct(CurrentObjectName, CurrentNamespace, InLine + 1);
			CurrentStruct->GetFlags() = Flags;
		}
		else
		{
			CurrentClass = &Header->AddClass(CurrentObjectName, CurrentNamespace, InLine + 1);
			CurrentStruct = static_cast<CStruct*>(CurrentClass);
		}

		for(const auto& Parent : Parents)
		{
			CurrentStruct->AddParent(Parent);
		}
	}

	CTypeDatabase::Get().RegisterType(CurrentObjectName);
}

void CParser::BeginEnum(const std::vector<std::string>& InLines,
	const std::vector<std::string>& InWords, const size_t& InLine)
{
	/** Check if C++ enum */
	if(InLines[InLine].find("enum class") == std::string::npos)
		return;

	size_t NameIdx = InLine;
	size_t NameWordIdx = 0;
	CurrentObjectName = GetObjectName(InLines, InWords, InLine, NameIdx,
		NameWordIdx, true);

	/**
	 * Detect if this struct/class has a ZSTRUCT or ZCLASS macro
	 */
	if(!IsValidReflectedType(EType::Enum, InLines, InLine))
	{
		if(CurrentType != EType::None)
			NestedEncounters++;
		return;
	}

	CurrentType = EType::Enum;

	if(!bIgnorePropertiesAndFunctions)
		CurrentEnum = &Header->AddEnum(CurrentNamespace, CurrentObjectName);
	
	CTypeDatabase::Get().RegisterType(CurrentObjectName);
}

void CParser::ParseLine(const std::vector<std::string>& InLines,
	const std::vector<std::string>& InWords, const size_t& InLine)
{
	/**
	 * Check for type ending
	 */
	if(InWords[0].find(GStructClassEnumEnding) != std::string::npos)
	{
		if(NestedEncounters == 0)
		{
			EndCurrentType();
			return;
		}
		else
		{
			NestedEncounters--;
			return;
		}
	}

	if(!bIgnorePropertiesAndFunctions)
	{
		if(CurrentType == EType::Struct ||
			CurrentType == EType::Class)
		{
			/**
			 * Search for REFL_BODY() if not found
			 */
			if(CurrentStruct->GetBodyLine() == -1 && 
				InWords[0].find(GReflBodyMacro) != std::string::npos)
			{
				CurrentStruct->SetBodyLine(InLine + 1);
				return;
			}

			/** Search for ctors */
			if (InWords[0].find(CurrentStruct->GetName() + "(") != std::string::npos &&
				InWords[0].find("~") == std::string::npos &&
				InWords[0].find("()") == std::string::npos &&
				InLines[InLine].find("delete") == std::string::npos &&
				InLines[InLine].find("default") == std::string::npos)
			{
				std::string FCtor;

				size_t CurrentLine = InLine;
				std::string Line = InLines[CurrentLine];
				std::vector<std::string> Ctor = Tokenize(Line, '(');
				std::vector<std::string> Args = Tokenize(Ctor[1], ',');
				size_t CurrentWord = 0;
				while(true)
				{
					if(Args[CurrentWord] != ")")
					{
						// spaghetti hell
						std::vector<std::string> CtorTypeTokenized = Tokenize(Args[CurrentWord], ' ');
						std::vector<size_t> TokensToErase;
						for(size_t i = 0; i < CtorTypeTokenized.size(); ++i)
							if(CtorTypeTokenized[i].find(")") != std::string::npos)
								TokensToErase.push_back(i);
						std::sort(TokensToErase.begin(), TokensToErase.end(),
							[](const auto& left, const auto& right) { return left > right; });
						for(const auto& TokenToErase : TokensToErase)
							CtorTypeTokenized.erase(CtorTypeTokenized.begin() + TokenToErase);
						if(TokensToErase.empty())
							CtorTypeTokenized.erase(CtorTypeTokenized.end() - 1);
						std::string CtorType;
						for(const auto& Word : CtorTypeTokenized)
							CtorType += Word + " ";
						/*if(CtorType.find_last_of(':') != std::string::npos && 
							CtorType.find_last_of(':') - 1 != ':')
						{
							CtorType.erase(CtorType.begin() + CtorType.find_last_of(':'), CtorType.end());
						}*/
						if(!FCtor.empty())
							FCtor += ",";
						CtorType.erase(std::remove(CtorType.begin(), CtorType.end(), '='), CtorType.end());
						std::cout << CtorType << std::endl;
						FCtor += CtorType;
					}
					else break;

					if(CurrentWord == Args.size() - 1)
					{
						if (Line.find(')') != std::string::npos)
						{
							break;
						}
						else
						{
							CurrentWord = 0;
							CurrentLine++;
							Line = InLines[CurrentLine];
							Args = Tokenize(Line, ',');
						}
					}
				}

				CurrentStruct->AddCtor(FCtor);
			}

			/**
			 * Check for accessors
			 */
			if(InWords[0].find(GPublicAccessor) != std::string::npos)
			{
				CurrentAccess = EAccess::Public;
				return;
			}
			else if(InWords[0].find(GProtectedAccessor) != std::string::npos)
			{
				CurrentAccess = EAccess::Protected;
				return;
			}
			else if(InWords[0].find(GPrivateAccessor) != std::string::npos)
			{
				CurrentAccess = EAccess::Private;
				return;
			}

			/**
			 * Try to parse a potential property
			 */
			if(InWords[0].rfind(GPropertyMacro, 0) != std::string::npos)
			{
				ParseProperty(InLines, InWords, InLine);
			}
		}
		else if(CurrentType == EType::Enum)
		{
			if(InWords[0].find("{") == std::string::npos)
			{
				auto Words = Tokenize(InWords[0], ',');
				
				if(!Words.empty() && Words[0].find("*") == std::string::npos)
					CurrentEnum->AddValue(Words[0]);
			}
		}
	}
}

void CParser::EndCurrentType()
{
	if(!bIgnorePropertiesAndFunctions && 
		(CurrentType == EType::Struct || CurrentType == EType::Class) &&
		CurrentStruct->GetBodyLine() == -1)
	{
		Fatal("%s(%d) : error: Missing ZE_REFL_BODY() macro for struct/class %s",
			Filename.c_str(),
			CurrentStruct->GetDeclLine(),
			CurrentObjectName.c_str());
	}

	CurrentAccess = EAccess::Public;
	CurrentType = EType::None;
	CurrentStruct = nullptr;
	CurrentClass = nullptr;
}

void CParser::ParseProperty(const std::vector<std::string>& InLines,
	const std::vector<std::string>& InWords, const size_t& InLine)
{
	EPropertyFlags Flags = EPropertyFlags::None;
	EPropertyAttributes Attributes = EPropertyAttributes::None;
	
	/**
	 * Parse macro metadata
	 */
	std::vector<std::string> MetaWords = Tokenize(InLines[InLine], '(');
	MetaWords = Tokenize(MetaWords[1], ',');
	for(auto& Word : MetaWords)
	{
		/** Sanitize word */
		Word.erase(std::remove_if(Word.begin(), Word.end(), isspace), Word.end());
		Word.erase(std::remove(Word.begin(), Word.end(), '('), Word.end());
		Word.erase(std::remove(Word.begin(), Word.end(), ')'), Word.end());

		auto Flag = GPropFlagMap.find(Word);
		if(Flag != GPropFlagMap.end())
			Flags |= Flag->second;
	}

	/**
	 * Next line must contain a property
	 */
	size_t PossiblePropertyLineIndex = InLine + 1;
	std::string PossiblePropertyLine = "";
	std::vector<std::string> Words;
	while(PossiblePropertyLine.empty())
	{
		PossiblePropertyLine = InLines[PossiblePropertyLineIndex++];
	}

	Words = Tokenize(PossiblePropertyLine);

	/**
	 * Words may contains specifiers
	 */
	size_t CurrentWord = 0;

	while(true)
	{	
		if(Words[CurrentWord].find(GApiMacro) != std::string::npos)
		{
			CurrentWord++;
		}
		else if(Words[CurrentWord].find(GConstExpr) != std::string::npos)
		{
			Attributes |= EPropertyAttributes::Constexpr;
			CurrentWord++;
		}
		else if (Words[CurrentWord].find(GConst) != std::string::npos)
		{
			Attributes |= EPropertyAttributes::Const;
			CurrentWord++;
		}
		else if (Words[CurrentWord].find(GMutable) != std::string::npos)
		{
			Attributes |= EPropertyAttributes::Mutable;
			CurrentWord++;
		}
		else if (Words[CurrentWord].find(GVolatile) != std::string::npos)
		{
			Attributes |= EPropertyAttributes::Volatile;
			CurrentWord++;
		}
		else
		{
			break;
		}
	}

	/**
	 * CurrentWord + 1 must contain the type
	 *	and +2 the name
	 */
	std::string Type = Words[CurrentWord++];
	
	/** Check if type is valid */
	if(Type.find("<") != std::string::npos ||
		Type.find(">") != std::string::npos)
	{
		bool bIsValid = false;

		for(const auto& AcceptedTemplateType : GAcceptedTemplatedTypes)
		{
			if(Type.find(AcceptedTemplateType) != std::string::npos)
			{
				bIsValid = true;
				break;
			}
		}

		if(!bIsValid)
		{
			Fatal("%s(%d) : error: Unsupported type %s",
				Filename.c_str(),
				InLine + 2,
				Type.c_str());
		}
	}

	/**
	 * If CurrentWord doesn't contain a ; then it is a type
	 */
	if(Words[CurrentWord].find(";") == std::string::npos)
	{
		Type += " " + Words[CurrentWord++];
	}
	
	std::string Name = Words[CurrentWord];
	Name.erase(std::remove(Name.begin(), Name.end(), ';'), Name.end());

	CurrentStruct->AddProperty(std::move(Name), 
		std::move(Type),
		CurrentAccess,
		std::move(Flags),
		Attributes);

	CTypeDatabase::Get().RegisterType(Name);
}

void CParser::OpenNamespace(const std::string& InNamespace)
{
	CurrentNamespace += InNamespace + "_";
}

void CParser::CloseNamespace()
{
	std::vector<std::string> Depths = Tokenize(CurrentNamespace, '_');
	CurrentNamespace = "";
	for(size_t i = 0; i < Depths.size(); ++i)
	{
		if(i < Depths.size() - 1)
			CurrentNamespace += Depths[i] + "_";
	}
}

bool CParser::IsValidReflectedType(const EType& InNewType, const std::vector<std::string>& InLines,
	const size_t& InLine) const
{
	if(InLine == 0)
		return false;

	const std::string& PossibleMacroLine = InLines[InLine - 1];
	std::vector<std::string> Words = Tokenize(PossibleMacroLine);
	if(Words.empty())
		return false;

	if (Words[0].find(GStructMacro) != std::string::npos)
	{
		if(InNewType == EType::Struct)
			return true;
		else
			Fatal("%s(%d) : error: Bad macro for %s! Must be ZCLASS", 
				Filename.c_str(),
				InLine + 1,
				CurrentObjectName.c_str());
	} 
	else if (Words[0].find(GClassMacro) != std::string::npos)
	{
		if (InNewType == EType::Class)
			return true;
		else
			Fatal("%s(%d) : error: Bad macro for %s! Must be ZSTRUCT", 
				Filename.c_str(),
				InLine + 1, 
				CurrentObjectName.c_str());
	}
	else if (Words[0].find(GEnumMacro) != std::string::npos)
	{
		if (InNewType == EType::Enum)
			return true;
		else
			Fatal("%s(%d) : error: Bad macro for %s! Must be ZENUM", 
				Filename.c_str(),
				InLine + 1, 
				CurrentObjectName.c_str());
	}
	
	return false;
}

std::string CParser::GetObjectName(const std::vector<std::string>& InLines,
	const std::vector<std::string>& InWords, const size_t& InLine, size_t& InNameLine,
	size_t& InWordIdx, const bool& bIsEnum) const
{
	/**
	 * Try to find the name in the current line
	 */
	InNameLine = InLine;

	/**
	 * There may be a XXXX_API macro in the way, ignore it
	 */
	if(InWords.size() > 1 &&
		InWords[1].find(GApiMacro) != std::string::npos)
	{
		/**
		 * If there is no other words, the name must be in the next line
		 */
		if(InWords.size() < 2)
		{
			std::vector<std::string> Words = Tokenize(InLines[InLine + 1]);
			InNameLine = InLine;
			InWordIdx = 0;
			return Words[0];
		}
		else
		{
			if(bIsEnum)
			{
				InWordIdx = 3;
				return InWords[3];
			}
			else
			{
				InWordIdx = 2;
				return InWords[2];
			}
		}
	}
	else
	{
		/**
		 * If there is no other words, the name must be in the next line
		 */
		if(InWords.size() <= 1)
		{
			std::vector<std::string> Words = Tokenize(InLines[InLine + 1]);
			InNameLine = InLine;
			InWordIdx = 0;
			return Words[0];
		}
		else
		{
			if(bIsEnum)
			{
				InWordIdx = 2;
				return InWords[2];
			}
			else
			{
				InWordIdx = 1;
				return InWords[1];
			}
		}
	}

	return "";
}