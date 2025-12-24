cmake_minimum_required(VERSION 3.16)

project(GamersLauncher VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ===== QT PACKAGES =====
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Widgets
    LinguistTools
)

qt_standard_project_setup(REQUIRES 6.8)


# ===== TRANSLATIONS =====
set(TS_FILES
    assets/translations/tr.ts
    assets/translations/en.ts
    assets/translations/de.ts
)

# ===== EXECUTABLE =====
qt_add_executable(appGamersLauncher
    src/main.cpp

    # ===== UI =====
    src/ui/mainwindow.h
    src/ui/mainwindow.cpp
    src/ui/mainwindow.ui

    # ===== MANAGERS =====
    src/mgr/appdata.hpp src/mgr/appdata.cpp
    src/mgr/settings.hpp src/mgr/settings.cpp
    src/mgr/language.hpp src/mgr/language.cpp

    # ===== HEADER-ONLY =====
    src/mgr/version.hpp
    src/etc/instances.hpp
    src/etc/utils.hpp
    src/etc/enums.hpp
    src/etc/result.hpp
    src/etc/string.hpp

    # ===== UTIL =====
    src/util/jsonvalidator.hpp src/util/jsonvalidator.cpp
    src/util/jsonworker.hpp src/util/jsonworker.cpp
    src/util/jsonstructs.hpp
)

# ===== TRANSLATION TOOLS =====
qt_add_lupdate(appGamersLauncher
    TS_FILES ${TS_FILES}
    SOURCES
        src/main.cpp
        src/ui
        src/mgr
        src/util
)

qt_add_lrelease(appGamersLauncher
    TS_FILES ${TS_FILES}
)

# ===== INCLUDES =====
target_include_directories(appGamersLauncher PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# ===== QT LINK =====
target_link_libraries(appGamersLauncher
    PRIVATE
        Qt6::Core
        Qt6::Widgets
)

# ===== LOGGER LIB =====
if (WIN32)
    target_link_libraries(appGamersLauncher PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/win32/liblogger.dll.a
    )
elseif (UNIX)
    target_link_libraries(appGamersLauncher PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/linux/logger
    )
endif()

# ===== TARGET PROPERTIES =====
set_target_properties(appGamersLauncher PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# ===== INSTALL =====
include(GNUInstallDirs)
install(TARGETS appGamersLauncher
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
