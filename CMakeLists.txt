cmake_minimum_required(VERSION 2.8)

project(lv2lint.lv2)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_BINARY_DIR})

set(CMAKE_C_FLAGS "-std=gnu11 -Wextra -Wno-unused-parameter -ffast-math -fvisibility=hidden ${CMAKE_C_FLAGS}")
set(CMAKE_C_FLAGS "-Wshadow -Wimplicit-function-declaration -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes ${CMAKE_C_FLAGS}")
add_definitions("-D_GNU_SOURCE=1") # asprintf

file(STRINGS "VERSION" LV2LINT_VERSION)
string(REPLACE "." ";" VERSION_LIST ${LV2LINT_VERSION})
list(GET VERSION_LIST 0 LV2LINT_MAJOR_VERSION)
list(GET VERSION_LIST 1 LV2LINT_MINOR_VERSION)
list(GET VERSION_LIST 2 LV2LINT_MICRO_VERSION)
add_definitions("-DLV2LINT_VERSION=\"${LV2LINT_VERSION}\"")

find_package(PkgConfig) # ${PKG_CONFIG_FOUND}
include(FindUnixCommands)

set(LIBS m)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set(LIBS ${LIBS} dl)
endif()

pkg_search_module(LV2 REQUIRED lv2>=1.10)
include_directories(${LV2_INCLUDE_DIRS})
set(LIBS ${LIBS} ${LV2_LDFLAGS})

pkg_search_module(LILV REQUIRED lilv-0>=0.20.0)
include_directories(${LILV_INCLUDE_DIRS})
if(DEFINED STATIC_LILV)
	set(LIBS ${STATIC_LILV} ${STATIC_SRATOM} ${STATIC_SORD} ${STATIC_SERD} ${LIBS})
else()
	set(LIBS ${LIBS} ${LILV_LDFLAGS})
endif()
if((${LILV_VERSION} VERSION_EQUAL "0.22.0") OR (${LILV_VERSION} VERSION_GREATER "0.22.0"))
	add_definitions("-DLILV_0_22")
endif()

option(ENABLE_ONLINE_TESTS "Enable online tests (e.g. tool will try to connect to the internet)" ON)
if(ENABLE_ONLINE_TESTS)
	add_definitions("-DENABLE_ONLINE_TESTS")

	pkg_search_module(CURL REQUIRED libcurl>=7.0.0)
	include_directories(${CURL_INCLUDE_DIRS})
	set(LIBS ${LIBS} ${CURL_LDFLAGS})
endif()

if(NOT WIN32)
	option(ENABLE_ELF_TESTS "Enable ELF tests (e.g. tool will check for symbol visibility)" ON)
	if(ENABLE_ELF_TESTS)
		add_definitions("-DENABLE_ELF_TESTS")

		pkg_search_module(ELF REQUIRED libelf>=0.170)
		include_directories(${ELF_INCLUDE_DIRS})
		set(LIBS ${LIBS} ${ELF_LDFLAGS})
	endif()
endif()

add_executable(lv2lint
	lv2lint.c
	lv2lint_plugin.c
	lv2lint_port.c
	lv2lint_parameter.c
	lv2lint_ui.c)
set_target_properties(lv2lint PROPERTIES INTERPROCEDURAL_OPTIMIZATION true) # -flto
target_link_libraries(lv2lint ${LIBS})
install(TARGETS lv2lint DESTINATION bin)

add_custom_command(
	OUTPUT ${PROJECT_BINARY_DIR}/lv2lint.1.gz
	COMMAND ${GZIP} ARGS
	"-c9" ${CMAKE_CURRENT_SOURCE_DIR}/lv2lint.1
		">" ${PROJECT_BINARY_DIR}/lv2lint.1.gz
	DEPENDS
		${CMAKE_CURRENT_SOURCE_DIR}/lv2lint.1)
add_custom_target(LV2LINT_MAN_GZ ALL DEPENDS ${PROJECT_BINARY_DIR}/lv2lint.1.gz)
install(FILES "${PROJECT_BINARY_DIR}/lv2lint.1.gz" DESTINATION share/man/man1)

include(CTest)

if(${BUILD_TESTING})
	if(NOT WIN32)
		add_test(NAME LV2-Lint COMMAND lv2lint "http://open-music-kontrollers.ch/lv2/moony#a1xa1")
	else()
		add_test(NAME LV2-Lint COMMAND ${WINE} lv2lint "http://open-music-kontrollers.ch/lv2/moony#a1xa1")
	endif()
endif()
