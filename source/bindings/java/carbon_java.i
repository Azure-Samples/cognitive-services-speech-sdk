%module(directors="1") carbon_java

%rename("%(firstlowercase)s", %$isfunction) "";
%rename("%(firstlowercase)s", %$ismember) "";

%typemap(directorin, descriptor="[B") (char *STRING, unsigned int LENGTH) {
   jbyteArray jb = (jenv)->NewByteArray($2);
   (jenv)->SetByteArrayRegion(jb, 0, $2, (jbyte *)$1);
   $input = jb;
}

%typemap(directorargout) (char *STRING, unsigned int LENGTH)
%{
    (jenv)->GetByteArrayRegion($input, 0, $2, (jbyte *)$1);
%}

%{
#ifdef _MSC_VER
#pragma warning(disable : 4267) //warning C4267: 'initializing': conversion from 'size_t' to 'jsize', possible loss of data
// disable: error 6011:  : Dereferencing NULL pointer 'arg1'.
#pragma warning( disable : 6011 )
#endif
%}

%{
#include <speechapi_cxx_common.h>
#include "speechapi_cxx_audioinputstream.h"
%}

%feature("director") AudioInputStream;
%apply (char *STRING, int LENGTH) { (char* dataBuffer, int size) };

%include "carbon.i"


// cmake  -G "NMake Makefiles" -DCMAKE_SYSTEM_NAME=Android  -DCMAKE_ANDROID_NDK=a:/android-ndk-r16b    -DCMAKE_SYSTEM_VERSION=26 .. -DCMAKE_ANDROID_STL_TYPE=c++_shared -DJAVA_AWT_LIBRARY=a:\android-ndk-r16b\sysroot\usr\include\  -DOPENSSL_ROOT_DIR=a:\Carbon\external\openssl\_install  -DCURL_LIBRARY=a:\Carbon\external\curl\_install\lib\libcurl.so -DCURL_INCLUDE_DIR=a:\Carbon\external\curl\_install\include -Duse_default_uuid=ON
