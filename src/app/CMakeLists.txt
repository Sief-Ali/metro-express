add_subdirectory(logger)
add_subdirectory(ui)

add_library(app STATIC
    app.c
)

target_link_libraries(app
    PUBLIC
        logger
        ui
)