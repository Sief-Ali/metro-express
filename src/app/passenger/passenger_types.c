#include "passenger_types.h"

extern const destination_info_t destination_info[DESTINATION_COUNT] = {
    [ALEXANDRIA]    = { .display_name = "Alex.",    .code = "ALX" },
    [CAIRO]         = { .display_name = "Cairo",    .code = "CAI" },
    [ASWAN]         = { .display_name = "Aswan",    .code = "ASW" },
    [LUXOR]         = { .display_name = "Luxor",    .code = "LUX" },
    [HURGADA]       = { .display_name = "Hurgada",  .code = "HUR" },
    [MARSA_MATROUH] = { .display_name = "Matrouh",  .code = "MAT" },
    [SUEZ]          = { .display_name = "Suez",     .code = "SUE" },
    [TANTA]         = { .display_name = "Tanta",    .code = "TAN" },
    [MANSOURA]      = { .display_name = "Mansoura", .code = "MAN" },
    [PORT_SAID]     = { .display_name = "Pt. Said", .code = "PTS" },
    [ISMAILIA]      = { .display_name = "Ismailia", .code = "ISM" },
    [DAMANHUR]      = { .display_name = "Damanhur", .code = "DAM" }
};