AM_CPPFLAGS                = $(UNIMRCP_PLUGIN_INCLUDES)

plugin_LTLIBRARIES         = mssynth.la

mssynth_la_SOURCES         = src/ms_synth_engine.cpp ../ms-common/src/config_manager.cpp
mssynth_la_LDFLAGS         = $(UNIMRCP_PLUGIN_OPTS) \
                             -L$(SPEECHSDK_ROOT)/lib/x64 \
                             -lpthread -lMicrosoft.CognitiveServices.Speech.core -lPocoFoundation

mssynth_ladir              = $(libdir)

include $(top_srcdir)/build/rules/uniplugin.am

UNIMRCP_PLUGIN_INCLUDES     += -I$(SPEECHSDK_ROOT)/include/cxx_api \
                               -I$(SPEECHSDK_ROOT)/include/c_api \
                               -I$(top_srcdir)/plugins/ms-common/include