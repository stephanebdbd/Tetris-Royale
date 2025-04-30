#include "security.hpp"

std::string Security::genSalt(){
    unsigned char salt[crypto_pwhash_SALTBYTES];
    randombytes_buf(salt, sizeof(salt));
    char c_salt[crypto_pwhash_SALTBYTES*2];
    std::string str_salt;
    // avoid illegal character for the database insert
    sodium_bin2base64(c_salt, sizeof(c_salt), salt, sizeof(salt), sodium_base64_VARIANT_ORIGINAL);
    for(size_t i = 0; i < crypto_pwhash_SALTBYTES; i++){
        str_salt += static_cast<char>(salt[i]);
    }
    return c_salt;
    }


std::string Security::hashPwd(const std::string& salt_pwd){
    char hash_pwd[crypto_pwhash_STRBYTES];
    if (crypto_pwhash_str(hash_pwd, salt_pwd.c_str(), salt_pwd.length(),
                            crypto_pwhash_OPSLIMIT_INTERACTIVE, 
                            crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
        std::cout << "Hashing failed." << std::endl;
    }
    return std::string(hash_pwd);
}


bool Security::verifyPwd(const std::string& hash_pwd, const std::string& salt_pwd){
    if(crypto_pwhash_str_verify(hash_pwd.c_str(), salt_pwd.c_str(), salt_pwd.length())!= 0){
        return false;
    }
    return true;
}

