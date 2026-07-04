add_library(timer STATIC
    timer.c
    include/timer.h
    timer_registers.h
    timer_private.h
)

target_include_directories(timer
    PUBLIC
        include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(timer
    PUBLIC
        avr_options
        utils
)