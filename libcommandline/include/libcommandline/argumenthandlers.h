#ifndef LIBCOMMANDLINE_ARGUMENTHANDLERS_H
#define LIBCOMMANDLINE_ARGUMENTHANDLERS_H

#include "commandlineoption.h"

namespace libcommandline {

template<class T>
struct NumericParsingTraits {};

template<>
struct NumericParsingTraits<int>
{
    static const char *typeName;

    inline static int parse(const std::string &arg, size_t *endpos)
    {
        return std::stoi(arg, endpos);
    }
};

template<>
struct NumericParsingTraits<long>
{
    static const char *typeName;

    inline static long parse(const std::string &arg, size_t *endpos)
    {
        return std::stol(arg, endpos);
    }
};

template<>
struct NumericParsingTraits<unsigned long>
{
    static const char *typeName;

    inline static unsigned long parse(const std::string &arg, size_t *endpos)
    {
        return std::stoul(arg, endpos);
    }
};

template<>
struct NumericParsingTraits<long long>
{
    static const char *typeName;

    inline static long long parse(const std::string &arg, size_t *endpos)
    {
        return std::stoll(arg, endpos);
    }
};

template<>
struct NumericParsingTraits<unsigned long long>
{
    static const char *typeName;

    inline static unsigned long long parse(const std::string &arg, size_t *endpos)
    {
        return std::stoull(arg, endpos);
    }
};

template<>
struct NumericParsingTraits<float>
{
    static const char *typeName;

    inline static float parse(const std::string &arg, size_t *endpos)
    {
        return std::stof(arg, endpos);
    }
};

template<>
struct NumericParsingTraits<double>
{
    static const char *typeName;

    inline static double parse(const std::string &arg, size_t *endpos)
    {
        return std::stod(arg, endpos);
    }
};

template<>
struct NumericParsingTraits<long double>
{
    static const char *typeName;

    inline static long double parse(const std::string &arg, size_t *endpos)
    {
        return std::stold(arg, endpos);
    }
};

template<class TState, class TNum>
static std::function<typename ArgumentOption<TState>::Callback> makeNumericHandler(
        std::function<void(TState &, TNum)> callback)
{
    return [=] (TState &state, const std::string &arg) {
        size_t end;
        TNum ret;
        try {
            ret = NumericParsingTraits<TNum>::parse(arg, &end);
        } catch (const std::invalid_argument&) {
            throw ArgumentFormatException(std::string("value not valid for ") + NumericParsingTraits<TNum>::typeName);
        } catch (const std::out_of_range &) {
            throw ArgumentFormatException(std::string("value is out of range for ") + NumericParsingTraits<TNum>::typeName);
        }
        if (end != arg.size()) {
            throw ArgumentFormatException(std::string("value must be a valid ") + NumericParsingTraits<TNum>::typeName);
        }
        callback(state, ret);
    };
}

template<class TState, typename T>
static std::function<void(TState &, T)> makeCheckHandler(
        std::function<bool(T)> predicate,
        const std::string &failMessage,
        std::function<void(TState &, T)> callback)
{
    return [=, &failMessage] (TState &state, T arg) {
        if (!predicate(arg)) {
            throw ArgumentFormatException(failMessage);
        }
        callback(state, arg);
    };
}

} // namespace libcommandline

#endif // LIBCOMMANDLINE_ARGUMENTHANDLERS_H
