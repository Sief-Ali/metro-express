add_library(controller STATIC
    controller.c
    include/controller.h
    include/controller_types.h
)

target_include_directories(controller
    PUBLIC
        include
)

target_link_libraries(controller
    PUBLIC
        avr_options
        app
        logger
        ui
        analog
        passenger
        board
        timer_service
)
 