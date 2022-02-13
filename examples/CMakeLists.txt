if ( BUILD_EXAMPLES )
    option( BUILD_EXAMPLE_GLFW ON )
endif()

if ( BUILD_EXAMPLE_GLFW )
    add_subdirectory( glfw )
endif()
