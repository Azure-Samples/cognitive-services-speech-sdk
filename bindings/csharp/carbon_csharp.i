%module(directors="1") carbon_csharp

// make swig generated classes internal only.
%typemap(csclassmodifiers) SWIGTYPE "internal class"

%include <std_wstring.i>
%include <std_vector.i>
%include <std_map.i>

SWIG_STD_VECTOR_ENHANCED(std::vector<std::wstring>)
%template(UnsignedCharVector) std::vector<unsigned char>;
%template(StdMapWStringWString) std::map<std::wstring, std::wstring>;

%ignore operator bool;
%ignore operator();

%include "carbon.i"
