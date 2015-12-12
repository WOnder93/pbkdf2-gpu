#ifndef LIBPBKDF2_COMPUTE_OPENCL_OPENCLWRITER_H
#define LIBPBKDF2_COMPUTE_OPENCL_OPENCLWRITER_H

#include <ostream>
#include <string>

namespace libpbkdf2 {
namespace compute {
namespace opencl {

class OpenCLWriter
{
private:
    std::ostream &out;
    std::string indent;

public:
    OpenCLWriter(std::ostream &out);

    void writeEmptyLine();

    void writeDeclaration(const std::string &type,
                          const std::string &variable);

    void writeArrayDeclaration(const std::string &type,
                               const std::string &variable,
                               const std::string &size);

    void beginAssignment(const std::string &variable);
    void beginArrayAssignment(const std::string &variable);
    void endArrayAssignment();
    void endAssignment();

    void beginBlock();

    void beginIf();
    void endIf();
    void beginElse();

    void beginLoop(const std::string &loopVar,
                   const std::string &from,
                   const std::string &to);
    void endBlock();

    template<class T>
    OpenCLWriter &operator<<(const T &value)
    {
        out << value;
        return *this;
    }

    OpenCLWriter &operator<<(std::ostream &(&f)(std::ostream &))
    {
        out << f;
        return *this;
    }
};

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_OPENCL_OPENCLWRITER_H
