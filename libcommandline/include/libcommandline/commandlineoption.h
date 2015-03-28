#ifndef LIBCOMMANDLINE_COMMANDLINEOPTION_H
#define LIBCOMMANDLINE_COMMANDLINEOPTION_H

#include <string>
#include <ostream>
#include <sstream>
#include <functional>
#include <exception>
#include <stdexcept>

namespace libcommandline {

class ArgumentFormatException : public std::runtime_error
{
private:
    std::string message;

public:
    inline const std::string &getMessage() const { return message; }

    inline ArgumentFormatException(const std::string &message)
        : std::runtime_error(message), message(message)
    {
    }

    const char *what() const noexcept override
    {
        return message.c_str();
    }
};

template<class TState>
class CommandLineOption
{
private:
    std::string longName;
    char shortName;
    std::string helpText;
    bool takesArgument;
    std::string metavar;

public:
    inline const std::string &getLongName() const { return longName; }
    inline char getShortName() const { return shortName; }
    inline const std::string &getHelpText() const { return helpText; }
    inline bool doesTakeArgument() const { return takesArgument; }

    inline CommandLineOption(
            const std::string &longName, char shortName = '\0',
            const std::string &helpText = std::string(),
            bool takesArgument = false, const std::string &metavar = std::string())
        : longName(longName), shortName(shortName), helpText(helpText),
          takesArgument(takesArgument), metavar(metavar)
    {
    }
    virtual ~CommandLineOption() { }

    virtual void processOption(TState &state, const std::string &argument) const = 0;

    std::string formatOptionName() const
    {
        std::ostringstream res {};
        if (shortName == '\0') {
            res << "    ";
        } else {
            res << "-" << shortName << ", ";
        }
        res << "--" << longName;
        if (takesArgument && !metavar.empty()) {
            res << "=" << metavar;
        }
        return res.str();
    }
};

template<class TState>
class FlagOption : public CommandLineOption<TState>
{
public:
    typedef void Callback(TState &state);

private:
    std::function<Callback> callback;

public:
    inline FlagOption(
            std::function<Callback> callback,
            const std::string &longName, char shortName = '\0',
            const std::string &helpText = std::string())
        : CommandLineOption<TState>(longName, shortName, helpText),
          callback(callback)
    {
    }

    void processOption(TState &state, const std::string &) const override
    {
        callback(state);
    }
};

template<class TState>
class ArgumentOption : public CommandLineOption<TState>
{
public:
    typedef void Callback(TState &state, const std::string &argument);

private:
    std::function<Callback> callback;

    static std::string formatHelpText(std::string helpText, std::string defaultValue)
    {
        if (helpText.empty()) {
            return std::string();
        }
        if (defaultValue.empty()) {
            return helpText;
        }
        return helpText + " [default: " + defaultValue + "]";
    }

public:
    inline ArgumentOption(
            std::function<Callback> callback,
            const std::string &longName, char shortName = '\0',
            const std::string &helpText = std::string(),
            const std::string &defaultValue = std::string(),
            const std::string &metavar = "ARG")
        : CommandLineOption<TState>(
              longName, shortName, formatHelpText(helpText, defaultValue),
              true, metavar),
          callback(callback)
    {
    }

    void processOption(TState &state, const std::string &argument) const override
    {
        callback(state, argument);
    }
};

template<class TState>
class PositionalArgumentHandler
{
public:
    typedef void Callback(TState &state, const std::string &argument);

private:
    std::string name;
    std::string helpText;
    std::function<Callback> callback;

public:
    inline const std::string &getName() const { return name; }
    inline const std::string &getHelpText() const { return helpText; }

    inline PositionalArgumentHandler(
            std::function<Callback> callback,
            const std::string &name = std::string(),
            const std::string &helpText = std::string())
        : name(name), helpText(), callback(callback)
    {
        if (!helpText.empty()) {
            this->helpText = name + "  " + helpText;
        }
    }

    void processArgument(TState &state, const std::string &argument) const
    {
        callback(state, argument);
    }
};

} // namespace libcommandline

#endif // LIBCOMMANDLINE_COMMANDLINEOPTION_H
