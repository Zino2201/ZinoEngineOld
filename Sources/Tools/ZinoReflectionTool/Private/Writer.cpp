#include "Writer.h"
#include "Header.h"
#include <fstream>
#include <string>
#include <filesystem>
#include <regex>

const CHeader* CurrentHeader = nullptr;
uint64_t UniqueHeaderID = 0;
std::string CurrentFileUniqueId = "";

std::string GetObjectType(const std::string& InNamespace,
	const std::string& InObjectName)
{
	if(InNamespace.empty())
		return InObjectName;

	std::string RetName = InNamespace + InObjectName;
	return std::regex_replace(RetName, std::regex("_"), "::");
}

void ProcessHeaderStructOrClass(std::ofstream& File, const CStruct& InStruct,
	const bool& bIsClass)
{
	std::string Type = GetObjectType(InStruct.GetNamespace(), InStruct.GetName());

	/** Generate DECLARE_REFL_TYPE */
	std::string Parents;
	for(const auto& Parent : InStruct.GetParents())
		Parents += Parent;

	File << "/** " << InStruct.GetName() << " [" << Parents << "] */\n";

	/** Fwd declare */
	{
		std::string_view FwdBeg = bIsClass ? "class " : "struct ";
		if (!InStruct.GetNamespace().empty())
		{
			std::string Namespace = std::regex_replace(InStruct.GetNamespace(), std::regex("_"), "::");
			std::string_view NamespaceView(Namespace.c_str(), Namespace.size() - 2);
			File << "namespace " << NamespaceView << " { " << FwdBeg << InStruct.GetName() << "; }\n";
		}
		else
		{
			File << FwdBeg;
			File << InStruct.GetName();
			File << ";\n";
		}
	}

	File << GDeclareReflTypeMacro << Type
		<< ", \"" << InStruct.GetName() << "\");";
	File << "\n";
	if(bIsClass)
		File << "template<> static constexpr bool ZE::Refl::TIsReflClass<" << Type << "> = true;\n";
	else
		File << "template<> static constexpr bool ZE::Refl::TIsReflStruct<" << Type << "> = true;\n";

	/** Generate body macro */
	File << "#define " << GReflBodyDefMacro << CurrentFileUniqueId << "_" <<
		InStruct.GetBodyLine() << " ";
	std::string_view M = GDeclareStructMacro;
	if(bIsClass)
	{
		const auto& Class = static_cast<const CClass&>(InStruct);
		if(Class.IsAbstract())
			M = "ZE_REFL_DECLARE_ABSTRACT_CLASS(";
		else
			M = GDeclareClassMacro;
	}

	File << M << Type << ", \"" << InStruct.GetName() << "\")";
	File << "\n";
	File << "\n";
}

void ProcessHeaderEnum(std::ofstream& File, const CEnum& InEnum)
{
	std::string Type = GetObjectType(InEnum.GetNamespace(), InEnum.GetName());
	
	/** Fwd declare */
	{
		std::string_view FwdBeg = "enum class ";
		if (!InEnum.GetNamespace().empty())
		{
			std::string Namespace = std::regex_replace(InEnum.GetNamespace(), std::regex("_"), "::");
			std::string_view NamespaceView(Namespace.c_str(), Namespace.size() - 2);
			File << "namespace " << NamespaceView << " { " << FwdBeg << InEnum.GetName() << "; }\n";
		}
		else
		{
			File << FwdBeg;
			File << InEnum.GetName();
			File << ";\n";
		}
	}

	File << GDeclareReflTypeMacro << Type
		<< ", \"" << InEnum.GetName() << "\");\n";
	File << "template<> static constexpr bool ZE::Refl::TIsReflEnum<" << Type << "> = true;\n";
	File << "ZE_REFL_DECLARE_ENUM(" << Type << ", " << InEnum.GetName() << ")\n";
	File << "\n";
}

#include <iostream>

