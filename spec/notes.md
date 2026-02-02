# testing ``image_code_expand``
Specs that call it:
- ``expand_state_spec`` OK
- ``image_decompress_spec`` OK
- ``image_expand_spec`` OK

and above those ``image_block_spec``

## ``expand_state_spec`
// add successive unpacked codes
// examine the output, string_table, prior string
checks:
- string table
- prior string



## ``image_decompress_spec``
checks output and codeSize
- prior string
- string table
- output


## ``image_expand_spec``
checks output for various code streams
- output
- codeSize

## image data stack
- ``gd_read_image_data(gd_main_t *main, gd_index_t *output, size_t capacity);``
- ``gd_image_block_read(gd_main_t *main, gd_image_block_t *image_block)``
- ``gd_image_subblock_unpack(&image_block->unpack, subblock, subblockSize);``
- ``gd_image_code_expand(&unpack->expandCodes, unpack->extract);``
- ``gd_string_table-init/find/add``

### ``gd_read_image_data``
Just a wrapper for ``gd_image_block_read``.
Bridges the API to the internal stack.

Specs that call this:
- 10x10_red_blue_while
- read_block_error_spec
- read_block_spec

### ``gd_image_block_read``
Reads the min code size and a sequece of subblocks.

There is a single spec that calls this:
- image_data_block_spec

### ``gd_image_subblock_unpack``
Unpacks the code stream from the subblock.

There's only one spec that calls it:
- image_subblock_spec

### ``gd_image_code_expand``
Produces the index stream by decompressing the code stream.

There are a number of specs that call this:
- image_state_spec
- image_decompress_spec
- image_expand_spec

These are a bit mixed up.

## Testing levels
- API

### string table
- functions called: string_table_init, string_table_find, string_table-add
- mocked data: string table parameters
- stimulus: function call
- verify: object states

### expand
- function called: ``gd_expand``
- mocked data: gd_expand
- stimulus: codes, string tablen size, output size
- verify: expand states, string table, output
specs: expand_state_spec

### unpack
- function called: ``gd_unpack``
- mocked data: unpack, expand, FFILE
- verify: unpack state, expand state, (output?)

### api
- functions called: API functions
- mocked: FFILE (sometimes)
- verify: block data, errors

## Matrix
cross reference between specs and code
- C = call
- C2 = call below API
- M = mock
- E = excercise/execute
- E3 = some specs
- V = verify
- F = file mock
- X = not applicable

| spec | api | unpack | expand | table | output | helpers
| --- | --- | --- | --- | --- | --- | --- 
| 10x10_red_blue_spec | F C | E | E | E | V | X
| image_data_block_spec | F C2 M|E|E V|X|E V|X|
| read_block_spec |F C V |X|X|X|X|X
| read_block_error_spec | F C V|E3|E3|EV|X|X
| image_subblock_spec |X|M C V|M E V|E V|V|X
| expand_state_spec | X | X | C V | C E V | X | X
| image_decompress_spec |X|X|M C V| V|V|X
| image_expand_spec | X|X|C E V|E|V|X
| string_table_spec | X | X | X |C V | X  |X
| helpers_spec | X|X |X |X | X| C V
