#include "ZERT.h"
#include <stack>
#include "Header.h"

struct Header;

class Parser
{
public:
	Parser(Header& in_header, const std::string& in_text, const bool in_fast_parsing);
private:
	void parse_class(const bool in_struct);
	void parse_enum();
	std::string parse_ctor(const std::string& in_ctor);
private:
	Header& header;
	bool fast_parsing;
	std::string file;
	size_t cursor;
	std::string current_namespace;
	std::stack<std::string> namespace_stack;
};