void WriteGenHeader(const std::string_view& InOutDir, const CHeader& InHeader)
{
	if(!CurrentHeader)
		return;

	std::string Path = InOutDir.data();
	Path += "/";

	/**
	 * Create directory if it doesn't exist
	 */
	std::filesystem::path FsPath(Path);
	if(!std::filesystem::exists(FsPath))
		std::filesystem::create_directories(FsPath);

	std::filesystem::path Filename(InHeader.GetFilename());
	Filename.replace_extension();
	Path += Filename.string() + ".gen.h";

	std::cout << Path << std::endl;

	std::ofstream File(Path.c_str(), std::ios::out);
	File << GGeneratedFileHeader;
	File << "\n\n";
	File << GGeneratedHeaderIncludes;
	File << "#undef ZE_CURRENT_FILE_UNIQUE_ID\n";
	CurrentFileUniqueId = InHeader.GetModule() + "_" + std::to_string(UniqueHeaderID);
	File << "#define ZE_CURRENT_FILE_UNIQUE_ID " << CurrentFileUniqueId << "\n\n";

	for(const auto& Struct : InHeader.GetStructs())
		ProcessHeaderStructOrClass(File, Struct, false);

	for (const auto& Class : InHeader.GetClasses())
		ProcessHeaderStructOrClass(File, Class, true);

	for (const auto& Enum : InHeader.GetEnums())
		ProcessHeaderEnum(File, Enum);

	File.close();
}

void ProcessProperty(std::ofstream& File, const std::string& InType, 
	const CStruct& InStruct, const SProperty& InProperty)
{
	std::string BuilderName = "Builder_" + InStruct.GetName();

	File << "\n\t\t" << BuilderName << ".Property(\"" << InProperty.Name << "\", &" << InType << "::" << InProperty.Name
		<< ", ";

	if(InProperty.Flags == EPropertyFlags::None)
	{
		File << "EPropertyFlagBits::None";
	}

	bool bHasAddedFlag = false;
	if(HAS_FLAG(InProperty.Flags, EPropertyFlags::Serializable))
	{
		bHasAddedFlag = true;
		File << "EPropertyFlagBits::Serializable";
	}

	File << ");";
}

void ProcessParents(std::ofstream& InFile, const CStruct& InStruct)
{
	std::string BuilderName = "Builder_" + InStruct.GetName();

	for(const auto& Parent : InStruct.GetParents())
	{
		if(CTypeDatabase::Get().HasType(Parent))
			InFile << "\n\t" << BuilderName << ".Parent(\"" << Parent << "\");";
	}
}

void ProcessCtors(std::ofstream& InFile, const CStruct& InStruct)
{
	std::string BuilderName = "Builder_" + InStruct.GetName();
	
	for(const auto& Ctor : InStruct.GetCtors())
	{
		InFile << "\t" << BuilderName << ".Ctor<" << Ctor << ">();";
	}

	if(InStruct.GetCtors().empty())
	{
		InFile << "\t" << BuilderName << ".Ctor<>();";
	}
}

void ProcessCppStruct(std::ofstream& File, const CStruct& InStruct)
{
	std::string Type = GetObjectType(InStruct.GetNamespace(), InStruct.GetName());
	std::string BuilderName = "Builder_" + InStruct.GetName();

	File << "\t" << GStructBuilder << "<" << Type << "> " << BuilderName
		<< "(\"" << InStruct.GetName() << "\");\n";
	ProcessCtors(File, InStruct);
	for(const auto& Property : InStruct.GetProperties())
		ProcessProperty(File, Type, InStruct, Property);
	ProcessParents(File, InStruct);
	File << ";\n";
	File << "\tStaticStruct_" << InStruct.GetName() << " = "
		<< "Builder_" << InStruct.GetName() << ".GetStruct();\n";
}

void ProcessCppClass(std::ofstream& File, const CClass& InClass)
{
	std::string Type = GetObjectType(InClass.GetNamespace(), InClass.GetName());

	std::string BuilderName = "Builder_" + InClass.GetName();

	File << "\t" << GClassBuilder << "<" << Type << "> " << BuilderName
		<< "(\"" << InClass.GetName() << "\");\n";
	ProcessCtors(File, InClass);
	for (const auto& Property : InClass.GetProperties())
		ProcessProperty(File, Type, InClass, Property);
	ProcessParents(File, InClass);
	File << ";\n";
	if (InClass.GetName().rfind("I", 0) == 0)
	{
		File << "\n\t" << BuilderName << ".MarkAsInterface();";
	}
	File << "\tStaticClass_" << InClass.GetName() << " = " 
		<< "Builder_" << InClass.GetName() << ".GetClass();\n";
}

void ProcessCppEnum(std::ofstream& File, const CEnum& InEnum)
{
	std::string Type = GetObjectType(InEnum.GetNamespace(), InEnum.GetName());

	std::string BuilderName = "Builder_" + InEnum.GetName();
	File << "\t" << "Builders::TEnumBuilder" << "<" << Type << "> " << BuilderName
		<< "(\"" << InEnum.GetName() << "\");";
	for(const auto& Value : InEnum.GetValues())
		File << "\n\t\t" << BuilderName << ".Value(\"" << Value << "\", " << Type << "::" << Value << ");";
	File << "\n";
	File << "\tStaticEnum_" << InEnum.GetName() << " = " 
		<< "Builder_" << InEnum.GetName() << ".GetEnum();\n";
}

