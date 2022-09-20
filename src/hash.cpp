//
//  hash.cpp
//  eosclient
//
//  Created by Manuel Ramon Lopez Torres on 20/9/22.
//

#include <eosclient/Hash.hpp>
#include <openssl/sha.h>

using namespace onikami::eosclient;

bool Hash::SHA256(const Buffer &msg, Buffer &digest) {
    return Hash::hash(msg, digest, EVP_sha256());
}

bool Hash::SHA512(const Buffer &msg, Buffer &digest) {
    return Hash::hash(msg, digest, EVP_sha512());
}

bool Hash::hash(const Buffer &msg, Buffer &digest, const EVP_MD *algorithm) {
    unsigned char *buffer;
    unsigned int bufferLength;
    EVP_MD_CTX *context;
    bool result = false;

    if ((context = EVP_MD_CTX_new()) == NULL)
        goto exitSHA;

    if (!EVP_DigestInit_ex(context, algorithm, NULL))
        goto exitSHA;

    if (!EVP_DigestUpdate(context, (unsigned char *)&msg[0], msg.size()))
        goto exitSHA;

    if ((buffer = (unsigned char *)OPENSSL_malloc(EVP_MD_size(algorithm))) == NULL)
        goto exitSHA;

    if (!EVP_DigestFinal_ex(context, buffer, &bufferLength))
        goto exitSHA;

    digest.assign(buffer, buffer + bufferLength);

    result = true;

exitSHA:
    /* Clean up */
    if (buffer && !result)
        OPENSSL_free(buffer);
    if (context)
        EVP_MD_CTX_free(context);

    return result;
}
