add_executable(coro_test task_test.cpp)

file(GLOB_RECURSE SOURCE_FILES *.cc)
target_sources(coro_test PRIVATE ${SOURCE_FILES})

find_package(spdlog CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
target_link_libraries(coro_test
	PRIVATE
	coro
	spdlog::spdlog
	GTest::gmock_main
	GTest::gtest_main)
