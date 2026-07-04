add_library(timer STATIC
    timer.c
    include/timer.h
)

target_include_directories(timer
    PUBLIC
        include
)

target_link_libraries(timer
    PUBLIC
        avr_options
        utils
)