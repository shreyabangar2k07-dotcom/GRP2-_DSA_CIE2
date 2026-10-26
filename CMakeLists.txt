cmake_minimum_required(VERSION 3.16)
project(SecurePasswordSimulator LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SODIUM REQUIRED IMPORTED_TARGET libsodium)
add_executable(secure_password_gui
    gui/GuiMain.cpp
    gui/MainWindow.cpp
    gui/MainWindow.h
)
target_link_libraries(secure_password_gui PRIVATE Qt6::Widgets PkgConfig::SODIUM)
# Keep user records in a predictable project-local location when launched from IDE/build folder.
set_target_properties(secure_password_gui PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
