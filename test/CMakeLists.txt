add_executable(thst_test
	main.cpp
)

target_link_libraries(thst_test
	THST::THST
)

set_target_properties(thst_test
	PROPERTIES
		CXX_STANDARD 11
		CXX_STANDARD_REQUIRED ON
)

find_package(doctest REQUIRED)

add_executable(test
	test_quadtree.cpp
)
add_executable(THST::test ALIAS test)

target_link_libraries(test
	PRIVATE
		doctest::doctest
		THST::THST
)

set_target_properties(test
	PROPERTIES
		CXX_STANDARD 11
		CXX_STANDARD_REQUIRED ON
)

target_compile_definitions(test
	PRIVATE
		DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
)

