#ifndef LIBCOMMANDLINE_COMMANDLINEOPTION_H
#define LIBCOMMANDLINE_COMMANDLINEOPTION_H

#include <string>
#include <ostream>
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

public:
    inline const std::string &getLongName() const { return longName; }
    inline char getShortName() const { return shortName; }
    inline const std::string &getHelpText() const { return helpText; }

    inline CommandLineOption(
            const std::string &longName, char shortName = '\0',
            const std::string &helpText = std::string())
        : longName(longName), shortName(shortName), helpText(helpText)
    {
    }
    virtual ~CommandLineOption() { }

    virtual bool takesArgument() const = 0;
    virtual void printExtraHelpText(std::ostream &) const { }
    virtual void processOption(TState &state, const std::string &argument) const = 0;

    void printHelpText(std::ostream &out) const
    {
        out << helpText;
        printExtraHelpText(out);
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
            const std::string &longName, char shortName,
            const std::string &helpText, std::function<Callback> callback)
        : CommandLineOption<TState>(longName, shortName, helpText),
          callback(callback)
    {
    }

    bool takesArgument() const override { return false; }
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
    std::string defaultValue;
    std::function<Callback> callback;

public:
    inline ArgumentOption(
            const std::string &longName, char shortName,
            const std::string &helpText, const std::string &defaultValue,
            std::function<Callback> callback)
        : CommandLineOption<TState>(longName, shortName, helpText),
          defaultValue(defaultValue), callback(callback)
    {
    }

    bool takesArgument() const override { return true; }
    void printExtraHelpText(std::ostream &out) const override
    {
        if (!defaultValue.empty()) {
            out << " [default: " << defaultValue << "]";
        }
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
            const std::string &name, const std::string &helpText,
            std::function<Callback> callback)
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
