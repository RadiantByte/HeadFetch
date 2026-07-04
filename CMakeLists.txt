cmake_minimum_required(VERSION 3.10)
project(headfetch LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

include(FetchContent)
FetchContent_Declare(libhat
    GIT_REPOSITORY https://github.com/BasedInc/libhat.git
    EXCLUDE_FROM_ALL
)
FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.1
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(libhat imgui)

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_ES2)
target_compile_options(imgui PRIVATE -Os -ffunction-sections -fdata-sections -fvisibility=hidden)
target_link_libraries(imgui PUBLIC GLESv2)

add_library(headfetch SHARED
    src/main.cpp
)
target_include_directories(headfetch PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_options(headfetch PRIVATE -Os -ffunction-sections -fdata-sections)
target_link_libraries(headfetch PUBLIC libhat::libhat imgui GLESv2 android EGL log)
target_link_options(headfetch PRIVATE
    -static-libstdc++
    -Wl,--gc-sections
    -Wl,--exclude-libs,ALL
    -Wl,-s
)
set_target_properties(headfetch PROPERTIES OUTPUT_NAME headfetch)
install(TARGETS headfetch LIBRARY DESTINATION mods)
