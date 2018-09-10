%module(directors="1") carbon_csharp

// make swig generated classes internal only.
%typemap(csclassmodifiers) SWIGTYPE "internal class"

%include "std_string.i"
%include "arrays_csharp.i"

%{
#include <speechapi_cxx_common.h>
#include "speechapi_cxx_audio_stream.h"
%}

// only apply to those named "dataBuffer"
%apply char* { uint8_t* dataBuffer }
%typemap(imtype,
         directoroutattributes="[FIXME_NOT_NEEDED /*left in swig to fast-fail in case new code requires new marshalling*/]",
         directorinattributes="[System.Runtime.InteropServices.MarshalAs(System.Runtime.InteropServices.UnmanagedType.LPArray, ArraySubType = System.Runtime.InteropServices.UnmanagedType.U1, SizeParamIndex = 1)][System.Runtime.InteropServices.Out][System.Runtime.InteropServices.In]"
        ) uint8_t* dataBuffer "byte[]"
%typemap(cstype) uint8_t* dataBuffer "byte[]"
%typemap(in) uint8_t* dataBuffer %{ $1 = ($1_ltype)$input; %}
%typemap(directorin) uint8_t* dataBuffer %{ $input = (char*)$1; %}
%typemap(csin) uint8_t *dataBuffer "$csinput"

%feature("director") PullAudioInputStreamCallback;

%include <std_vector.i>

%define SWIGCSHARP_IMTYPE_STRING(TYPENAME)
%typemap(imtype,
         inattributes="
                        #if USE_UTF8
                        [System.Runtime.InteropServices.MarshalAs(global::System.Runtime.InteropServices.UnmanagedType.LPStr)]
                        #else
                        [System.Runtime.InteropServices.MarshalAs(global::System.Runtime.InteropServices.UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(Utf8StringMarshaler))]
                        #endif
                      ",
         outattributes="
                        #if USE_UTF8
                        [return: System.Runtime.InteropServices.MarshalAs(global::System.Runtime.InteropServices.UnmanagedType.LPStr)]
                        #else
                        [return: System.Runtime.InteropServices.MarshalAs(global::System.Runtime.InteropServices.UnmanagedType.BStr)]
                        #endif
                       "
        ) TYPENAME "string"
%enddef


%insert(runtime) %{
typedef char* (SWIGSTDCALL* SWIG_CSharpStringHelperCallback)(const char *);
static SWIG_CSharpStringHelperCallback SWIG_csharp_string_callback = NULL;
%}


%pragma(csharp) imclasscode=%{

  protected class SWIGStringHelper {

    #if USE_UTF8
    [return: System.Runtime.InteropServices.MarshalAs(global::System.Runtime.InteropServices.UnmanagedType.LPStr)]
    public delegate string SWIGStringDelegate([System.Runtime.InteropServices.MarshalAs(global::System.Runtime.InteropServices.UnmanagedType.LPStr)] string message);
    #else
    [return: System.Runtime.InteropServices.MarshalAs(global::System.Runtime.InteropServices.UnmanagedType.BStr)]
    public delegate string SWIGStringDelegate([System.Runtime.InteropServices.MarshalAs(global::System.Runtime.InteropServices.UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(Utf8StringMarshaler))] string message);
    #endif

    static SWIGStringDelegate stringDelegate = new SWIGStringDelegate(CreateString);

    [global::System.Runtime.InteropServices.DllImport("Microsoft.CognitiveServices.Speech.csharp.bindings.dll", EntryPoint="SWIGRegisterStringCallback_carbon_csharp")]
    public static extern void SWIGRegisterStringCallback_carbon_csharp(SWIGStringDelegate stringDelegate);

    static string CreateString(string cString) {
      return cString;
    }

    static SWIGStringHelper() {
      SWIGRegisterStringCallback_carbon_csharp(stringDelegate);
    }
  }

  static protected SWIGStringHelper swigStringHelper = new SWIGStringHelper();
%}


%insert(runtime) %{
#ifdef __cplusplus
extern "C"
#endif
SWIGEXPORT void SWIGSTDCALL SWIGRegisterStringCallback_carbon_csharp(SWIG_CSharpStringHelperCallback callback) {
  SWIG_csharp_string_callback = callback;
}
%}


SWIGCSHARP_IMTYPE_STRING(std::string)
SWIGCSHARP_IMTYPE_STRING(std::string&)
SWIGCSHARP_IMTYPE_STRING(const std::string&)
SWIGCSHARP_IMTYPE_STRING(std::string*)
SWIGCSHARP_IMTYPE_STRING(const std::string*)
SWIGCSHARP_IMTYPE_STRING(char_t*)
SWIGCSHARP_IMTYPE_STRING(const char*)

SWIG_STD_VECTOR_ENHANCED(std::vector<std::string>)
%template(UnsignedCharVector) std::vector<unsigned char>;

%ignore operator bool;
%ignore operator();

%include "carbon.i"
