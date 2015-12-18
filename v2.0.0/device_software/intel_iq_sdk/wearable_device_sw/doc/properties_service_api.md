@addtogroup properties_service
@anchor properties_service_api_arch


Allows any client to backup a property (service_id/property_id) into the
non-volatile memory.

It provides file system-like operations where a Property (buffer) is defined
with two IDs (one first id that can mimic a directory name, and one second id
that can mimic a file name).
Property (buffer) characteristics like its max size and its storage class can't
be changed after being created

Properties can be spread across multiple blocks, but one property can't be
stored across two blocks.
The max size of all properties inside a property block is the block size minus
the header minus "offset header".
The "offset header" holds service_id/property_id/offset to locate the property
inside the block (see [Properties block description below](@ref properties_block_description)).

Properties service is suitable for storing data with a Low update rate:
- Erased during factory reset:
  - product/application specific data
  - user settings, paired devices
  - Those settings are managed in at least two flash erase blocks to avoid
    losing any data (e.g. battery removal use-case).
- Never erased during factory reset:
  - system properties , that hold for example battery charging cycle count.
  - Those settings are also managed in at least two flash erase blocks to avoid
    losing any data (e.g. battery removal use-case)

Clients of this interface can:
      - \b add    - add a service/property if not already existing.
      - \b read   - read the latest version of a property.
      - \b write  - update the data for a given property.
      - \b remove - remove a service/property - NOT SUPPORTED

### Properties block description:

Updating of one property in a block implies:
- reading the full block,
- updating the property, and
- writing back the full block.

Removing one property in a block implies:
- reading the full block,
- packing all remaining properties, and
- writing back the full block.

@warning Removing a property is NOT SUPPORTED.


The blocks are used in circular manner for basic wear-leveling purpose.

The partition must have enough blocks to make sure that:
- at anytime one version of the properties exists
- good enough basic wear-leveling based on underlying non volatile memory Erase/Program Cycle count

@anchor properties_block_description
@image html properties.png "Properties block description"

@anchor properties_service_api
## Properties Service API

<!-- Documentation from code goes at the end -->
