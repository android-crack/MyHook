//
// Created by killer on 4/28/17.
//

#ifndef U3DHOOK_XXTEA_H
#define U3DHOOK_XXTEA_H

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Function: xxtea_encrypt
 * @data:    Data to be encrypted
 * @len:     Length of the data to be encrypted
 * @key:     Symmetric key
 * @out_len: Pointer to output length variable
 * Returns:  Encrypted data or %NULL on failure
 *
 * Caller is responsible for freeing the returned buffer.
 */
void * xxtea_encrypt(const void * data, size_t len, const void * key, size_t * out_len);

/**
 * Function: xxtea_decrypt
 * @data:    Data to be decrypted
 * @len:     Length of the data to be decrypted
 * @key:     Symmetric key
 * @out_len: Pointer to output length variable
 * Returns:  Decrypted data or %NULL on failure
 *
 * Caller is responsible for freeing the returned buffer.
 */
void * xxtea_decrypt(const void * data, size_t len, const void * key, size_t * out_len);

#ifdef __cplusplus
}
#endif

#endif //U3DHOOK_XXTEA_H
