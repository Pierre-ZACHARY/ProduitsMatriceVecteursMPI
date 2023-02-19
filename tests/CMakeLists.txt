include_directories(${CMAKE_SOURCE_DIR})
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
        hello_test
        hello_test.cc
        matrice_test.cpp
        ../classes/Matrix.h
        ../classes/Matrix.cpp)
add_executable(
        mpi_tests
        mpi_tests.cpp
        ../classes/Matrix.h
        ../classes/Matrix.cpp
        ../classes/MPI1_Impl.cpp
        ../classes/MPITemplate.cpp
        ../classes/MPI1_Impl.h
        ../classes/MPITemplate.h)
target_link_libraries(
        hello_test
        GTest::gtest_main
)
target_link_libraries(
        mpi_tests
        GTest::gtest_main
        MPI::MPI_CXX
        OpenMP::OpenMP_CXX
)
add_custom_command(
        TARGET hello_test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/matrice_input_test
        ${CMAKE_CURRENT_BINARY_DIR}/matrice_input_test)

set_property(DIRECTORY PROPERTY ADDITIONAL_MAKE_CLEAN_FILES matrice_input_test)


include(GoogleTest)
gtest_discover_tests(hello_test)
#gtest_discover_tests(mpi_tests)
