%module(directors="1") carbon_csharp

// make swig generated classes internal only.
%typemap(csclassmodifiers) SWIGTYPE "internal class"

%include "carbon.i"
