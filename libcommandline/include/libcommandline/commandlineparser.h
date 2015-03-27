#ifndef LIBCOMMANDLINE_COMMANDLINEPARSER_H
#define LIBCOMMANDLINE_COMMANDLINEPARSER_H

#include "commandlineoption.h"

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <iostream>
#include <unordered_map>

namespace libcommandline {

template<class TState>
class CommandLineParser
{
private:
    std::string helpText;
    PositionalArgumentHandler<TState> posArgHandler;
    std::vector<std::unique_ptr<const CommandLineOption<TState>>> options;

    std::unordered_map<char, const CommandLineOption<TState> *> mapShort;
    std::unordered_map<std::string, const CommandLineOption<TState> *> mapLong;

    inline static int tryProcessOption(
            const std::string &progname, const std::string &optname,
            const CommandLineOption<TState> *opt,
            TState &state, const std::string &argument)
    {
        try {
            opt->processOption(state, argument);
        } catch (const ArgumentFormatException& e) {
            std::cerr << progname << ": "
                      << "'" << optname << "': "
                      << "'" << argument << "': "
                      << "invalid argument format: "
                      << e.getMessage() << std::endl;
            return 1;
        } catch (const std::exception &e) {
            std::cerr << progname << ": "
                      << "'-" << optname << "': "
                      << "error while processing option: "
                      << e.what() << std::endl;
            return 1;
        }
        return 0;
    }

    inline static int tryProcessOption(
            const std::string &progname, const std::string &optname,
            const CommandLineOption<TState> *opt,
            TState &state)
    {
        try {
            opt->processOption(state, std::string());
        } catch (const std::exception &e) {
            std::cerr << progname << ": "
                      << "'-" << optname << "': "
                      << "error while processing option: "
                      << e.what() << std::endl;
            return 1;
        }
        return 0;
    }

public:
    CommandLineParser(const CommandLineParser &) = delete;
    CommandLineParser &operator=(const CommandLineParser &) = delete;

    CommandLineParser(CommandLineParser &&) = default;
    CommandLineParser &operator=(CommandLineParser &&) = default;

    /* NOTE: the paser takes ownership of the pointers
     * passed in the 'options' vector */
    inline CommandLineParser(
            const std::string &helpText,
            const PositionalArgumentHandler<TState> &posArgHandler,
            const std::vector<const CommandLineOption<TState>*> &options)
        : helpText(helpText), posArgHandler(posArgHandler), options(),
          mapShort(), mapLong()
    {
        for (auto &opt : options) {
            mapLong.insert(std::make_pair(opt->getLongName(), opt));

            char shortName = opt->getShortName();
            if (shortName != '\0') {
                mapShort.insert(std::make_pair(shortName, opt));
            }
            this->options.emplace_back(opt);
        }
    }

    void printHelp(const char * const * argv) const {
        const std::string progname(argv[0]);

        std::cerr << "usage: " << progname << " [options]";
        if (!posArgHandler.getName().empty()) {
            std::cerr << " " << posArgHandler.getName();
        }
        std::cerr << std::endl;

        if (!helpText.empty()) {
            std::cerr << "  " << helpText << std::endl;
        }
        std::cerr << std::endl;

        size_t longest = 0;
        for (auto &opt : options) {
            auto ptr = opt.get();
            size_t length = ptr->getLongName().size();
            if (ptr->takesArgument()) {
                length += 4; /* std::string("=ARG").size(); */
            }
            if (length > longest) {
                longest = length;
            }
        }
        std::cerr << "Options:" << std::endl;
        for (auto &opt : options) {
            auto ptr = opt.get();

            std::cerr << "  ";
            char shortName = ptr->getShortName();
            if (shortName != '\0') {
                std::cerr << "-" << shortName << ", ";
            } else {
                std::cerr << "    ";
            }

            size_t padding = longest;
            std::cerr << "--" << ptr->getLongName();
            padding -= ptr->getLongName().size();
            if (ptr->takesArgument()) {
                std::cerr << "=ARG";
                padding -= 4;
            }
            std::cerr << std::string(padding + 2, ' ');
            std::cerr << ptr->getHelpText();
            ptr->printExtraHelpText(std::cerr);
            std::cerr << std::endl;
        }
        if (!posArgHandler.getHelpText().empty()) {
            std::cerr << std::endl;
            std::cerr << "Positional arguments:" << std::endl;
            std::cerr << "  " << posArgHandler.getHelpText() << std::endl;
        }
    }

