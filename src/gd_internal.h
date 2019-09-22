// internal, exposed for unit testing
typedef struct {
    uint8_t size;
    uint16_t *characters;
} gd_code_string_t;

typedef struct {
    gd_status_t status;
    gd_code_string_t *code_table;
    uint16_t code_table_size;
    uint8_t current_code_size;
    gd_code_string_t *previous_string;
    uint16_t *codes;
} gd_lzw_t;

void gd_lzw_decode_next(uint16_t extract, gd_lzw_t *lzw);

