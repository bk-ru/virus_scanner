add_executable(scanner_cli
    main.cpp
    config.cpp
    lineParser.cpp
)

target_include_directories(scanner_cli
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../libscanner/include
)

target_link_libraries(scanner_cli
    PRIVATE 
        scanner
)

target_compile_features(scanner_cli PRIVATE cxx_std_20)

install(TARGETS scanner_cli
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)