STRING TOKENIZER Requirements
================

## Overview
The STRING TOKENIZER provides the functionality of splitting a STRING into multiples tokens

## Exposed API
```C
typedef struct STRING_TOKEN_TAG* STRING_TOKENIZER_HANDLE;
 
extern STRING_TOKENIZER_HANDLE STRING_TOKENIZER_create(STRING_HANDLE handle);
extern STRING_TOKENIZER_HANDLE STRING_TOKENIZER_create_from_char(const char* input);
extern int STRING_TOKENIZER_get_next_token(STRING_TOKENIZER_HANDLE t, STRING_HANDLE output, const char* delimiters);
extern void STRING_TOKENIZER_destroy(STRING_TOKENIZER_HANDLE t);
```
### STRING_TOKENIZER_create
extern STRING_TOKENIZER_HANDLE STRING_TOKENIZER_create(STRING_HANDLE handle);
**SRS_STRING_TOKENIZER_04_001: [**STRING_TOKENIZER_create shall return an NULL STRING_TOKENIZER_HANDLE if parameter handle is NULL**]**   
**SRS_STRING_TOKENIZER_04_002: [**STRING_TOKENIZER_create shall allocate a new STRING_TOKENIZER _HANDLE having the content of the STRING_HANDLE copied and current position pointing at the beginning of the string**]**   
**SRS_STRING_TOKENIZER_04_003: [**STRING_TOKENIZER_create shall return an NULL STRING_TOKENIZER _HANDLE on any error that is encountered**]**   

### STRING_TOKENIZER_create_from_char
extern STRING_TOKENIZER_HANDLE STRING_TOKENIZER_create_from_char(const char* input);
**SRS_STRING_TOKENIZER_07_001: [**STRING_TOKENIZER_create_from_char shall return an NULL STRING_TOKENIZER_HANDLE if parameter input is NULL**]**   
**SRS_STRING_TOKENIZER_07_002: [**STRING_TOKENIZER_create_from_char shall allocate a new STRING_TOKENIZER _HANDLE having the content of the STRING_HANDLE copied and current position pointing at the beginning of the string**]**   
**SRS_STRING_TOKENIZER_07_003: [**STRING_TOKENIZER_create_from_char shall return an NULL STRING_TOKENIZER _HANDLE on any error that is encountered**]**   

### STRING_TOKENIZER_get_next_token
extern int STRING_TOKENIZER_get_next_token(STRING_TOKENIZER_HANDLE t, STRING_HANDLE output,const char* delimiters); 
**SRS_STRING_TOKENIZER_04_004: [**STRING_TOKENIZER_get_next_token shall return a nonzero value if any of the 3 parameters is NULL**]**   
**SRS_STRING_TOKENIZER_04_005: [**STRING_TOKENIZER_get_next_token searches the string inside STRING_TOKENIZER_HANDLE for the first character that is NOT contained in the current delimiter**]**   
**SRS_STRING_TOKENIZER_04_006: [**If no such character is found, then STRING_TOKENIZER_get_next_token shall return a nonzero Value (You’ve reach the end of the string or the string consists with only delimiterstokens).**]**   
**SRS_STRING_TOKENIZER_04_007: [**If such a character is found, STRING_TOKENIZER_get_next_token shall consider it as the start of a token.**]**   
**SRS_STRING_TOKENIZER_04_008: [**STRING_TOKENIZER_get_next_token than searches from the start of a token for a character that is contained in the delimiters string.**]**   
**SRS_STRING_TOKENIZER_04_009: [**If no such character is found, STRING_TOKENIZER_get_next_token extends the current token to the end of the string inside t, copies the token to output and returns 0.**]**   
**SRS_STRING_TOKENIZER_04_010: [**If such a character is found, STRING_TOKENIZER_get_next_token shall consider it the end of the token and copy it’s content to output, updates the current position inside t to the next character and returns 0.**]**   
**SRS_STRING_TOKENIZER_04_011: [**Each subsequent call to STRING_TOKENIZER_get_next_token starts searching from the saved position on t and behaves as described above.**]**   
**SRS_STRING_TOKENIZER_TOKENIZER_04_014: [**STRING_TOKENIZER_get_next_token shall return nonzero value if t contains an empty string.**]**   

### STRING_TOKENIZER_destroy
extern void STRING_TOKENIZER_destroy(STRING_TOKENIZER_HANDLE t);  
**SRS_STRING_TOKENIZER_TOKENIZER_04_012: [**STRING_TOKENIZER_destroy shall free the memory allocated by the STRING_TOKENIZER_create **]**   
**SRS_STRING_TOKENIZER_TOKENIZER_04_013: [**When the t argument is NULL, then STRING_TOKENIZER_destroy shall not attempt to free**]**   

