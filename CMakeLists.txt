cmake_minimum_required(VERSION 3.10)
project(Lotus-DIR VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt5 components
find_package(Qt5 REQUIRED COMPONENTS Widgets)

# Define executable
add_executable(lotus-dir
    src/main.cpp
    src/mainwindow.cpp
    src/sidebar.cpp
    src/filemodel.cpp
    resources/icons.qrc
)

# Link Qt5 libraries
target_link_libraries(lotus-dir Qt5::Widgets)

# Installation directories
install(TARGETS lotus-dir DESTINATION bin)
install(DIRECTORY resources/ DESTINATION share/lotus-dir)

# Desktop entry
install(FILES resources/lotus.desktop DESTINATION share/applications)

# Create config directory
install(CODE "execute_process(COMMAND mkdir -p \$ENV{DESTDIR}/etc/xdg/Lotus-DIR)")
