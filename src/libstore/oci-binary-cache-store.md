R"(

**Store URL format**: `oci-bc://`*registry*`/`*repo*

This store allows reading and writing a binary cache stored in an OCI registry.

This implementation simply treats the OCI registry as a generic blob store
through the `BinaryCacheStore` interface.

)"
