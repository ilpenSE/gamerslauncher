cmake_minimum_required(VERSION 3.16)

project(GamersLauncher VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)
find_package(Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appGamersLauncher
    src/main.cpp
)

qt_add_qml_module(appGamersLauncher
    URI GamersLauncher
    VERSION 1.0
    QML_FILES
        qml/Main.qml
    RESOURCES
        # ===== CLASSES WITH CPP =====
        SOURCES src/mgr/appdata.hpp src/mgr/appdata.cpp
        SOURCES src/mgr/settings.hpp src/mgr/settings.cpp

        # ===== CLASSES WITHOUT CPP =====
        SOURCES src/mgr/version.hpp

        # ===== ETC FILES =====
        SOURCES src/etc/instances.hpp
        SOURCES src/etc/utils.hpp
        SOURCES src/etc/enums.hpp
        SOURCES src/etc/result.hpp
        SOURCES src/etc/string.hpp

        # ===== UTIL FILES =====
        SOURCES src/util/jsonvalidator.hpp src/util/jsonvalidator.cpp
        SOURCES src/util/jsonworker.hpp src/util/jsonworker.cpp
        SOURCES
        SOURCES src/util/jsonstructs.hpp
)

# ===== INCLUDES =====
target_include_directories(appGamersLauncher PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# ===== DYNAMIC LIBRARY LINKS =====
if (WIN32)
    target_link_libraries(appGamersLauncher PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/win32/liblogger.dll.a
    )
elseif (UNIX)
    target_link_libraries(appGamersLauncher PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/linux/logger
    )
endif()

# ===== QT LINK =====
target_link_libraries(appGamersLauncher
    PRIVATE Qt6::Quick
)
target_link_libraries(appGamersLauncher PRIVATE Qt6::Core)

set_target_properties(appGamersLauncher PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS appGamersLauncher
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
