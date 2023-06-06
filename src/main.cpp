#include "scanner.h"
#include "file_writer.h"

#include <iostream>

extern "C" {
#include "vmth.tab.h"
#include <stdio.h>
}

int main(int argc, char** argv) {
	if(argc == 2){
		yy_init(argv[1]);
	}
	else {
		std::cout << "Usage:\n";
		std::cout << "vmth <path>\n";
	}
	return 0;
}