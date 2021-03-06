# Building and Running Tests
include_directories(
    ${3rdparty_INCLUDES}
    ${TESTS_3rdparty_INCLUDES}
    ${pfasst_INCLUDES}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(TESTS
    test_fft
    test_spectral_1d
)

set(TESTS_MPI
)

if(NOT pfasst_WITH_MPI)
    foreach(test ${TESTS})
        message(STATUS "  ${test}")
        add_executable(${test} ${test}.cpp)
        if(${pfasst_NUM_DEPENDEND_TARGETS} GREATER 0)
            add_dependencies(${test} ${pfasst_DEPENDEND_TARGETS})
        endif()
        if(${pfasst_TESTS_NUM_DEPENDEND_TARGETS} GREATER 0)
            add_dependencies(${test} ${pfasst_TESTS_DEPENDEND_TARGETS})
        endif()
        target_link_libraries(${test}
            ${3rdparty_DEPENDEND_LIBS}
            ${TESTS_3rdparty_DEPENDEND_LIBS}
            ${pfasst_DEPENDEND_LIBS}
        )
        if(pfasst_WITH_GCC_PROF AND ${CMAKE_CXX_COMPILER_ID} MATCHES GNU)
            set_target_properties(${test}
                PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -ftest-coverage -fprofile-arcs"
                           LINK_FLAGS "-fprofile-arcs"
            )
        endif()
        add_test(NAME ${test}
            COMMAND ${CMAKE_BINARY_DIR}/tests/contrib/${test} --gtest_output=xml:${test}_out.xml
        )
    endforeach(test)
else()
    include_directories(${MPI_CXX_INCLUDE_PATH})
    foreach(test ${TESTS_MPI})
        message(STATUS "  ${test}")
        add_executable(${test} ${test}.cpp)
        if(${pfasst_NUM_DEPENDEND_TARGETS} GREATER 0)
            add_dependencies(${test} ${pfasst_DEPENDEND_TARGETS})
        endif()
        if(${pfasst_TESTS_NUM_DEPENDEND_TARGETS} GREATER 0)
            add_dependencies(${test} ${pfasst_TESTS_DEPENDEND_TARGETS})
        endif()
        target_link_libraries(${test}
            ${3rdparty_DEPENDEND_LIBS}
            ${TESTS_3rdparty_DEPENDEND_LIBS}
            ${pfasst_DEPENDEND_LIBS}
        )
        if(pfasst_WITH_GCC_PROF AND ${CMAKE_CXX_COMPILER_ID} MATCHES GNU)
            set_target_properties(${test}
                PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -ftest-coverage -fprofile-arcs"
                           LINK_FLAGS "-fprofile-arcs"
            )
        endif()
        add_test(NAME ${test}
            COMMAND ${CMAKE_BINARY_DIR}/tests/contrib/${test} --gtest_output=xml:${test}_out.xml
        )
    endforeach(test)
endif()
