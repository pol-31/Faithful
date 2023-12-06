
add_compile_options(
        -Wall
        -Wno-deprecated
        -Wno-deprecated-declarations
        -Wno-sign-compare
        -Wno-unused
        -Wuninitialized
        -Wunused-result
)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_compile_options(-g)
else()
    add_compile_options(-O3 -flto)
endif()
