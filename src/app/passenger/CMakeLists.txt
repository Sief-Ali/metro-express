add_library(passenger STATIC
    passenger.c
    include/passenger.h
    include/passenger_types.h
)

target_include_directories(controller
    PUBLIC
        include
)

target_link_libraries(passenger
    PUBLIC
        logger
        ui
        controller
        board
        btn
)