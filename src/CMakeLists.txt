add_library(coro)
file(GLOB_RECURSE SOURCE_FILES *.cc *.h)
target_include_directories(coro
	PUBLIC
	$<INSTALL_INTERFACE:include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_sources(coro PRIVATE ${SOURCE_FILES})

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(coro PRIVATE spdlog::spdlog)

install(TARGETS coro)
