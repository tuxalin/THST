cmake_minimum_required(VERSION 3.5..3.18)

# version range does not set policies until version 3.12
if(${CMAKE_VERSION} VERSION_LESS 3.12)
	cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(THST
	DESCRIPTION "Templated hierarchical spatial trees designed for high performance and hierarchical spatial partitioning use cases."
	LANGUAGES CXX
	VERSION 0.1.0
	HOMEPAGE_URL "https://github.com/tuxalin/THST"
)

add_library(THST INTERFACE)
add_library(THST::THST ALIAS THST)

target_include_directories(THST
	INTERFACE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

if(CMAKE_VERSION VERSION_GREATER 3.7)
	target_compile_features(THST INTERFACE cxx_std_11)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	add_subdirectory(test)
endif()

include(GNUInstallDirs)
install(
	TARGETS THST
	EXPORT THSTTargets
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
	"THSTConfigVersion.cmake"
	VERSION "${PROJECT_VERSION}"
	COMPATIBILITY SameMajorVersion
)
configure_package_config_file(
	"${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
	"${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
	INSTALL_DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake"
)

configure_file(
	"${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}.pc.in"
	"${PROJECT_BINARY_DIR}/${PROJECT_NAME}.pc"
	@ONLY
)

export(
	EXPORT ${PROJECT_NAME}Targets
	FILE ${PROJECT_NAME}Targets.cmake
	NAMESPACE THST::
)

export(PACKAGE ${PROJECT_NAME})

install(
	EXPORT ${PROJECT_NAME}Targets
	FILE ${PROJECT_NAME}Targets.cmake
	NAMESPACE THST::
	DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake
)

install(
	FILES
		"${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
		"${PROJECT_BINARY_DIR}/THSTConfigVersion.cmake"
	DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake
)

install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}.pc"
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

install(
	DIRECTORY ${PROJECT_SOURCE_DIR}/include/THST
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
