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

#include "argumenthandlers.h"

namespace libcommandline {

const char *NumericParsingTraits<int>               ::typeName = "int";
const char *NumericParsingTraits<long>              ::typeName = "long";
const char *NumericParsingTraits<unsigned long>     ::typeName = "unsigned long";
const char *NumericParsingTraits<long long>         ::typeName = "long long";
const char *NumericParsingTraits<unsigned long long>::typeName = "unsigned long long";
const char *NumericParsingTraits<float>             ::typeName = "float";
const char *NumericParsingTraits<double>            ::typeName = "double";
const char *NumericParsingTraits<long double>       ::typeName = "long double";

} // namespace libcommandline
