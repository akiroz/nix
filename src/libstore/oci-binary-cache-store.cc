#include "binary-cache-store.hh"
#include "filetransfer.hh"
#include "globals.hh"
#include "nar-info-disk-cache.hh"
#include "callback.hh"
#include "hash.hh"

#include <nlohmann/json.hpp>

namespace nix {

struct OciBinaryCacheStoreConfig : virtual BinaryCacheStoreConfig
{
    using BinaryCacheStoreConfig::BinaryCacheStoreConfig;

    const std::string name() override { return "OCI Binary Cache Store"; }

    std::string doc() override
    {
        return
          #include "oci-binary-cache-store.md"
          ;
    }
};

struct OciBinaryCacheStore : public virtual OciBinaryCacheStoreConfig, public virtual BinaryCacheStore
{
    std::string ociName;
    std::string ociEndpoint;

    OciBinaryCacheStore(
        const std::string & scheme,
        const std::string & name,
        const Params & params)
        : ociName(name)
    {
        diskCache = getNarInfoDiskCache();
        std::string_view repo{ name };
        auto registry = splitPrefixTo(repo, '/');
        ociEndpoint = "https://" + registry + "/v2/" + repo + "/";
    }

    std::string getUri() override
    {
        return "oci-bc://" + ociName;
    }

    void init() override
    {
        if (auto cacheInfo = diskCache->upToDateCacheExists(getUri())) {
            wantMassQuery.setDefault(cacheInfo->wantMassQuery);
            priority.setDefault(cacheInfo->priority);
        } else {
            BinaryCacheStore::init();
            diskCache->createCache(getUri(), storeDir, wantMassQuery, priority);
        }
    }

    /**
     * Use hashed path as OCI digest, this isn't actually a content digest.
     * 
     * OCI complient registries SHOULD allow digests with unrecognized
     * algorithms to pass validation
     */
    std::string ociDigest(const std::string & path)
    {
        auto hash = hashString(htSHA256, path);
        return "oci-bc:" + hash.to_string(Base32, false);
    }

    bool fileExists(const std::string & path) override
    {
        auto digest = ociDigest(path);
        // TODO: HEAD blobs/<digest>
    }

    void upsertFile(const std::string & path,
        std::shared_ptr<std::basic_iostream<char>> istream,
        const std::string & mimeType) override
    {
        auto digest = ociDigest(path);
        // TODO: upload blob, count size

        auto manifest = nlohmann::json{
            {"schemaVersion", 2},
            {"config", {
                {"mediaType", "application/vnd.oci.image.config.v1+json"},
            }},
            {"layers", {
                {
                    {"mediaType", mimeType},
                    {"digest", digest},
                    {"size", },
                },
            }}
        };
        // TODO: upload manifest
    }

    void getFile(const std::string & path, Sink & sink) override
    {
        auto digest = ociDigest(path);
        // TODO: GET blobs/<digest>
    }

    /**
     * For now, we conservatively say we don't know.
     *
     * \todo try to expose our registry authentication status.
     */
    std::optional<TrustedFlag> isTrustedClient() override
    {
        return std::nullopt;
    }

    /**
     * Reserve the plain oci:// scheme for full-fledged
     * store implementation
     */
    static std::set<std::string> uriSchemes() { return {"oci-bc"}; }

}
