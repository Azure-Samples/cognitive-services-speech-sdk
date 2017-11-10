#pragma once

typedef unsigned int SPXHR;

#define SPX_NOERROR                 0x00000000;
#define SPX_INIT_HR(hr)	            SPXHR hr = SPX_NOERROR

#define SPX_SUCCEEDED(x)            ((x & 0x80000000) == 0)
#define SPX_FAILED(x)               (!SPX_SUCCEEDED(x))   

#define __SPX_ERRCODE_SUCCEEDED(x)  (0x000f5000 | (x & 0x0fff))
#define __SPX_ERRCODE_FAILED(x)     (0x800f5000 | (x & 0x0fff))


/*** SPXERR_NOT_IMPL
*   The function is not implemented.
*/
#define SPXERR_NOT_IMPL             __SPX_ERRCODE_FAILED(0xfff)

/*** SPXERR_UNINITIALIZED
*   The object has not been properly initialized.
*/
#define SPXERR_UNINITIALIZED        __SPX_ERRCODE_FAILED(0x001)

/*** SPXERR_ALREADY_INITIALIZED
*   The object has already been initialized.
*/
#define SPXERR_ALREADY_INITIALIZED  __SPX_ERRCODE_FAILED(0x002)

/*** SPXERR_UNHANDLED_EXCEPTION
*   The object has already been initialized.
*/
#define SPXERR_UNHANDLED_EXCEPTION  __SPX_ERRCODE_FAILED(0x003)

/*** SPXERR_NOT_FOUND
*   The object or property was not found.
*/
#define SPXERR_NOT_FOUND            __SPX_ERRCODE_FAILED(0x004)

/*** SPXERR_INVALID_ARG
*   One or more arguments are not valid.
*/
#define SPXERR_INVALID_ARG          __SPX_ERRCODE_FAILED(0x004)
