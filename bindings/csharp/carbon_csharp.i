%module(directors="1") carbon_csharp

// make swig generated classes internal only.
%typemap(csclassmodifiers) SWIGTYPE "internal class"

%include "std_unordered_map.i"
%include "carbon.i"
