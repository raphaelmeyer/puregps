add_executable(device-service-test)

target_sources(device-service-test PRIVATE test.cc)
target_link_libraries(device-service-test PRIVATE doctest::doctest)

add_test(NAME "Device Service" COMMAND device-service-test)
