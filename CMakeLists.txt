cmake_minimum_required(VERSION 3.21)
project(scanner VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Настройка выходных директорий
if(WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

    foreach(cfg Debug Release RelWithDebInfo MinSizeRel)
        string(TOUPPER "${cfg}" CFG)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CFG} ${CMAKE_BINARY_DIR}/bin/${cfg})
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CFG} ${CMAKE_BINARY_DIR}/bin/${cfg})
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CFG} ${CMAKE_BINARY_DIR}/lib/${cfg})
    endforeach()
endif()

# Опции проекта
option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_CLI "Build command line interface" ON)

# Поиск зависимостей
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

# Добавление поддиректорий
add_subdirectory(scanner)

if(BUILD_CLI)
    add_subdirectory(scannerCli)
endif()

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

# Установка
include(GNUInstallDirs)

install(TARGETS scanner
    EXPORT scanner-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT scanner-targets
    FILE scanner-config.cmake
    NAMESPACE Scanner::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/scanner
)