#ifndef LIBPBKDF2_COMPUTE_TESTS_PBKDF2TESTVECTOR_H
#define LIBPBKDF2_COMPUTE_TESTS_PBKDF2TESTVECTOR_H

#include <cstdlib>
#include <vector>
#include <string>

namespace libpbkdf2 {
namespace compute {
namespace tests {

class PBKDF2TestVector
{
private:
    std::string name;

    const void *password;
    size_t password_length;
    const void *salt;
    size_t salt_length;
    const void *dk;
    size_t dk_length;

    size_t iter;

public:
    inline const std::string &getName() const { return name; }

    inline const void *getPasswordData() const { return password; }
    inline const void *getSaltData() const { return salt; }
    inline const void *getDerivedKeyData() const { return dk; }

    inline size_t getPasswordLength() const { return password_length; }
    inline size_t getSaltLength() const { return salt_length; }
    inline size_t getDerivedKeyLength() const { return dk_length; }

    inline size_t getIterationCount() const { return iter; }

    inline PBKDF2TestVector() { }
    inline PBKDF2TestVector(
            const std::string &name,
            const void *password, size_t password_length,
            const void *salt, size_t salt_length,
            const void *dk, size_t dk_length,
            size_t iter) :
        name(name),
        password(password), password_length(password_length),
        salt(salt), salt_length(salt_length),
        dk(dk), dk_length(dk_length),
        iter(iter) { }

    static const std::vector<PBKDF2TestVector> &getStandardVectors(std::string hashSpec);
};

} // namespace tests
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_TESTS_PBKDF2TESTVECTOR_H
