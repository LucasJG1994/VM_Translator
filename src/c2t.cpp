#include "c2t.h"

#include <string>

/*
	code: https://cplusplus.com/forum/general/12245/#:~:text=std%3A%3Astring%20str%3D%22something%22%3B%20TCHAR%20%2Aparam%3Dnew%20TCHAR%20%5Bstr.size%20%28%29%2B1%5D%3B%20param,may%20not%20be%20true%3A%20std%3A%3Acopy%20%28str.begin%20%28%29%2Cstr.end%20%28%29%2Cparam%29%3B
*/

TCHAR* char_to_t(const char* str) {
	std::string tmp = std::string(str);
	TCHAR* self = new TCHAR[tmp.length() + 1];
	if(self == nullptr) return nullptr;

	self[tmp.length()] = 0;
	std::copy(tmp.begin(), tmp.end(), self);
	return self;
}