cmake_minimum_required(VERSION 3.5)

project(UltraCurlQT VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(CMAKE_AUTOUIC_SEARCH_PATHS assets/)

set(PROJECT_SOURCES
        src/main.cpp

        include/mainwindow.h
        src/mainwindow.cpp

		assets/mainwindow.ui
)

set(UI assets/mainwindow.ui)

qt5_wrap_ui(UI_HEADERS ${UI})

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(UltraCurlQT
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}

    )
else()
	add_executable(UltraCurlQT ${PROJECT_SOURCES} )
endif()

add_subdirectory(lib/UltraCurl/)

set_property(TARGET UltraCurlQT PROPERTY AUTOGEN ON)
set_property(TARGET UltraCurlQT PROPERTY AUTOMOC ON)
set_property(TARGET UltraCurlQT PROPERTY AUTORCC ON)
set_property(TARGET UltraCurlQT PROPERTY AUTOUIC ON)


set_target_properties(UltraCurlQT PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(UltraCurlQT)
endif()

target_link_libraries(UltraCurlQT 
	PRIVATE Qt5::Widgets
	ultra_curl)

target_include_directories(
	UltraCurlQT PRIVATE 
	"include/" "assets/"
	<AUTOGEN_BUILD_DIR>/include
)
# Adding UI for application


# Making all warnings errors and turning on pedantic feature in GCC
if(MSVC)
	target_compile_options(UltraCurlQT
		PRIVATE "/W4" "/WX")
else()
	target_compile_options(UltraCurlQT
		PRIVATE "-Wall" "-Wextra" "-Wpedantic" "-Werror")
endif()

# Command to create symlink for clangd to see all includes
if(CMAKE_BUILD_TYPE MATCHES Debug)
	set_target_properties(${PROJECT_NAME} PROPERTIES EXPORT_COMPILE_COMMANDS true)

	execute_process(
		COMMAND ${CMAKE_COMMAND} -E create_symlink
			${CMAKE_BINARY_DIR}/compile_commands.json
			${CMAKE_SOURCE_DIR}/compile_commands.json
	)
endif()
