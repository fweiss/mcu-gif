// internal, exposed for unit testing
typedef struct {
    uint8_t size;
    uint16_t *characters;
} gd_string_t;

typedef struct {
    gd_status_t status;
    uint8_t code_size;
    gd_string_t *string_table;
    uint16_t string_table_size;
    uint16_t string_table_size_max;
    gd_string_t *previous_string;
    uint16_t *characters;
    uint16_t characters_size;
    uint16_t characters_size_max;
} gd_lzw_t;

void gd_lzw_decode_next(gd_lzw_t *lzw, uint16_t code);

