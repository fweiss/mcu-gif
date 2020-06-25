# GIF domain model

domain
- stream
- protocol information aka header, trailer
- block aka data packet
- sub block
- graphics control block
- - graphics control extension block
- graphic rendering blocks
- - plain text extension block
- - local image descriptor
- image data block aka bitmap data
- special purpose block
- - comment extension
- - application extension
- control extensions (gif89a)
- - graphics control extension
- - plain text extension
- - comment extension
- - application extension

also
- code size
- compress
- compression code

- header signature
- version
- logical screen descriptor
- global color table
- local color table

stream
- header
- logical screen descriptor
- global color table
- block...
- trailer

image block
- local image descriptor
- local color table
- image data...

https://www.fileformat.info/format/gif/egff.htm
