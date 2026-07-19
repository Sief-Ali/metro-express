add_library(ui STATIC
    ui.c
    include/ui.h
)

target_include_directories(ui
    PUBLIC
        include
)

target_link_libraries(ui
    PUBLIC
        avr_options
        lcd
        board
)
 