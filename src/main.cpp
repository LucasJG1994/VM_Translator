#include "scanner.h"
#include "file_writer.h"

#include <iostream>

extern "C" {
#include "vmth.tab.h"
#include <stdio.h>
}

int main(int argc, char** argv) {
	//if(argc == 2){
		FILE* fp;
		if (fopen_s(&fp, "test/Sys.vm", "rb") != 0) {
			std::cout << "Failed to read file...\n";
			return EXIT_FAILURE;
		}

		fseek(fp, 0L, SEEK_END);
		long len = ftell(fp);
		fseek(fp, 0L, SEEK_SET);

		char* buffer = new char[len + 1];
		if (buffer == nullptr) {
			std::cout << "Failed to allocate memory...\n";
			return EXIT_FAILURE;
		}

		fread(buffer, sizeof(char), len, fp);
		fclose(fp);

		buffer[len] = 0;

		std::cout << buffer << std::endl;

		fw_init();

		yy_init(buffer);
		yyparse();

		fw_dump();

		delete[] buffer;
	//}
	//else {
	//	std::cout << "Expected a single .VM file...\n";
	//	std::cout << "Usage:\n";
	//	std::cout << "vmth <file_name>.vm\n";
	//}
	return 0;
}