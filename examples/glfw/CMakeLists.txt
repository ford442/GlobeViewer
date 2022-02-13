set( example example_glfw )


add_executable( ${example} gles.cpp )


target_link_libraries( ${example}
    globe_viewer
)


file( COPY ${DATA_DIR}/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )

install( TARGETS ${example} DESTINATION ${example} )
install( DIRECTORY ${DATA_DIR}/shaders DESTINATION ${example} )
