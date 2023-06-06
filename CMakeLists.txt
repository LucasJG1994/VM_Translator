cmake_minimum_required(VERSION 2.8.11)
project(vmth)

include_directories(
	src/parser
	src
)

add_executable(vmth
	${vmth_SOURCE_DIR}/src/main.cpp
	${vmth_SOURCE_DIR}/src/dirWin.c
	${vmth_SOURCE_DIR}/src/c2t.cpp
	${vmth_SOURCE_DIR}/src/parser/vmth.tab.c
	${vmth_SOURCE_DIR}/src/parser/scanner.cpp
	${vmth_SOURCE_DIR}/src/parser/file_writer.cpp
	${vmth_SOURCE_DIR}/src/parser/vmth.tab.c
	
	${vmth_SOURCE_DIR}/src/c2t.h
	${vmth_SOURCE_DIR}/src/dir.h
	${vmth_SOURCE_DIR}/src/parser/file_writer.h
	${vmth_SOURCE_DIR}/src/parser/vmth.tab.h
	${vmth_SOURCE_DIR}/src/parser/scanner.h
	${vmth_SOURCE_DIR}/src/parser/vmth.tab.h
	
	${vmth_SOURCE_DIR}/src/parser/vmth.y
)