void WriteGenCpp(const std::string_view& InOutDir, const CHeader& InHeader)
{
	if(!CurrentHeader)
		return;

	std::string Path = InOutDir.data();
	Path += "/";

	/**
	 * Create directory if it doesn't exist
	 */
	std::filesystem::path FsPath(Path);
	if(!std::filesystem::exists(FsPath))
		std::filesystem::create_directories(FsPath);

	std::filesystem::path Filename(InHeader.GetFilename());
	Filename.replace_extension();
	Path += Filename.string() + ".gen.cpp";

	std::ofstream File(Path.c_str(), std::ios::out);
	File << GGeneratedFileCpp;
	File << "\n\n";
	File << "#include " << std::filesystem::absolute(InHeader.GetPath()) << "\n";
	File << "#include \"Reflection/Builders.h\"\n";
	File << "#include \"Reflection/Class.h\"\n";
	File << "#include \"Reflection/Struct.h\"\n";
	File << "#include \"Reflection/Enum.h\"\n";
	File << "\n";

	/** Static ptrs */
	File << "/** Static variables used for GetStaticStruct()/GetStaticClass()/GetStaticEnum() */\n";
	{
		for (const auto& Struct : InHeader.GetStructs())
		{
			File << "static const ZE::Refl::CStruct* StaticStruct_" << Struct.GetName() << " = nullptr;\n";
		}

		for (const auto& Class : InHeader.GetClasses())
		{
			File << "static const ZE::Refl::CClass* StaticClass_" << Class.GetName() << " = nullptr;\n";
		}

		for (const auto& Enum : InHeader.GetEnums())
		{
			File << "static const ZE::Refl::CEnum* StaticEnum_" << Enum.GetName() << " = nullptr;\n";
		}
	}

	File << "\n";

	/** Refl builder func */
	{
		File << "namespace ZE::Refl\n{\n";
		File << GReflBuilderMacro << "(" << CurrentFileUniqueId << "_" << Filename.string() << ")\n{\n";
	//	File << "static bool bHasBeenCalled = false;\nif(bHasBeenCalled)\nreturn;\nelse\nbHasBeenCalled = true;\n";
		for(const auto& Struct : InHeader.GetStructs())
			ProcessCppStruct(File, Struct);
		for (const auto& Class : InHeader.GetClasses())
			ProcessCppClass(File, Class);
		for(const auto& Enum : InHeader.GetEnums())
			ProcessCppEnum(File, Enum);
		File << "}\n";
		File << "}\n";
	}

	/** GetStaticXXXXX */
	{
		for(const auto& Struct : InHeader.GetStructs())
		{
			std::string Type = GetObjectType(Struct.GetNamespace(), Struct.GetName());

			File << "const ZE::Refl::CStruct* " << Type << "::GetStaticStruct()\n{\n";
			File << "\treturn StaticStruct_" << Struct.GetName() << ";\n";
			File << "}\n\n";

			File << "const ZE::Refl::CStruct* " << Type << "::GetStruct() const\n{\n";
			File << "\treturn StaticStruct_" << Struct.GetName() << ";\n";
			File << "}\n\n";
		}

		for (const auto& Class : InHeader.GetClasses())
		{
			std::string Type = GetObjectType(Class.GetNamespace(), Class.GetName());

			File << "const ZE::Refl::CClass* " << Type << "::GetStaticClass()\n{\n";
			File << "\treturn StaticClass_" << Class.GetName() << ";\n";
			File << "}\n\n";

			File << "const ZE::Refl::CClass* " << Type << "::GetClass() const\n{\n";
			File << "\treturn StaticClass_" << Class.GetName() << ";\n";
			File << "}\n\n";
		}

		for(const auto& Enum : InHeader.GetEnums())
		{
			File << "const ZE::Refl::CEnum* " << "ZE__Refl_GetStaticEnumImpl_" << Enum.GetName() << "()\n{\n";
			File << "\treturn StaticEnum_" << Enum.GetName() << ";\n";
			File << "}\n\n";
		}
	}

	File.close();
}

void Writer::Write(const std::string_view& InOutDir, const CHeader& InHeader)
{
	CurrentHeader = &InHeader;
	WriteGenHeader(InOutDir, InHeader);
	WriteGenCpp(InOutDir, InHeader);
	CurrentHeader = nullptr;
	UniqueHeaderID++;
}