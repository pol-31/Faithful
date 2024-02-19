
add_library(miniz STATIC miniz.c miniz.h)
target_compile_definitions(miniz PUBLIC
        MINIZ_NO_ZLIB_APIS
        MINIZ_NO_TIME
)
