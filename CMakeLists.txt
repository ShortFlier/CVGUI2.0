cmake_minimum_required(VERSION 3.16)

project(CVGUI2_0 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# OpenCV 4.8.0
set(OpenCV_DIR "D:\\gw\\opencv\\opencv\\build-qt-contrib\\install")
find_package(OpenCV REQUIRED)

# spdlog
set(spdlog_DIR "D:/gw/clib/spdlog/spdlog-1.15.1/bulid")
find_package(spdlog REQUIRED)

#ui文件类提升,选择全局包含
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/scene)


# 获取当前目录下的所有 .cpp .h 文件（递归子目录）
file(GLOB_RECURSE  PROJECT_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.ui"
)
file(GLOB_RECURSE  PROJECT_SOURCES_UI
    "${CMAKE_CURRENT_SOURCE_DIR}/*.ui"
)
file(GLOB_RECURSE  PROJECT_SOURCES_QRC
    "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc"
)
# 过滤掉 build 目录中的文件
list(FILTER PROJECT_SOURCES EXCLUDE REGEX ".*/build/.*")

# set(PROJECT_SOURCES
#         main.cpp
#         mainwindow.cpp
#         mainwindow.h
#         mainwindow.ui
# )

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(CVGUI2_0
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${PROJECT_SOURCES_UI}
        ${PROJECT_SOURCES_QRC}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET CVGUI2_0 APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(CVGUI2_0 SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(CVGUI2_0
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(CVGUI2_0 PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${OpenCV_LIBS} spdlog::spdlog)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.CVGUI2_0)
endif()
set_target_properties(CVGUI2_0 PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS CVGUI2_0
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(CVGUI2_0)
endif()
