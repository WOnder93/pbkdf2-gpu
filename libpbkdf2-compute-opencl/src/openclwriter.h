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
