if(BUILD_TESTS)
    enable_testing()
    
    find_package(GTest REQUIRED)
    
    add_executable(scanner_tests
    )
    
    target_include_directories(scanner_tests
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/../libscanner/include
    )
    
    target_link_libraries(scanner_tests
        PRIVATE
            scanner
            GTest::GTest
            GTest::Main
    )
    
    include(GoogleTest)
    gtest_discover_tests(scanner_tests)
endif()