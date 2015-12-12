#include "sha1hashfunctionhelper.h"

#include "openclwriter.h"

#define IBLOCK_WORDS 16
#define OBLOCK_WORDS 5
#define ML_WORDS 2

#define ITERATIONS 80

namespace libpbkdf2 {
namespace compute {
namespace opencl {

static const char * const INIT_STATE[] = {
    "0x67452301",
    "0xEFCDAB89",
    "0x98BADCFE",
    "0x10325476",
    "0xC3D2E1F0",
};

static const char * const FS[] = {
    "SHA1_F0",
    "SHA1_F1",
    "SHA1_F2",
    "SHA1_F3",
};

static const char * const KS[] = {
    "0x5A827999",
    "0x6ED9EBA1",
    "0x8F1BBCDC",
    "0xCA62C1D6",
};

Sha1HashFunctionHelper::Sha1HashFunctionHelper()
    : UIntHashFunctionHelper(
          false, IBLOCK_WORDS, OBLOCK_WORDS, ML_WORDS, INIT_STATE)
{
}

void Sha1HashFunctionHelper::writeDefinitions(OpenCLWriter &out) const
{
    out << "#ifdef cl_nv_pragma_unroll" << std::endl;
    out << "#define NVIDIA" << std::endl;
    out << "#endif /* cl_nv_pragma_unroll */" << std::endl;
    out << std::endl;
    out << "#ifndef NVIDIA" << std::endl;
    out << "#define SHA_F0(x,y,z) bitselect(z, y, x)" << std::endl;
    out << "#else" << std::endl;
    out << "#define SHA_F0(x,y,z) (z ^ (x & (y ^ z)))" << std::endl;
    out << "#endif /* NVIDIA */" << std::endl;
    out << std::endl;
    out << "#define SHA_F1(x,y,z) bitselect(y, x, y ^ z)" << std::endl;
    out << std::endl;
    out << "#define SHA1_F0(x, y, z) SHA_F0(x, y, z)" << std::endl;
    out << "#define SHA1_F1(x, y, z) (x ^ y ^ z)" << std::endl;
    out << "#define SHA1_F2(x, y, z) SHA_F1(x, y, z)" << std::endl;
    out << "#define SHA1_F3(x, y, z) (x ^ y ^ z)" << std::endl;
    out << std::endl;
}

void Sha1HashFunctionHelper::writeUpdate(
        OpenCLWriter &writer,
        const std::vector<std::string> &prevState,
        const std::vector<std::string> &inputBlock,
        const std::vector<std::string> &state,
        const std::vector<std::string> &buffer,
        bool swap) const
{
    for (std::size_t i = 0; i < OBLOCK_WORDS; i++) {
        writer.beginAssignment(state[i]);
        writer << prevState[i];
        writer.endAssignment();
    }
    writer.writeEmptyLine();

    const std::vector<std::string> &dest = swap ? prevState : state;
    const std::vector<std::string> &aux  = swap ? state : prevState;

    for (std::size_t iter = 0; iter < ITERATIONS; iter++) {
        auto &src = iter / IBLOCK_WORDS == 0
                ? inputBlock
                : buffer;

        std::size_t state_a = (ITERATIONS - iter) % OBLOCK_WORDS;
        std::size_t quarter = iter / (ITERATIONS / 4);

        writer.beginAssignment(dest[(state_a + 4) % OBLOCK_WORDS]);
        writer << "rotate(" << dest[(state_a + 0) % OBLOCK_WORDS]
               << ", (uint)5) + " << FS[quarter]
               << "(" << dest[(state_a + 1) % OBLOCK_WORDS]
               << ", " << dest[(state_a + 2) % OBLOCK_WORDS]
               << ", " << dest[(state_a + 3) % OBLOCK_WORDS]
               << ") + " << KS[quarter] << " + "
               << dest[(state_a + 4) % OBLOCK_WORDS] << " + "
               << src[iter % IBLOCK_WORDS];
        writer.endAssignment();

        writer.beginAssignment(dest[(state_a + 1) % OBLOCK_WORDS]);
        writer << "rotate(" << dest[(state_a + 1) % OBLOCK_WORDS]
               << ", (uint)30)";
        writer.endAssignment();

        if (iter % IBLOCK_WORDS == IBLOCK_WORDS - 1) {
            writer.writeEmptyLine();
            for (std::size_t i = 0; i < IBLOCK_WORDS; i++) {
                writer.beginAssignment(buffer[i]);
                writer << "rotate("
                       << src[(i + 13) % IBLOCK_WORDS] << " ^ "
                       << src[(i +  8) % IBLOCK_WORDS] << " ^ "
                       << src[(i +  2) % IBLOCK_WORDS] << " ^ "
                       << src[(i +  0) % IBLOCK_WORDS]
                       << ", (uint)1)";
                writer.endAssignment();
            }
        }
        writer.writeEmptyLine();
    }

    for (std::size_t i = 0; i < OBLOCK_WORDS; i++) {
        writer.beginAssignment(dest[i]);
        writer << dest[i] << " + " << aux[i];
        writer.endAssignment();
    }
}

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2

