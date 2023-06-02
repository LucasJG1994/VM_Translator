#include "scanner.h"

#include <iostream>
#include <map>
#include <string>

extern "C" {
#include "vmth.tab.h"
extern int line;
}

static const char* src;
static const char* cur;
static const char* begin;

static std::map<std::string, int> keywords = {
	{ "push"    ,      TK_PUSH },
	{ "pop"     ,       TK_POP },
	{ "argument",       TK_ARG },
	{ "local"   ,       TK_LCL },
	{ "static"  ,       TK_STC },
	{ "constant",       TK_CST },
	{ "this"    ,       TK_THS },
	{ "that"    ,       TK_THT },
	{ "pointer" ,       TK_PTR },
	{ "temp"    ,       TK_TMP },
	{ "add"     ,       TK_ADD },
	{ "sub"     ,       TK_SUB },
	{ "neg"     ,       TK_NEG },
	{ "eq"      ,        TK_EQ },
	{ "gt"      ,        TK_GT },
	{ "lt"      ,        TK_LT },
	{ "and"     ,       TK_AND },
	{ "or"      ,        TK_OR },
	{ "not"     ,       TK_NOT },
	{ "label"   ,     TK_LABEL },
	{ "goto"    ,      TK_GOTO },
	{ "if-goto" ,    TK_IFGOTO },
	{ "function",  TK_FUNCTION },
	{ "call"    ,      TK_CALL },
	{ "return"  ,    TK_RETURN }
};

static void adv() { cur++; }
static char peek() { return cur[1]; }
static bool match(char c) { return cur[0] == c; }
static bool end() { return cur[0] == 0; }

static std::string get_lex() {
	std::string self = std::string(begin, cur - begin);
	return self;
}

static bool is_num(std::string s) {
	for (char c : s) {
		if((c >= '0' && c <= '9') == 0) return false;
	}

	return true;
}

static bool is_id(std::string s) {
	return (s[0] >= 'a' && s[0] <= 'z') || (s[0] >= 'A' && s[0] <= 'Z') || s[0] == '_';
}

extern "C" void yy_init(const char* source) {
	src = source;
	cur = source;
	begin = source;
	line = 1;
}

extern "C" int yylex() {
	if(end()) return TK_EOF;

	while(1){
		switch (*cur) {
			case '/': 
				adv();
				if (match('/')) {
					while(match('\n') == 0) adv();
				}
				break;
			case ' ':
			case '\t': adv(); break;
			case '\r': adv(); break;
			case '\n': line++; adv(); break;
			default: {
				begin = cur;
				while(match(' ') == 0 && match('\t') == 0 && match('\n') == 0 && match('\r') == 0){
					if(end()) break;
					adv();
				}
				std::string lex = get_lex();

				std::cout << (char)34 << lex << (char)34 << std::endl;

				if (is_num(lex)){
					yylval.ival = std::atoi(lex.c_str());
					return TK_NUM;
				}

				if (keywords.find(lex) != keywords.end()) return keywords[lex];

				if(is_id(lex)){
					char* tmp = new char[lex.length() + 1];
					if(tmp == nullptr) {
						std::cout << "Failed to allocate memory for label...\n";
						return TK_EOF;
					}

					memcpy_s(tmp, lex.length(), lex.c_str(), lex.length());
					tmp[lex.length()] = 0;
					yylval.sval = tmp;
					return TK_ID;
				}

				return TK_EOF;
			}
		}
	}
}