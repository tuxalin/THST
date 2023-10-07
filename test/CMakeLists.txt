add_subdirectory(doctest)

add_executable(test_quadtree
	test_quadtree.cpp
)
add_executable(THST::test_quadtree ALIAS test_quadtree)

target_link_libraries(test_quadtree
	PRIVATE
		doctest::doctest
		THST::THST
)

set_target_properties(test_quadtree
	PROPERTIES
		CXX_STANDARD 11
		CXX_STANDARD_REQUIRED ON
)

target_compile_definitions(test_quadtree
	PRIVATE
		DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
)

##

add_executable(test_rtree
	test_rtree.cpp
)
add_executable(THST::test_rtree ALIAS test_rtree)

target_link_libraries(test_rtree
	PRIVATE
		doctest::doctest
		THST::THST
)

set_target_properties(test_rtree
	PROPERTIES
		CXX_STANDARD 11
		CXX_STANDARD_REQUIRED ON
)

target_compile_definitions(test_rtree
	PRIVATE
		DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
)