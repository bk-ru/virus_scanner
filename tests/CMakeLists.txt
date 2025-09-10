include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP ON
)

if(WIN32)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

FetchContent_MakeAvailable(googletest)

include_directories(
    ${CMAKE_SOURCE_DIR}/scanner
    ${CMAKE_SOURCE_DIR}/scannerCli
)

set(TEST_SOURCES
    testMain.cpp
    tests.cpp
)

add_executable(unit_tests ${TEST_SOURCES})

target_link_libraries(unit_tests
    PRIVATE
    gtest
    gtest_main
    scanner
)

if(WIN32)
    add_custom_command(TARGET unit_tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:scanner>
        $<TARGET_FILE_DIR:unit_tests>
    )
endif()

include(GoogleTest)
gtest_discover_tests(unit_tests)