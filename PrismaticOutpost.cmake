project(PristmaticOutpost VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets)
if (NOT Qt6_FOUND)
  find_package(Qt5 5.15 REQUIRED COMPONENTS Widgets)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "(Clang|GNU)")
  add_compile_options(-Wall -Wpedantic -Wextra -Werror)
  add_compile_options(-fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set_property(DIRECTORY APPEND PROPERTY COMPILE_OPTIONS "/w")
endif()

add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0xFFFFFF)

add_executable(PristmaticOutpost
  databasemanager.cpp
  itemwindow.cpp
  main.cpp
  menuwindow.cpp
  prismaticoutpost.cpp
  scripteditor.cpp
  toolwindow.cpp
)

target_link_libraries(PristmaticOutpost Qt::Widgets)
