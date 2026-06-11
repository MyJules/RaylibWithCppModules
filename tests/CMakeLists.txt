include(FetchContent)

FetchContent_Declare(
	googletest
	SOURCE_DIR ${PROJECT_SOURCE_DIR}/vendor/googletest
)
FetchContent_MakeAvailable(googletest)

add_executable(unit_tests
	test_args.cpp
)

set(_gtest_main_target "")
if(TARGET GTest::gtest_main)
	set(_gtest_main_target GTest::gtest_main)
elseif(TARGET gtest_main)
	set(_gtest_main_target gtest_main)
endif()

if(NOT _gtest_main_target)
	message(FATAL_ERROR "Could not find gtest_main target (neither GTest::gtest_main nor gtest_main).")
endif()

target_link_libraries(unit_tests PRIVATE args ${_gtest_main_target})

include(GoogleTest)
gtest_discover_tests(unit_tests)
