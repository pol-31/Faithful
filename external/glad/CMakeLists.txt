
add_library(glad STATIC ${FAITHFUL_SOURCE_DIR}/external/glad/src/glad.c)
set(FAITHFUL_GLAD_INCLUDE_DIR ${FAITHFUL_SOURCE_DIR}/external/glad/include)
target_include_directories(glad PRIVATE ${FAITHFUL_GLAD_INCLUDE_DIR})
