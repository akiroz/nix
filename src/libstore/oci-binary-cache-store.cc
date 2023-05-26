#include "binary-cache-store.hh"
#include "filetransfer.hh"
#include "globals.hh"
#include "nar-info-disk-cache.hh"
#include "callback.hh"

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
    std::string ociRepo;

    OciBinaryCacheStore(
        const std::string & scheme,
        const std::string & repo,
        const Params & params)
        : ociRepo(repo)
    {
        diskCache = getNarInfoDiskCache();
    }

    std::string getUri() override
    {
        return "oci://" + ociRepo;
    }

    void init() override
    {
    }

    bool fileExists(const std::string & path) override
    {
    }

    void uploadFile(const std::string & path,
        std::shared_ptr<std::basic_iostream<char>> istream,
        const std::string & mimeType,
        const std::string & contentEncoding)
    {
    }

    void upsertFile(const std::string & path,
        std::shared_ptr<std::basic_iostream<char>> istream,
        const std::string & mimeType) override
    {
    }

    void getFile(const std::string & path, Sink & sink) override
    {
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

    static std::set<std::string> uriSchemes() { return {"oci"}; }

}
