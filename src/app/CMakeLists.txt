add_subdirectory(logger)
add_subdirectory(ui)
add_subdirectory(controller)

add_library(app STATIC
    app.c
    include/app.h
    include/app_types.h
)

target_include_directories(controller
    PUBLIC
        include
)

target_link_libraries(app
    PUBLIC
        logger
        ui
        controller
)