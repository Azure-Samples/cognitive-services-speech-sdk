%module carbon_csharp

// TODO: #1137570
%ignore CallbackWrapper;
%ignore Carbon::EventSignal::operator();

%include "carbon.i"
