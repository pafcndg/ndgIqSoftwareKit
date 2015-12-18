@addtogroup ll_storage_service
@{

## Features

Low Level Storage Service API with block level read/write/erase operations for the
non-volatile memory.

The number of partitions for a platform and their properties are fixed at
firmware build time.
All partitions are not meant to be managed by the Storage Service, the list of
partitions it manages is configured at boot time by the board configurable
code (storage_configuration).

The Storage Service does not support any wear-leveling (bad block management).

Two levels of APIs are provided for full flexibility to access each partitions.
A partition shall be accessed only using one and only one API.

- *High Level API* should be used by applications and other services.
  - Properties API with add/read/write.
    - It provides file system-like operations where a property (buffer) is
      defined with two IDs (the first id mimics a directory name, the second one
      mimics a file name).
    - Property (buffer) max size and its storage class (factory erasable or
      not) can't be changed after being added.
    - It shall be used only for data that is updated at low rate because no
      real support of wear-leveling.
    - The maximum size of a property depends on the underlying flash memory
      (and is linked to the block size).
- *Low Level API* with block level read/write/erase operations.
  - Given that the block size is large and the amount of internal RAM is small,
    we provide a read/write API that works at a lower level than block level to
    avoid caching a large amount of data.
  - The minimum size / alignment of an access depends on the underlying
    non-volatile memory, and shall be enforced by the application/service when
    Low Level API is used directly.
  - The Intel&reg; Curie&trade; on-die flash requires accesses to be 4-bytes
    aligned and the size being a multiple of 4-bytes.

The data that is stored and the amount of them highly depend on the availability
and the size of the external flash.

Clients of this interface can:
- \b read - read data from a partition; return size that has
            been read and data or error code in case of issues.
- \b write - write to a partition; return size that has been
             written or error code in case of issues.
- \b read_block - read a full block from a partition; return number of blocks
                  that has been read and data or error code in case of issues.
- \b write_block - write a full block to a partition; return number of blocks
                   that has been written or error code in case of issues.
- \b erase_block - erase block(s) in the requested partition; returns
                   status of erase.
- \b erase_partition - erase an entire partition; returns status of erase.

@image html storage.png "Storage system architecture"

@}
