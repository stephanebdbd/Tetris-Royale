#pragma once

#include <iostream>
#include <sodium.h>
#include <openssl/sha.h>


namespace Security {

    std::string genSalt();

    std::string hashPwd(const std::string& salt_pwd);

    bool verifyPwd(const std::string& hash_pwd, const std::string& salt_pwd);
}
