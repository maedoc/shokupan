cmake_minimum_required(VERSION 3.16)
project(imap-kanban VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Network Widgets)

# Enable static linking for release builds
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    set(BUILD_SHARED_LIBS OFF)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
endif()

# Include directories
include_directories(src)

# Core library
set(CORE_SOURCES
    src/core/imap_client.cpp
    src/core/email_card.cpp
    src/core/mailbox_list.cpp
    src/core/settings.cpp
    src/core/kanban_model.cpp
)

set(CORE_HEADERS
    src/core/imap_client.h
    src/core/email_card.h
    src/core/mailbox_list.h
    src/core/settings.h
    src/core/kanban_model.h
)

add_library(imap-kanban-core STATIC ${CORE_SOURCES} ${CORE_HEADERS})
target_link_libraries(imap-kanban-core Qt6::Core Qt6::Network)

# CLI application
set(CLI_SOURCES
    src/cli/main.cpp
    src/cli/cli_application.cpp
)

set(CLI_HEADERS
    src/cli/cli_application.h
)

add_executable(imap-kanban-cli ${CLI_SOURCES} ${CLI_HEADERS})
target_link_libraries(imap-kanban-cli imap-kanban-core Qt6::Core Qt6::Network)

# GUI application
set(GUI_SOURCES
    src/gui/main.cpp
    src/gui/main_window.cpp
    src/gui/kanban_board.cpp
    src/gui/card_widget.cpp
    src/gui/settings_dialog.cpp
    src/gui/card_dialog.cpp
)

set(GUI_HEADERS
    src/gui/main_window.h
    src/gui/kanban_board.h
    src/gui/card_widget.h
    src/gui/settings_dialog.h
    src/gui/card_dialog.h
)

add_executable(imap-kanban-gui ${GUI_SOURCES} ${GUI_HEADERS})
target_link_libraries(imap-kanban-gui imap-kanban-core Qt6::Core Qt6::Network Qt6::Widgets)

# Enable Qt MOC for GUI headers
qt6_standard_project_setup()

# Set up MOC for headers with Q_OBJECT
set_target_properties(imap-kanban-core PROPERTIES
    AUTOMOC ON
)

set_target_properties(imap-kanban-cli PROPERTIES
    AUTOMOC ON
)

set_target_properties(imap-kanban-gui PROPERTIES
    AUTOMOC ON
)

# Platform-specific settings
if(WIN32)
    set_target_properties(imap-kanban-gui PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

# Install targets
install(TARGETS imap-kanban-cli imap-kanban-gui
    RUNTIME DESTINATION bin
)

# CPack configuration for packaging
set(CPACK_PACKAGE_NAME "imap-kanban")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "IMAP-based Kanban application")
set(CPACK_PACKAGE_VENDOR "IMAP Kanban Project")

if(WIN32)
    set(CPACK_GENERATOR "ZIP")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
else()
    set(CPACK_GENERATOR "TGZ")
endif()

include(CPack)