set(target example_glem)

# Add sources
file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# Buid DESKTOP target
add_executable(${target}
    ${SOURCES}
    ${PROJECT_SOURCE_DIR}/src/gmg12864lib.c
    ${PROJECT_SOURCE_DIR}/src/dev/glem_dev.c
)

target_include_directories(${target}
    PRIVATE
    ${PROJECT_SOURCE_DIR}
)

target_link_libraries(${target} glem)
