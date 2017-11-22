// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/**************************** usha.c ****************************/
/******************** See RFC 4634 for details ******************/
/*
*  Description:
*     This file implements a unified interface to the SHA algorithms.
*/

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "azure_c_shared_utility/gballoc.h"

#include "azure_c_shared_utility/sha.h"

/*
*  USHAReset
*
*  Description:
*      This function will initialize the SHA Context in preparation
*      for computing a new SHA message digest.
*
*  Parameters:
*      context: [in/out]
*          The context to reset.
*      whichSha: [in]
*          Selects which SHA reset to call
*
*  Returns:
*      sha Error Code.
*
*/
int USHAReset(USHAContext *ctx, enum SHAversion whichSha)
{
    if (ctx) {
        ctx->whichSha = whichSha;
        switch (whichSha) {
        case SHA1:   return SHA1Reset((SHA1Context*)&ctx->ctx);
        case SHA224: return SHA224Reset((SHA224Context*)&ctx->ctx);
        case SHA256: return SHA256Reset((SHA256Context*)&ctx->ctx);
        case SHA384: return SHA384Reset((SHA384Context*)&ctx->ctx);
        case SHA512: return SHA512Reset((SHA512Context*)&ctx->ctx);
        default: return shaBadParam;
        }
    }
    else {
        return shaNull;
    }
}

/*
*  USHAInput
*
*  Description:
*      This function accepts an array of octets as the next portion
*      of the message.
*
*  Parameters:
*      context: [in/out]
*          The SHA context to update
*      message_array: [in]
*          An array of characters representing the next portion of
*          the message.
*      length: [in]
*          The length of the message in message_array
*
*  Returns:
*      sha Error Code.
*
*/
int USHAInput(USHAContext *ctx,
    const uint8_t *bytes, unsigned int bytecount)
{
    if (ctx) {
        switch (ctx->whichSha) {
        case SHA1:
            return SHA1Input((SHA1Context*)&ctx->ctx, bytes, bytecount);
        case SHA224:
            return SHA224Input((SHA224Context*)&ctx->ctx, bytes,
                bytecount);
        case SHA256:
            return SHA256Input((SHA256Context*)&ctx->ctx, bytes,
                bytecount);
        case SHA384:
            return SHA384Input((SHA384Context*)&ctx->ctx, bytes,
                bytecount);
        case SHA512:
            return SHA512Input((SHA512Context*)&ctx->ctx, bytes,
                bytecount);
        default: return shaBadParam;
        }
    }
    else {
        return shaNull;
    }
}

/*
* USHAFinalBits
*
* Description:
*   This function will add in any final bits of the message.
*
* Parameters:
*   context: [in/out]
*     The SHA context to update
*   message_bits: [in]
*     The final bits of the message, in the upper portion of the
*     byte. (Use 0b###00000 instead of 0b00000### to input the
*     three bits ###.)
*   length: [in]
*     The number of bits in message_bits, between 1 and 7.
*
* Returns:
*   sha Error Code.
*/
int USHAFinalBits(USHAContext *ctx,
const uint8_t bits, unsigned int bitcount)
{
    if (ctx) {
        switch (ctx->whichSha) {
        case SHA1:
            return SHA1FinalBits((SHA1Context*)&ctx->ctx, bits, bitcount);
        case SHA224:
            return SHA224FinalBits((SHA224Context*)&ctx->ctx, bits,
                bitcount);
        case SHA256:
            return SHA256FinalBits((SHA256Context*)&ctx->ctx, bits,
                bitcount);
        case SHA384:
            return SHA384FinalBits((SHA384Context*)&ctx->ctx, bits,
                bitcount);
        case SHA512:
            return SHA512FinalBits((SHA512Context*)&ctx->ctx, bits,
                bitcount);
        default: return shaBadParam;
        }
    }
    else {
        return shaNull;
    }
}

/*
* USHAResult
*
* Description:
*   This function will return the 160-bit message digest into the
*   Message_Digest array provided by the caller.
*   NOTE: The first octet of hash is stored in the 0th element,
*      the last octet of hash in the 19th element.
*
* Parameters:
*   context: [in/out]
*     The context to use to calculate the SHA-1 hash.
*   Message_Digest: [out]
*     Where the digest is returned.
*
* Returns:
*   sha Error Code.
*
*/
int USHAResult(USHAContext *ctx,
    uint8_t Message_Digest[USHAMaxHashSize])
{
    if (ctx) {
        switch (ctx->whichSha) {
        case SHA1:
            return SHA1Result((SHA1Context*)&ctx->ctx, Message_Digest);
        case SHA224:
            return SHA224Result((SHA224Context*)&ctx->ctx, Message_Digest);
        case SHA256:
            return SHA256Result((SHA256Context*)&ctx->ctx, Message_Digest);
        case SHA384:
            return SHA384Result((SHA384Context*)&ctx->ctx, Message_Digest);
        case SHA512:
            return SHA512Result((SHA512Context*)&ctx->ctx, Message_Digest);
        default: return shaBadParam;
        }
    }
    else {
        return shaNull;
    }
}

/*
* USHABlockSize
*
* Description:
*   This function will return the blocksize for the given SHA
*   algorithm.
*
* Parameters:
*   whichSha:
*     which SHA algorithm to query
*
* Returns:
*   block size
*
*/
int USHABlockSize(enum SHAversion whichSha)
{
    switch (whichSha) {
    case SHA1:   return SHA1_Message_Block_Size;
    case SHA224: return SHA224_Message_Block_Size;
    case SHA256: return SHA256_Message_Block_Size;
    case SHA384: return SHA384_Message_Block_Size;
    default:
    case SHA512: return SHA512_Message_Block_Size;
    }
}

/*
* USHAHashSize
*
* Description:
*   This function will return the hashsize for the given SHA
*   algorithm.
*
* Parameters:
*   whichSha:
*     which SHA algorithm to query
*
* Returns:
*   hash size
*
*/
int USHAHashSize(enum SHAversion whichSha)
{
    switch (whichSha) {
    case SHA1:   return SHA1HashSize;
    case SHA224: return SHA224HashSize;
    case SHA256: return SHA256HashSize;
    case SHA384: return SHA384HashSize;
    default:
    case SHA512: return SHA512HashSize;
    }
}

/*
* USHAHashSizeBits
*
* Description:
*   This function will return the hashsize for the given SHA
*   algorithm, expressed in bits.
*
* Parameters:
*   whichSha:
*     which SHA algorithm to query
*
* Returns:
*   hash size in bits
*
*/
int USHAHashSizeBits(enum SHAversion whichSha)
{
    switch (whichSha) {
    case SHA1:   return SHA1HashSizeBits;
    case SHA224: return SHA224HashSizeBits;
    case SHA256: return SHA256HashSizeBits;
    case SHA384: return SHA384HashSizeBits;
    default:
    case SHA512: return SHA512HashSizeBits;
    }
}

