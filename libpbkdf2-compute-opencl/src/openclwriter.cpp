#include "openclwriter.h"

namespace libpbkdf2 {
namespace compute {
namespace opencl {

OpenCLWriter::OpenCLWriter(std::ostream &out)
    : out(out), indent()
{
}

void OpenCLWriter::writeEmptyLine()
{
    out << std::endl;
}

void OpenCLWriter::writeDeclaration(const std::string &type, const std::string &variable)
{
    out << indent << type << " " << variable << ";" << std::endl;
}

void OpenCLWriter::writeArrayDeclaration(
        const std::string &type, const std::string &variable, const std::string &size)
{
    out << indent << type << variable << "[" << size << "];" << std::endl;
}

void OpenCLWriter::beginAssignment(const std::string &variable)
{
    out << indent << variable << " = ";
}

void OpenCLWriter::beginArrayAssignment(const std::string &variable)
{
    out << indent << variable << "[";
}

void OpenCLWriter::endArrayAssignment()
{
    out << "] = ";
}

void OpenCLWriter::endAssignment()
{
    out << ";" << std::endl;
}

void OpenCLWriter::beginBlock()
{
    out << indent << "{" << std::endl;
    indent.append("    ");
}

void OpenCLWriter::beginIf()
{
    out << indent << "if (";
}

void OpenCLWriter::endIf()
{
    out << indent << ") {" << std::endl;
    indent.append("    ");
}

void OpenCLWriter::beginElse()
{
    indent.resize(indent.size() - 4);
    out << indent << "} else {" << std::endl;
    indent.append("    ");
}

void OpenCLWriter::beginLoop(const std::string &loopVar, const std::string &from, const std::string &to)
{
    out << indent << "for (uint " << loopVar << " = " << from
        << "; " << loopVar << " < " << to << "; " << loopVar << "++) {"
        << std::endl;
    indent.append("    ");
}

void OpenCLWriter::endBlock()
{
    indent.resize(indent.size() - 4);
    out << indent << "}" << std::endl;
}

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2

