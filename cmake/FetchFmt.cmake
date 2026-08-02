include(FetchContent)

message(STATUS "Pulling and configuring fmt")

FetchContent_Declare(
        fmt
        GIT_REPOSITORY "https://github.com/fmtlib/fmt.git"
        GIT_TAG "10.2.1"
        GIT_PROGRESS TRUE
        GIT_SHALLOW TRUE
        OVERRIDE_FIND_PACKAGE
        EXCLUDE_FROM_ALL
)

set(FMT_DOC OFF CACHE INTERNAL "Generate the doc target.")
set(FMT_INSTALL OFF CACHE INTERNAL "Generate the install target.")
set(FMT_TEST OFF CACHE INTERNAL "Generate the test target.")
set(FMT_FUZZ OFF CACHE INTERNAL "Generate the fuzz target.")
set(FMT_CUDA_TEST OFF CACHE INTERNAL "Generate the cuda-test target.")
set(FMT_SYSTEM_HEADERS OFF CACHE BOOL "Expose headers with marking them as system." FORCE)

FetchContent_MakeAvailable(fmt)