add_executable(coro main.cc "generator.h")

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(coro PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
