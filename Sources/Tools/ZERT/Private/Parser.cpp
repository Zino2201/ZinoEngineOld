#include "Parser.h"
#include "Class.h"
#include <fstream>
#include <sstream>
#include "Enum.h"
#include "Header.h"

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

std::string read_text_file(const std::string_view& InFilename)
{
	std::string output;
	output.reserve(100);

	std::string Line;
	std::ifstream File(InFilename.data());
	if (!File.is_open())
		fatal("Failed to read file %s", InFilename.data());

	while (!safeGetline(File, Line).eof())
	{
		output += "\n";
        output += Line;
	}

	File.close();

	return output;
}

std::vector<std::string> read_file_lines(const std::string_view& InFilename)
{
	std::vector<std::string> Lines;
	Lines.reserve(50);

	std::string Line;
	std::ifstream File(InFilename.data());
	if (!File.is_open())
		fatal("Failed to read file %s", InFilename.data());

	while (!safeGetline(File, Line).eof())
	{
		Lines.emplace_back(Line);
	}

	File.close();

	return Lines;
}

std::vector<std::string> tokenize(const std::string& InString, 
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

std::string sanitize_name(const std::string& in_name)
{
    std::string str = in_name;

    size_t comment_begin = str.find("/*");
    if(comment_begin != std::string::npos)
    {
        str = str.substr(str.find("*/") + 2);
    }

    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
        
    return str;
}

Parser::Parser(Header& in_header, const std::string& in_file,
    const bool in_fast_parsing) : header(in_header), fast_parsing(in_fast_parsing), file(in_file), cursor(0)
{
    /** Check if we should skip this file */
	if (in_file.find("#zert skip", 50) != std::string::npos)
    {
		return;
	}

    size_t end = in_file.size() - 1;
    
    enum ScopeType
    {
        Namespace,
        Scope,
    };

    std::stack<ScopeType> scope;

    while(cursor++ < end)
    {
        /** Check for strings beginning with Z */
        if(in_file[cursor] == 'Z')
        {
            if(in_file.substr(cursor, sizeof("ZCLASS")).find("ZCLASS") != std::string::npos)
            {
                parse_class(false);
            }
            else if(in_file.substr(cursor, sizeof("ZSTRUCT")).find("ZSTRUCT") != std::string::npos)
            {
                parse_class(true);
            }
            else if(in_file.substr(cursor, sizeof("ZENUM")).find("ZENUM") != std::string::npos)
            {
                parse_enum();
            }
        }
        else if(in_file[cursor] == '{')
        {
            scope.push(Scope);
        }
        else if(in_file[cursor] == '}')
        {
            if(!scope.empty())
            {
                ScopeType type = scope.top();

                if(type == Namespace)
                {
                    if(!namespace_stack.empty())
                    {
                        std::string ns = namespace_stack.top();
                        if(namespace_stack.size() > 1)
                        {
                            current_namespace.erase(current_namespace.find(ns) - sizeof("::"));
                        }
                        else
                        {
                            current_namespace.erase(current_namespace.find(ns));
                        }
                        namespace_stack.pop();
                    }
                }

                scope.pop();
            }
        }
        else if(in_file.substr(cursor, sizeof("namespace")).find("namespace") != std::string::npos)
        {
            size_t namespace_end = in_file.find('{', cursor); 
            std::string ns = in_file.substr(cursor + sizeof("namespace"), 
                  namespace_end - (cursor + sizeof("namespace")));

            /** Remove any spaces */
            ns.erase(std::remove_if(ns.begin(), ns.end(), isspace), ns.end());

            if(!current_namespace.empty())
                current_namespace += "::";
            current_namespace += ns;

            namespace_stack.push(ns);

            scope.push(Namespace);

            cursor = namespace_end;
        }
    }
}

/** https://stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string */
void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

void Parser::parse_class(const bool in_struct)
{
    /** Jump to the class declaration */
    if(in_struct)
        cursor = file.find("struct", cursor);
    else
        cursor = file.find("class", cursor);

    /** Get a substring of the whole class header */
    size_t class_body_start = file.find("{", cursor);

    std::string header = file.substr(cursor, class_body_start - cursor);
    std::vector<std::string> decl_tokens;
    size_t parent_begin = header.find(":");
    bool has_parents = parent_begin != std::string::npos;
    if(has_parents)
        decl_tokens = tokenize(header.substr(0, header.find(':')));
    else
        decl_tokens = tokenize(header);

    /** Get the class name */
    std::string name = decl_tokens[1];
    if(name.find("_API") != std::string::npos)
        name = decl_tokens[2];

    typedb_register(Type(ReflType::Class, name, current_namespace));

    /** Find class end */
    size_t fce_cursor = cursor + 4;
    size_t class_end = -1;
    size_t nested_encounters = 0;

    while(true)
    {
        if(file.substr(fce_cursor, 4).starts_with("enum"))
        {
            nested_encounters++;
        }
        else if(file.substr(fce_cursor, 2).starts_with("};"))
        {
            if(nested_encounters == 0)
            {
                class_end = fce_cursor;
                break;
            }
            else
            {
                nested_encounters--;
            }
        }

        fce_cursor++; 
    }

    /** If we are fast parsing, stop here */
    if(fast_parsing)
    {
        cursor = class_end;
        return;
    }

    /** Body */
    size_t body_refl_start = file.find("ZE_REFL_BODY", cursor);
    if(body_refl_start > class_end)
        fatal("Missing ZE_REFL_BODY() for class %s", name.c_str());
    std::string body_refl_substr = file.substr(0, body_refl_start);

    size_t line_count = std::count(body_refl_substr.begin(), body_refl_substr.end(), '\n');
  
    Class& cl = this->header.classes.emplace_back(in_struct, line_count, sanitize_name(name), current_namespace);

    /** Check if we find any documentation */
    if(size_t doc_end = file.substr(cursor - 15, 15).find("*/") != std::string::npos)
    {
        doc_end = (cursor - 15) + doc_end;

        size_t old_cursor_pos = cursor;

        size_t doc_begin = -1;
        while(true)
        {
            cursor--;

            if(cursor == 0)
                break;

            if(size_t possible_begin = file.substr(cursor - 2, 2).find("/*") != std::string::npos)
            {
                doc_begin = (cursor - 2) + possible_begin;
                break;
            }
        }

        if(doc_begin != -1)
        {
            //doc_begin += 0;
            doc_end += 2;
            cl.documentation = file.substr(doc_begin, doc_end - doc_begin);
            //cl.documentation.erase(std::remove(cl.documentation.begin(), cl.documentation.end(), '\n'), cl.documentation.end());
            replaceAll(cl.documentation, "\n", "\\n");
        }

        cursor = old_cursor_pos;
    }

    /** Parse parent classes */
    if(has_parents)
    {
        std::vector<std::string> parents = tokenize(header.substr(parent_begin + 1, class_body_start), ',');
        for(const auto& parent : parents)
        {
            size_t specifier = parent.find("public");
            size_t specifier_size = sizeof("public");
            if(specifier == std::string::npos)
            {
                specifier = parent.find("private");
                specifier_size = sizeof("private");
            }

            cl.add_parent(sanitize_name(parent.substr(specifier + specifier_size)));
        }
    }
    
    /** Parse ctor/propreties/functions */
    while(cursor++ < class_end)
    {
        /** Look for ctor */
        std::string ctor_name = name + "(";
        if (file.substr(cursor, ctor_name.size()).find(ctor_name) != std::string::npos &&
            file[cursor - 1] != '~')
        {
            size_t ctor_args_end = file.find(')', cursor);
            size_t ctor_function_end = file.find(';', cursor);
            if(file.substr(ctor_args_end, ctor_function_end - ctor_args_end).find("= delete") != std::string::npos)
                continue;
            
            std::string ctor_args = parse_ctor(file.substr(cursor + ctor_name.size(), ctor_args_end - (cursor + ctor_name.size())));
            cl.add_ctor(ctor_args);

            cursor = ctor_function_end;
        }

        /** Look for functions */
        else if(file.substr(cursor, sizeof("ZFUNCTION")).find("ZFUNCTION") != std::string::npos)
        {
            fatal("Reflected functions not yet supported");
        }
        /** Look for propreties */
        else if(file.substr(cursor, sizeof("ZPROPERTY")).find("ZPROPERTY") != std::string::npos)
        {
            size_t property_end = file.find(';', cursor);
            std::string property = file.substr(cursor, 
                property_end - cursor);
            
            size_t property_macro_args_start = file.find("(", cursor) + 1;
            size_t property_macro_args_end = file.find(")", cursor);
            std::vector<std::string> property_args = tokenize(file.substr(property_macro_args_start,
                property_macro_args_end - property_macro_args_start), ',');

            /** Parse type and name */
            std::string property_type_name = file.substr(property_macro_args_end + 1,
                property_end - (property_macro_args_end + 1));

            size_t name_start = property_type_name.find_last_of(' ') + 1;
            std::string property_type = property_type_name.substr(0, name_start);
            std::string property_name = property_type_name.substr(name_start);

            Property& prop = cl.add_property(property_type, sanitize_name(property_name));

            cursor = property_end;

            for(const auto& arg : property_args)
            {
                if(arg.find("=") != std::string::npos)
                {
                    std::string sanitized_arg = arg;
                    sanitized_arg.erase(std::remove_if(sanitized_arg.begin(), sanitized_arg.end(), isspace), sanitized_arg.end());
                    sanitized_arg.erase(std::remove(sanitized_arg.begin(), sanitized_arg.end(), '"'), sanitized_arg.end());
                    std::vector<std::string> value_args = tokenize(sanitized_arg, '=');
                    prop.metadatas.insert({ value_args[0], value_args[1] });
                }
                else
                {
                    std::string sanitized_arg = arg;
                    sanitized_arg.erase(std::remove_if(sanitized_arg.begin(), sanitized_arg.end(), isspace), sanitized_arg.end());
                    prop.metadatas.insert({ arg, "true" });
                }
            }
        }
    }
}

void Parser::parse_enum()
{
    size_t enum_end = file.find('};', cursor);

    /** Jump to the enum declaration */
    cursor = file.find("enum class", cursor);

    /** Get and register name */
    size_t enum_body_start = file.find("{", cursor);
    std::string header = file.substr(cursor, enum_body_start - cursor);

    std::string decl_without_type = header;
    size_t type_start = header.find(":");
    if(type_start != std::string::npos)
    {
        decl_without_type = header.substr(0, type_start);
    }
    size_t name_start = decl_without_type.find_last_of(' ') + 1;
    std::string name = decl_without_type.substr(name_start);

    Enum& enm = this->header.enums.emplace_back(sanitize_name(name), current_namespace);
    typedb_register(Type(ReflType::Enum, name, current_namespace));

    /** If we are fast parsing, stop here */
    if(fast_parsing)
    {
        cursor = enum_end;
        return;
    }

    /** Parse values */
    cursor = enum_body_start + 1;

    std::vector<std::string> tokens = tokenize(file.substr(cursor, (enum_end - 1) - cursor), ',');
    
    for(auto& token : tokens)
    {
        std::string sanitized_token = sanitize_name(token);

        bool unvalid = false;
        for(const auto& c : sanitized_token)
        {
            if(!std::isalnum(c))
            {
                unvalid = true;
                break;
            }
        }

        if(unvalid || sanitized_token.empty())
            continue;

        enm.add_value(sanitize_name(sanitized_token));
    }

    cursor = enum_end;
}

std::string Parser::parse_ctor(const std::string& in_ctor)
{
    std::vector<std::string> tokens = tokenize(in_ctor, ',');
    std::string ctor;

    for(const auto& token : tokens)
    {
        if(!ctor.empty())
            ctor += ",";

        std::vector<std::string> subtokens = tokenize(token);
        size_t i = 0;
        for(const auto& subtoken : subtokens)
        {
            if(i != subtokens.size() - 1)
                ctor += " " + subtoken;
            i++;
        }
    }

    return ctor;
}