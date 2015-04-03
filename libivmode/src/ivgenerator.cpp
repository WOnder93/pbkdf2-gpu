#include "ivgenerator.h"

#include "plainivgenerator.h"
#include "plain64ivgenerator.h"
#include "essivivgenerator.h"

#include <unordered_map>

namespace libivmode {

IVGenerator::Context::~Context()
{
}

IVGenerator::~IVGenerator()
{
}

std::shared_ptr<IVGenerator> buildNullGenerator(const std::string &)
{
    return std::shared_ptr<IVGenerator>(nullptr);
}

template<typename Generator>
std::shared_ptr<IVGenerator> buildGenerator(const std::string &opts)
{
    return std::make_shared<Generator>(opts);
}

std::shared_ptr<const IVGenerator> IVGenerator::getGenerator(const std::string &ivmode)
{
    typedef std::shared_ptr<IVGenerator> (*GeneratorFactory)(const std::string &opts);

    static const std::unordered_map<std::string, GeneratorFactory> factories {
        { "null",    &buildNullGenerator },
        { "plain",   &buildGenerator<PlainIVGenerator> },
        { "plain64", &buildGenerator<Plain64IVGenerator> },
        { "essiv",   &buildGenerator<EssivIVGenerator> },
    };

    size_t pos = ivmode.find(':');
    std::string mode, opts;
    if (pos == std::string::npos) {
        mode.assign(ivmode);
    } else {
        mode.assign(ivmode.begin(), ivmode.begin() + pos);
        opts.assign(ivmode.begin() + pos + 1, ivmode.end());
    }
    return factories.at(mode)(opts);
}

} // namespace libivmode
