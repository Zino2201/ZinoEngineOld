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

	File << "ZE_REFL_DECLARE_CLASS(" << Type << ");";
	File << "\n";

	/** Generate body macro */
	File << "#define " << GReflBodyDefMacro << CurrentFileUniqueId << "_" <<
		InStruct.GetBodyLine() << " ";
	if(bIsClass) File << GDeclareStructMacro << Type << ")\n";
	else File << "ZE_REFL_DECLARE_STRUCT_BODY(" << Type << ")\n";
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

	File << "ZE_REFL_DECLARE_ENUM(" << Type << ")\n";
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

	File << "\n\t\t" << BuilderName << ".property<" << InProperty.Type 
		<< ">(\"" << InProperty.Name << "\", &" << InType << "::" << InProperty.Name
			<< ", ";

	if(InProperty.Flags == EPropertyFlags::None)
	{
		File << "PropertyFlagBits::None";
	}

	bool bHasAddedFlag = false;
	if(HAS_FLAG(InProperty.Flags, EPropertyFlags::Serializable))
	{
		if(bHasAddedFlag) File << "| ";
		bHasAddedFlag = true;
		File << "PropertyFlagBits::Serializable";
	}

	if(HAS_FLAG(InProperty.Flags, EPropertyFlags::Visible))
	{
		if(bHasAddedFlag) File << "| ";
		File << "PropertyFlagBits::Visible";
		bHasAddedFlag = true;
	}

	if(HAS_FLAG(InProperty.Flags, EPropertyFlags::Editable))
	{
		if(bHasAddedFlag) File << "| ";
		bHasAddedFlag = true;
		File << "PropertyFlagBits::Editable";
	}

	File << ");";
}

void ProcessParents(std::ofstream& InFile, const CStruct& InStruct)
{
	std::string BuilderName = "Builder_" + InStruct.GetName();

	for(const auto& Parent : InStruct.GetParents())
	{
		if(CTypeDatabase::Get().HasType(Parent))
			InFile << "\n\t" << BuilderName << ".parent(\"" << GetObjectType(InStruct.GetNamespace(), Parent) << "\");";
	}
}

void ProcessCtors(std::ofstream& InFile, const CStruct& InStruct)
{
	std::string BuilderName = "Builder_" + InStruct.GetName();
	
	for(const auto& Ctor : InStruct.GetCtors())
	{
		InFile << "\t" << BuilderName << ".constructor<" << Ctor << ">();";
	}

	if(InStruct.GetCtors().empty())
	{
		InFile << "\t" << BuilderName << ".constructor<>();";
	}
}

void ProcessCppStruct(std::ofstream& File, const CStruct& InStruct)
{
	std::string Type = GetObjectType(InStruct.GetNamespace(), InStruct.GetName());
	std::string BuilderName = "Builder_" + InStruct.GetName();

	File << "\t" << GStructBuilder << "<" << Type << "> " << BuilderName;
	
	if(InStruct.GetFlags() == EStructFlags::None)
		File << ";";
	else
		File << "(";

	bool bHasAddedFlag = false;
	if(HAS_FLAG(InStruct.GetFlags(), EStructFlags::HideInEditor))
	{
		if(bHasAddedFlag) File << "| ";
		bHasAddedFlag = true;
		File << "ClassFlagBits::HideInEditor";
	}
	
	if(InStruct.GetFlags() != EStructFlags::None)
		File << ");\n";

	ProcessCtors(File, InStruct);
	for(const auto& Property : InStruct.GetProperties())
		ProcessProperty(File, Type, InStruct, Property);
	ProcessParents(File, InStruct);
	File << ";\n";
}

void ProcessCppClass(std::ofstream& File, const CClass& InClass)
{
	std::string Type = GetObjectType(InClass.GetNamespace(), InClass.GetName());

	std::string BuilderName = "Builder_" + InClass.GetName();

	File << "\t" << GClassBuilder << "<" << Type << "> " << BuilderName
		<< ";\n";
	ProcessCtors(File, InClass);
	for (const auto& Property : InClass.GetProperties())
		ProcessProperty(File, Type, InClass, Property);
	ProcessParents(File, InClass);
	File << ";\n";
}

void ProcessCppEnum(std::ofstream& File, const CEnum& InEnum)
{
	std::string Type = GetObjectType(InEnum.GetNamespace(), InEnum.GetName());

	std::string BuilderName = "Builder_" + InEnum.GetName();
	File << "\t" << "builders::EnumBuilder" << "<" << Type << "> " << BuilderName
		<< ";";
	for(const auto& Value : InEnum.GetValues())
		File << "\n\t\t" << BuilderName << ".value(\"" << Value << "\", " << Type << "::" << Value << ");";
	File << "\n";
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
	File << "#include \"Reflection/Enum.h\"\n";
	File << "#include \"Reflection/Serialization.h\"\n";
	File << "\n";

	/** Static ptrs */
	File << "/** Static variables used for GetStaticStruct()/GetStaticClass()/GetStaticEnum() */\n";
	{
		for (const auto& Struct : InHeader.GetStructs())
		{
			std::string Type = GetObjectType(Struct.GetNamespace(), Struct.GetName());

			File << "ZE_REFL_SERL_REGISTER_TYPE(" << Type << ", " << Struct.GetName() << ");\n";
		}

		for (const auto& Class : InHeader.GetClasses())
		{
			std::string Type = GetObjectType(Class.GetNamespace(), Class.GetName());

			File << "ZE_REFL_SERL_REGISTER_TYPE(" << Type << ", " << Class.GetName() << ");\n";
		}
	}

	File << "\n";

	/** Refl builder func */
	{
		File << "namespace ze::reflection\n{\n";
		File << GReflBuilderMacro << "(" << CurrentFileUniqueId << "_" << Filename.string() << ")\n{\n";
		File << "using namespace ze;\n";
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