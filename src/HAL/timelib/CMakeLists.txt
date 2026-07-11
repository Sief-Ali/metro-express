add_library(timelib STATIC
    timelib.c
    include/timelib.h
)

target_include_directories(timelib
    PUBLIC
        include
)

target_link_libraries(timelib
    PUBLIC
        avr_options
        timer
)
 