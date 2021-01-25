find_package(udev REQUIRED)

add_executable(device-service)
target_sources(device-service PRIVATE main.cc)
target_link_libraries(device-service PRIVATE settings udev::udev)

add_subdirectory(test)
