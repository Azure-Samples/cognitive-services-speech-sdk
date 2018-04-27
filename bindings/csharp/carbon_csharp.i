%module(directors="1") carbon_csharp

// make swig generated classes internal only.
%typemap(csclassmodifiers) SWIGTYPE "internal class"

#define SWIG_CSHARP_NO_WSTRING_HELPER

%include <std_wstring.i>
%include <std_vector.i>
%include <std_map.i>

%define SWIGCSHARP_IMTYPE_WSTRING(TYPENAME)
%typemap(imtype,
         inattributes="[System.Runtime.InteropServices.MarshalAs(global::System.Runtime.InteropServices.UnmanagedType.LPWStr)]",
         outattributes="[return: System.Runtime.InteropServices.MarshalAs(global::System.Runtime.InteropServices.UnmanagedType.BStr)]"
        ) TYPENAME "string"
%enddef

%insert(runtime) %{
/* Callback for returning strings to C# without leaking memory */
typedef void * (SWIGSTDCALL* SWIG_CSharpWStringHelperCallback)(const wchar_t *);
static SWIG_CSharpWStringHelperCallback SWIG_csharp_wstring_callback = NULL;
%}

%pragma(csharp) imclasscode=%{
  protected class SWIGWStringHelper {

    [return: System.Runtime.InteropServices.MarshalAs(global::System.Runtime.InteropServices.UnmanagedType.BStr)]
    public delegate string SWIGWStringDelegate(global::System.IntPtr message);

    static SWIGWStringDelegate wstringDelegate = new SWIGWStringDelegate(CreateWString);

    [global::System.Runtime.InteropServices.DllImport("$dllimport", EntryPoint="SWIGRegisterWStringCallback_$module")]
    public static extern void SWIGRegisterWStringCallback_$module(SWIGWStringDelegate wstringDelegate);

    [return: System.Runtime.InteropServices.MarshalAs(global::System.Runtime.InteropServices.UnmanagedType.BStr)]
    static string CreateWString([global::System.Runtime.InteropServices.MarshalAs(global::System.Runtime.InteropServices.UnmanagedType.LPWStr)]global::System.IntPtr cString) {
      return global::System.Runtime.InteropServices.Marshal.PtrToStringUni(cString);
    }

    static SWIGWStringHelper() {
      SWIGRegisterWStringCallback_$module(wstringDelegate);
    }
  }

  static protected SWIGWStringHelper swigWStringHelper = new SWIGWStringHelper();
%}

%insert(runtime) %{
#ifdef __cplusplus
extern "C"
#endif
SWIGEXPORT void SWIGSTDCALL SWIGRegisterWStringCallback_$module(SWIG_CSharpWStringHelperCallback callback) {
  SWIG_csharp_wstring_callback = callback;
}
%}

SWIGCSHARP_IMTYPE_WSTRING(std::wstring)
SWIGCSHARP_IMTYPE_WSTRING(std::wstring&)
SWIGCSHARP_IMTYPE_WSTRING(const std::wstring&)
SWIGCSHARP_IMTYPE_WSTRING(std::wstring*)
SWIGCSHARP_IMTYPE_WSTRING(const std::wstring*)
SWIGCSHARP_IMTYPE_WSTRING(wchar_t*)
SWIGCSHARP_IMTYPE_WSTRING(const wchar_t*)

SWIG_STD_VECTOR_ENHANCED(std::vector<std::wstring>)
%template(UnsignedCharVector) std::vector<unsigned char>;
%template(StdMapWStringWString) std::map<std::wstring, std::wstring>;

%ignore operator bool;
%ignore operator();

%include "carbon.i"
