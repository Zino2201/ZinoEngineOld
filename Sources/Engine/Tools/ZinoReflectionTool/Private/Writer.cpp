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
		File << "template<> struct ZE::Refl::TIsReflClass<" << Type << "> { static constexpr bool Value = true; };\n";
	else
		File << "template<> struct ZE::Refl::TIsReflStruct<" << Type << "> { static constexpr bool Value = true; };\n";

	/** Generate body macro */
	File << "#define " << GReflBodyDefMacro << CurrentFileUniqueId << "_" <<
		InStruct.GetBodyLine() << " ";
	std::string_view M = GDeclareStructMacro;
	if(bIsClass)
	{
		const auto& Class = static_cast<const CClass&>(InStruct);
		if(Class.IsAbstract())
			M = "DECLARE_ABSTRACT_CLASS(";
		else
			M = GDeclareClassMacro;
	}

	File << M << Type << ", \"" << InStruct.GetName() << "\")";
	File << "\n";
	File << "\n";
}

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

	std::ofstream File(Path.c_str(), std::ios::out);
	File << GGeneratedFileHeader;
	File << "\n\n";
	File << GGeneratedHeaderIncludes;
	File << "#undef CURRENT_FILE_UNIQUE_ID\n";
	CurrentFileUniqueId = InHeader.GetModule() + "_" + std::to_string(UniqueHeaderID);
	File << "#define CURRENT_FILE_UNIQUE_ID " << CurrentFileUniqueId << "\n\n";

	for(const auto& Struct : InHeader.GetStructs())
		ProcessHeaderStructOrClass(File, Struct, false);

	for (const auto& Class : InHeader.GetClasses())
		ProcessHeaderStructOrClass(File, Class, true);

	File.close();
}

void ProcessProperty(std::ofstream& File, const std::string& InType, 
	const CStruct& InStruct, const SProperty& InProperty)
{
	std::string BuilderName = "Builder_" + InStruct.GetName();

	File << "\n\t" << BuilderName << ".Property(\"" << InProperty.Name << "\", &" << InType << "::" << InProperty.Name
		<< ");";
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
	File << "\n";

	/** Static ptrs */
	File << "/** Static variables used for GetStaticStruct()/GetStaticClass() */\n";
	{
		for (const auto& Struct : InHeader.GetStructs())
		{
			File << "static ZE::Refl::CStruct* StaticStruct_" << Struct.GetName() << " = nullptr;\n";
		}

		for (const auto& Class : InHeader.GetClasses())
		{
			File << "static ZE::Refl::CClass* StaticClass_" << Class.GetName() << " = nullptr;\n";
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
		File << "}\n";
		File << "}\n";
	}

	/** GetStaticXXXXX */
	{
		for(const auto& Struct : InHeader.GetStructs())
		{
			std::string Type = GetObjectType(Struct.GetNamespace(), Struct.GetName());

			File << "ZE::Refl::CStruct* " << Type << "::GetStaticStruct()\n{\n";
			File << "\treturn StaticStruct_" << Struct.GetName() << ";\n";
			File << "}\n\n";
		}

		for (const auto& Class : InHeader.GetClasses())
		{
			std::string Type = GetObjectType(Class.GetNamespace(), Class.GetName());

			File << "ZE::Refl::CClass* " << Type << "::GetStaticClass()\n{\n";
			File << "\treturn StaticClass_" << Class.GetName() << ";\n";
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