    int parseArguments(TState &state, const char * const * argv) const
    {
        const std::string progname(argv[0]);

        bool endOfOptions = false;
        for (auto argp = argv + 1; *argp != nullptr; argp++) {
            const std::string arg(*argp);
            if (endOfOptions || arg.size() < 2 || arg[0] != '-') {
                /* positional argument */
                try {
                    posArgHandler.processArgument(state, arg);
                } catch (const ArgumentFormatException& e) {
                    std::cerr << progname << ": "
                              << "'" << arg << "': "
                              << "invalid argument format: "
                              << e.getMessage() << std::endl;
                    return 1;
                } catch (const std::exception &e) {
                    std::cerr << progname << ": "
                              << "'" << arg << "': "
                              << "error while parsing argument: "
                              << e.what() << std::endl;
                    return 1;
                }
            } else if (arg[1] != '-') {
                /* short option */
                size_t optIndex = 1;
                do {
                    char name = arg[optIndex++];
                    auto entry = mapShort.find(name);
                    if (entry == mapShort.end()) {
                        std::cerr << progname << ": "
                                  << "unrecognized option: "
                                  "'-" << name << "'" << std::endl;
                        return 1;
                    }
                    auto opt = entry->second;
                    if (opt->takesArgument()) {
                        /* if the option takes an argument, then
                         * the rest of this cmdline arg is passed
                         * to the handler as that argument */
                        std::string argument;
                        if (optIndex == arg.size()) {
                            /* no characters left in this arg,
                             * eat the next cmdline argument */
                            ++argp;
                            if (*argp == nullptr) {
                                std::cerr << progname << ": "
                                          << "'-" << name << "': "
                                          << "option expects an argument"
                                          << std::endl;
                                return 1;
                            }
                            argument = std::string(*argp);
                        } else {
                            argument = arg.substr(optIndex);
                        }
                        int ret = tryProcessOption(progname, "-" + name, opt, state, argument);
                        if (ret != 0) return ret;
                        break;
                    }
                    /* if the option doesn't take an argument,
                     * then process the rest of this arg as
                     * another short option(s) */
                    int ret = tryProcessOption(progname, "-" + name, opt, state);
                    if (ret != 0) return ret;
                } while (optIndex < arg.size());
            } else if (arg.size() == 2) {
                /* -- (end of options) */
                endOfOptions = true;
            } else {
                /* long option */
                std::string name;

                /* split arg by equal sign: */
                auto eqSignPos = arg.find('=', 2);
                if (eqSignPos != std::string::npos) {
                    name = std::string(arg.begin() + 2, arg.begin() + eqSignPos);
                } else {
                    name = std::string(arg.begin() + 2, arg.end());
                }

                auto entry = mapLong.find(name);
                if (entry == mapLong.end()) {
                    std::cerr << progname << ": "
                              << "unrecognized option: "
                              "'--" << name << "'" << std::endl;
                    return 1;
                }
                auto opt = entry->second;
                if (opt->takesArgument()) {
                    std::string argument;
                    if (eqSignPos != std::string::npos) {
                        /* take whatever is after the equal
                         * sign as the argument: */
                        argument = std::string(arg.begin() + eqSignPos + 1, arg.end());
                    } else {
                        /* no equal sign was found --
                         * eat the next cmdline argument: */
                        ++argp;
                        if (*argp == nullptr) {
                            std::cerr << progname << ": "
                                      << "'--" << name << "': "
                                      << "option expects an argument"
                                      << std::endl;
                            return 1;
                        }
                        argument = std::string(*argp);
                    }
                    int ret = tryProcessOption(progname, "--" + name, opt, state, argument);
                    if (ret != 0) return ret;
                } else if (eqSignPos != std::string::npos) {
                    std::cerr << progname << ": "
                              << "option "
                              "'--" << name << "'"
                              << " does not take an argument"
                              << std::endl;
                    return 1;
                } else {
                    int ret = tryProcessOption(progname, "--" + name, opt, state);
                    if (ret != 0) return ret;
                }
            }
        } /* for */
        return 0;
    }
};

} // namespace libcommandline

#endif // LIBCOMMANDLINE_COMMANDLINEPARSER_H
