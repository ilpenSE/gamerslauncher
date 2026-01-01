cmake_minimum_required(VERSION 3.16)

project(GamersLauncher VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# ===== QT PACKAGES =====
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Widgets
    WidgetsTools
    LinguistTools
    Network
)

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
    src/ui/logindialog.h src/ui/logindialog.cpp src/ui/logindialog.ui
    src/mgr/download.hpp src/mgr/download.cpp
)

# ===== TRANSLATION TOOLS =====
# ===== FUCK EVERY QT "DEVELOPER" =====
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
    QM_FILES_OUTPUT_VARIABLE QM_FILES
)

# whatever the fuck is this but it works DONT TOUCH IT
foreach(qm ${QM_FILES})
    get_filename_component(qm_name ${qm} NAME)
    set_source_files_properties(${qm}
        PROPERTIES
            QT_RESOURCE_ALIAS ${qm_name}
    )
endforeach()

qt_add_resources(appGamersLauncher translations_qm
    PREFIX "/assets/translations"
    FILES ${QM_FILES}
)
# =========================

# ===== INCLUDES =====
target_include_directories(appGamersLauncher PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src

    # FUCK EVERY QT EMPLOYEE WHO MAKES THIS SHITTY AUTO UIC SYSTEM
    ${CMAKE_CURRENT_BINARY_DIR}/appGamersLauncher_autogen/include/src/ui/
)

# ===== QT LINK =====
target_link_libraries(appGamersLauncher
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Network
)

# ===== LOGGER LIB =====
if (WIN32)
    target_link_libraries(appGamersLauncher PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/win32/liblogger.dll.a
    )
elseif (UNIX)
    target_link_libraries(appGamersLauncher PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/linux/liblogger.so
    )
endif()

# ===== TARGET PROPERTIES =====
set_target_properties(appGamersLauncher PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# ===== COMPILER FLAGS =====
target_compile_options(appGamersLauncher PRIVATE
    $<$<CONFIG:Debug>:-g;-O0>
)

# ===== INSTALL =====
include(GNUInstallDirs)
install(TARGETS appGamersLauncher
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
