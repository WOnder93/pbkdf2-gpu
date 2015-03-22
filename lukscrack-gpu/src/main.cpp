#include "crypto/crypto_backend.h"

#include <iostream>

using namespace std;
using namespace lukscrack::crypto;

int main()
{
    crypt_backend_init();
    crypt_backend_cleanup();
    return 0;
}

