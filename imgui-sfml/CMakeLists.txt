cmake_minimum_required(VERSION 3.10)
project(BuildImGuiSFML CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# option(BUILD_DYNAMIC "Build dynamic library" OFF)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(OUTPUT_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")
file(GLOB OBJECT_SOURCE "src/*.cpp")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(LIBRARY_OUTPUT_NAME)

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

if(BUILD_DYNAMIC)
    add_library(imgui-sfml SHARED ${OBJECT_SOURCE})
    target_include_directories(imgui-sfml PUBLIC ${INCLUDE_DIR})
    target_link_libraries(imgui-sfml PUBLIC sfml-system sfml-network sfml-window sfml-audio sfml-main sfml-graphics opengl32 glu32 glfw3 imm32)
    target_compile_definitions(imgui-sfml PRIVATE IMGUI_SFML_SHARED_LIB=1)
    target_compile_definitions(imgui-sfml PRIVATE IMGUI_SFML_EXPORTS)
    target_compile_options(imgui-sfml PRIVATE "-Os" "-march=znver4" "-s")
else()
    add_library(imgui-sfml STATIC ${OBJECT_SOURCE})
    target_include_directories(imgui-sfml PUBLIC ${INCLUDE_DIR})
    target_link_libraries(imgui-sfml PUBLIC sfml-system sfml-network sfml-window sfml-audio sfml-main sfml-graphics opengl32 glu32 glfw3 imm32)
    target_compile_options(imgui-sfml PRIVATE "-Os" "-march=nehalem" "-s")
endif()

set_target_properties(imgui-sfml PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_LIB_DIR}"
)

# Do not install the library system-wide
install(TARGETS imgui-sfml EXPORT MyTarget
    ARCHIVE DESTINATION "${OUTPUT_LIB_DIR}"
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)