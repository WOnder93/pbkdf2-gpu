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
