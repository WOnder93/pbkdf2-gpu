#ifndef LIBPBKDF2_COMPUTE_TESTS_TESTEXCEPTION_H
#define LIBPBKDF2_COMPUTE_TESTS_TESTEXCEPTION_H

#include <exception>
#include <string>

namespace libpbkdf2 {
namespace compute {
namespace tests {

class TestException : public std::exception
{
private:
    std::string message;

public:
    inline const std::string &getMessage() const { return message; }

    inline TestException(const std::string &message) :
        message(message) { }

    virtual const char *what() const noexcept {
        return message.c_str();
    }
};

} // namespace tests
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_TESTS_TESTEXCEPTION_H
