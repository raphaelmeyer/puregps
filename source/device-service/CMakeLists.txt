find_package(libusb REQUIRED)

add_executable(device-service)
target_sources(device-service PRIVATE main.cc)
target_link_libraries(device-service PRIVATE settings)

add_subdirectory(test)
