/*
 * Copyright (C) 2015, Ondrej Mosnacek <omosnacek@gmail.com>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation: either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
    out << indent << "for (size_t " << loopVar << " = " << from
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

