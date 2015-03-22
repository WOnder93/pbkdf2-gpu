#ifndef ERRNO_H
#define ERRNO_H

namespace lukscrack {
namespace crypto {

/* Dummy errno values for cryptsetup code: */
enum {
    EINVAL = 1,
    ENOMEM,
    ENOTSUP,
    ENOENT,
    EIO
};

} // namespace crypto
} // namespace lukscrack

#endif // ERRNO_H
