# shared_buf
### C++ buffer with shared memory
`xu::shared_buf` wraps a `std::shared_ptr<uint8_t[]>` with a size.

Additionally, it implements:
  - an iterator
  - operator<<(stream, buf)
