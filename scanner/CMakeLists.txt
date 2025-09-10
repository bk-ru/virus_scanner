set(SCANNER_SOURCES
    hashDatabase.cpp
    hashDatabase.h
    logger.cpp
    logger.h
    md5Calc.cpp
    md5Calc.h
    scanner.cpp
    scanner.h
    scannerApi.h
    threadPool.cpp
    threadPool.h
    utils.cpp
    utils.h
)

find_package(OpenSSL REQUIRED)
add_library(scanner SHARED ${SCANNER_SOURCES})
target_compile_definitions(scanner PRIVATE SCANNER_DLL_EXPORTS)
target_link_libraries(scanner PRIVATE OpenSSL::SSL OpenSSL::Crypto)

if(WIN32)
    target_link_libraries(scanner PRIVATE ws2_32 wsock32)
endif()

if(UNIX)
    target_link_libraries(scanner PRIVATE pthread)
endif()

target_include_directories(scanner PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)