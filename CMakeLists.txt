set (CMAKE_C_STANDARD 99)
set (CMAKE_CXX_STANDARD 14)

add_library(
    coremark ${WEBROGUE_MOD_LIB_TYPE}

    coremark.c

    external/coremark/core_list_join.c
    # external/coremark/core_main.c
    external/coremark/core_matrix.c
    external/coremark/core_state.c
    external/coremark/core_util.c

    external/coremark/posix/core_portme.c
)

target_include_directories(
    coremark PRIVATE 
    external/coremark
    external/coremark/posix
)

target_compile_options(
    coremark PRIVATE
    -DCOMPILER_FLAGS=""
)

register_mod(
    NAME coremark
)
