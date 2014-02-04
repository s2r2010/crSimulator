#
# OMNeT++/OMNEST Makefile for crSimulator
#
# This file was generated with the command:
#  opp_makemake -f --deep -I/usr/include/mysql -I/usr/include
#

# Name of target to be created (-o option)
TARGET = crSimulator$(EXE_SUFFIX)

# User interface (uncomment one) (-u option)
USERIF_LIBS = $(ALL_ENV_LIBS) # that is, $(TKENV_LIBS) $(CMDENV_LIBS)
#USERIF_LIBS = $(CMDENV_LIBS)
#USERIF_LIBS = $(TKENV_LIBS)

# C++ include paths (with -I)
INCLUDE_PATH = \
    -I/usr/include/mysql \
    -I/usr/include \
    -I. \
    -Icore \
    -Icore/message-files \
    -IcrModules \
    -IcrModules/AppLayer \
    -IcrModules/Battery \
    -IcrModules/CognitiveEngine \
    -IcrModules/CrNetworks \
    -IcrModules/CrNodes \
    -IcrModules/DRM \
    -IcrModules/MacLayer \
    -IcrModules/MobilityComponent \
    -IcrModules/NetLayer \
    -IcrModules/PhyLayer \
    -IcrModules/PuNodes \
    -IcrModules/RFSpectrum \
    -IcrModules/SigCommLink \
    -IcrModules/SpectrumSensor \
    -IcrModules/Statistics

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS =

# Output directory
PROJECT_OUTPUT_DIR = ../out
PROJECTRELATIVE_PATH = src
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc and .msg files
OBJS = \
    $O/core/BaseAppLayer.o \
    $O/core/BaseCognitiveEngine.o \
    $O/core/BaseCrBattery.o \
    $O/core/BaseDrm.o \
    $O/core/BaseMacLayer.o \
    $O/core/BaseMobilityComponent.o \
    $O/core/BaseNetLayer.o \
    $O/core/BasePhyLayer.o \
    $O/core/BasePuNode.o \
    $O/core/BaseScl.o \
    $O/core/BaseSpecSensor.o \
    $O/core/BaseStats.o \
    $O/crModules/AppLayer/crAppLayer.o \
    $O/crModules/AppLayer/testApp.o \
    $O/crModules/Battery/crBattery.o \
    $O/crModules/CognitiveEngine/crCognitiveEngine.o \
    $O/crModules/DRM/testDRM.o \
    $O/crModules/MacLayer/cr80211b.o \
    $O/crModules/MacLayer/crMacLayer.o \
    $O/crModules/MacLayer/drmMacLayer.o \
    $O/crModules/MacLayer/MobilityMacLayer.o \
    $O/crModules/MacLayer/switchMac.o \
    $O/crModules/MobilityComponent/MobilityComponent.o \
    $O/crModules/NetLayer/crNetLayer.o \
    $O/crModules/PhyLayer/crPhyLayer.o \
    $O/crModules/PuNodes/puGSM.o \
    $O/crModules/PuNodes/puISM.o \
    $O/crModules/PuNodes/puTest.o \
    $O/crModules/RFSpectrum/datarateSpectrum.o \
    $O/crModules/RFSpectrum/spectrum.o \
    $O/crModules/RFSpectrum/suChannel.o \
    $O/crModules/SigCommLink/crScl.o \
    $O/crModules/SpectrumSensor/crSpecSensor.o \
    $O/crModules/SpectrumSensor/drmSpecSensor.o \
    $O/crModules/SpectrumSensor/MobilitySpecSensor.o \
    $O/crModules/Statistics/crStats.o \
    $O/core/message-files/ctrlMsg_m.o \
    $O/core/message-files/dataMsg_m.o \
    $O/core/message-files/mobilityMsg_m.o \
    $O/core/message-files/timerMsg_m.o

# Message files
MSGFILES = \
    core/message-files/ctrlMsg.msg \
    core/message-files/dataMsg.msg \
    core/message-files/mobilityMsg.msg \
    core/message-files/timerMsg.msg

#------------------------------------------------------------------------------

# Pull in OMNeT++ configuration (Makefile.inc or configuser.vc)

ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
ifneq ("$(OMNETPP_ROOT)","")
CONFIGFILE = $(OMNETPP_ROOT)/Makefile.inc
else
CONFIGFILE = $(shell opp_configfilepath)
endif
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)

# Simulation kernel and user interface libraries
OMNETPP_LIB_SUBDIR = $(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)
OMNETPP_LIBS = -L"$(OMNETPP_LIB_SUBDIR)" -L"$(OMNETPP_LIB_DIR)" -loppmain$D $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)

COPTS = $(CFLAGS)  $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH)

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE and have object
# files depend on it (except when "make depend" was called)
COPTS_FILE = $O/.last-copts
ifneq ($(MAKECMDGOALS),depend)
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
$(shell $(MKPATH) "$O" && echo "$(COPTS)" >$(COPTS_FILE))
endif
endif

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
# >>>
# <<<
#------------------------------------------------------------------------------

# Main target
all: $O/$(TARGET)
	$(Q)$(LN) $O/$(TARGET) .

$O/$(TARGET): $(OBJS)  $(wildcard $(EXTRA_OBJS)) Makefile
	@$(MKPATH) $O
	@echo Creating executable: $@
	$(Q)$(CXX) $(LDFLAGS) -o $O/$(TARGET)  $(OBJS) $(EXTRA_OBJS) $(AS_NEEDED_OFF) $(WHOLE_ARCHIVE_ON) $(LIBS) $(WHOLE_ARCHIVE_OFF) $(OMNETPP_LIBS)

.PHONY: all clean cleanall depend msgheaders

.SUFFIXES: .cc

$O/%.o: %.cc $(COPTS_FILE)
	@$(MKPATH) $(dir $@)
	$(qecho) "$<"
	$(Q)$(CXX) -c $(COPTS) -o $@ $<

%_m.cc %_m.h: %.msg
	$(qecho) MSGC: $<
	$(Q)$(MSGC) -s _m.cc $(MSGCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)

clean:
	$(qecho) Cleaning...
	$(Q)-rm -rf $O
	$(Q)-rm -f crSimulator crSimulator.exe libcrSimulator.so libcrSimulator.a libcrSimulator.dll libcrSimulator.dylib
	$(Q)-rm -f ./*_m.cc ./*_m.h
	$(Q)-rm -f core/*_m.cc core/*_m.h
	$(Q)-rm -f core/message-files/*_m.cc core/message-files/*_m.h
	$(Q)-rm -f crModules/*_m.cc crModules/*_m.h
	$(Q)-rm -f crModules/AppLayer/*_m.cc crModules/AppLayer/*_m.h
	$(Q)-rm -f crModules/Battery/*_m.cc crModules/Battery/*_m.h
	$(Q)-rm -f crModules/CognitiveEngine/*_m.cc crModules/CognitiveEngine/*_m.h
	$(Q)-rm -f crModules/CrNetworks/*_m.cc crModules/CrNetworks/*_m.h
	$(Q)-rm -f crModules/CrNodes/*_m.cc crModules/CrNodes/*_m.h
	$(Q)-rm -f crModules/DRM/*_m.cc crModules/DRM/*_m.h
	$(Q)-rm -f crModules/MacLayer/*_m.cc crModules/MacLayer/*_m.h
	$(Q)-rm -f crModules/MobilityComponent/*_m.cc crModules/MobilityComponent/*_m.h
	$(Q)-rm -f crModules/NetLayer/*_m.cc crModules/NetLayer/*_m.h
	$(Q)-rm -f crModules/PhyLayer/*_m.cc crModules/PhyLayer/*_m.h
	$(Q)-rm -f crModules/PuNodes/*_m.cc crModules/PuNodes/*_m.h
	$(Q)-rm -f crModules/RFSpectrum/*_m.cc crModules/RFSpectrum/*_m.h
	$(Q)-rm -f crModules/SigCommLink/*_m.cc crModules/SigCommLink/*_m.h
	$(Q)-rm -f crModules/SpectrumSensor/*_m.cc crModules/SpectrumSensor/*_m.h
	$(Q)-rm -f crModules/Statistics/*_m.cc crModules/Statistics/*_m.h

cleanall: clean
	$(Q)-rm -rf $(PROJECT_OUTPUT_DIR)

depend:
	$(qecho) Creating dependencies...
	$(Q)$(MAKEDEPEND) $(INCLUDE_PATH) -f Makefile -P\$$O/ -- $(MSG_CC_FILES)  ./*.cc core/*.cc core/message-files/*.cc crModules/*.cc crModules/AppLayer/*.cc crModules/Battery/*.cc crModules/CognitiveEngine/*.cc crModules/CrNetworks/*.cc crModules/CrNodes/*.cc crModules/DRM/*.cc crModules/MacLayer/*.cc crModules/MobilityComponent/*.cc crModules/NetLayer/*.cc crModules/PhyLayer/*.cc crModules/PuNodes/*.cc crModules/RFSpectrum/*.cc crModules/SigCommLink/*.cc crModules/SpectrumSensor/*.cc crModules/Statistics/*.cc

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/core/BaseAppLayer.o: core/BaseAppLayer.cc \
	core/BaseAppLayer.h \
	core/Logging.h
$O/core/BaseCognitiveEngine.o: core/BaseCognitiveEngine.cc \
	core/BaseCognitiveEngine.h
$O/core/BaseCrBattery.o: core/BaseCrBattery.cc \
	core/BaseCrBattery.h
$O/core/BaseDrm.o: core/BaseDrm.cc \
	core/BaseDrm.h
$O/core/BaseMacLayer.o: core/BaseMacLayer.cc \
	core/BaseMacLayer.h \
	core/Logging.h
$O/core/BaseMobilityComponent.o: core/BaseMobilityComponent.cc \
	core/BaseMobilityComponent.h \
	core/Logging.h
$O/core/BaseNetLayer.o: core/BaseNetLayer.cc \
	core/BaseNetLayer.h \
	core/Logging.h
$O/core/BasePhyLayer.o: core/BasePhyLayer.cc \
	core/BasePhyLayer.h
$O/core/BasePuNode.o: core/BasePuNode.cc \
	core/BasePuNode.h \
	core/message-files/ctrlMsg_m.h \
	core/message-files/dataMsg_m.h \
	core/message-files/timerMsg_m.h
$O/core/BaseScl.o: core/BaseScl.cc \
	core/BaseScl.h
$O/core/BaseSpecSensor.o: core/BaseSpecSensor.cc \
	core/BaseSpecSensor.h
$O/core/BaseStats.o: core/BaseStats.cc \
	core/BaseStats.h
$O/core/message-files/ctrlMsg_m.o: core/message-files/ctrlMsg_m.cc \
	core/message-files/ctrlMsg_m.h
$O/core/message-files/dataMsg_m.o: core/message-files/dataMsg_m.cc \
	core/message-files/dataMsg_m.h
$O/core/message-files/mobilityMsg_m.o: core/message-files/mobilityMsg_m.cc \
	core/message-files/mobilityMsg_m.h
$O/core/message-files/timerMsg_m.o: core/message-files/timerMsg_m.cc \
	core/message-files/timerMsg_m.h
$O/crModules/AppLayer/crAppLayer.o: crModules/AppLayer/crAppLayer.cc \
	core/BaseAppLayer.h \
	core/Logging.h \
	core/message-files/ctrlMsg_m.h \
	core/message-files/dataMsg_m.h \
	crModules/AppLayer/crAppLayer.h
$O/crModules/AppLayer/testApp.o: crModules/AppLayer/testApp.cc \
	crModules/AppLayer/testApp.h
$O/crModules/Battery/crBattery.o: crModules/Battery/crBattery.cc \
	core/BaseCrBattery.h \
	crModules/Battery/crBattery.h
$O/crModules/CognitiveEngine/crCognitiveEngine.o: crModules/CognitiveEngine/crCognitiveEngine.cc \
	core/message-files/ctrlMsg_m.h \
	core/message-files/dataMsg_m.h \
	crModules/CognitiveEngine/crCognitiveEngine.h
$O/crModules/DRM/testDRM.o: crModules/DRM/testDRM.cc \
	core/message-files/ctrlMsg_m.h \
	core/message-files/dataMsg_m.h \
	core/message-files/timerMsg_m.h \
	crModules/DRM/testDRM.h
$O/crModules/MacLayer/MobilityMacLayer.o: crModules/MacLayer/MobilityMacLayer.cc \
	core/BaseMacLayer.h \
	core/Logging.h \
	core/message-files/ctrlMsg_m.h \
	core/message-files/dataMsg_m.h \
	core/message-files/mobilityMsg_m.h \
	core/message-files/timerMsg_m.h \
	crModules/MacLayer/MobilityMacLayer.h
$O/crModules/MacLayer/cr80211b.o: crModules/MacLayer/cr80211b.cc \
	core/BaseMacLayer.h \
	core/Logging.h \
	core/message-files/ctrlMsg_m.h \
	core/message-files/dataMsg_m.h \
	core/message-files/timerMsg_m.h \
	crModules/MacLayer/cr80211b.h
$O/crModules/MacLayer/crMacLayer.o: crModules/MacLayer/crMacLayer.cc \
	core/BaseMacLayer.h \
	core/Logging.h \
	core/message-files/ctrlMsg_m.h \
	core/message-files/dataMsg_m.h \
	core/message-files/timerMsg_m.h \
	crModules/MacLayer/crMacLayer.h
$O/crModules/MacLayer/drmMacLayer.o: crModules/MacLayer/drmMacLayer.cc \
	core/BaseMacLayer.h \
	core/Logging.h \
	core/message-files/ctrlMsg_m.h \
	core/message-files/dataMsg_m.h \
	core/message-files/timerMsg_m.h \
	crModules/MacLayer/drmMacLayer.h
$O/crModules/MacLayer/switchMac.o: crModules/MacLayer/switchMac.cc \
	core/BaseMacLayer.h \
	core/Logging.h \
	core/message-files/ctrlMsg_m.h \
	core/message-files/dataMsg_m.h \
	core/message-files/timerMsg_m.h \
	crModules/MacLayer/switchMac.h
$O/crModules/MobilityComponent/MobilityComponent.o: crModules/MobilityComponent/MobilityComponent.cc \
	core/BaseMobilityComponent.h \
	core/Logging.h \
	core/message-files/ctrlMsg_m.h \
	core/message-files/mobilityMsg_m.h \
	crModules/MobilityComponent/MobilityComponent.h
$O/crModules/NetLayer/crNetLayer.o: crModules/NetLayer/crNetLayer.cc \
	core/BaseNetLayer.h \
	core/Logging.h \
	core/message-files/ctrlMsg_m.h \
	core/message-files/dataMsg_m.h \
	crModules/NetLayer/crNetLayer.h
$O/crModules/PhyLayer/crPhyLayer.o: crModules/PhyLayer/crPhyLayer.cc \
	core/message-files/ctrlMsg_m.h \
	core/message-files/dataMsg_m.h \
	core/message-files/timerMsg_m.h \
	crModules/PhyLayer/crPhyLayer.h
$O/crModules/PuNodes/puGSM.o: crModules/PuNodes/puGSM.cc \
	core/Logging.h \
	core/message-files/dataMsg_m.h \
	core/message-files/timerMsg_m.h \
	crModules/PuNodes/puGSM.h
$O/crModules/PuNodes/puISM.o: crModules/PuNodes/puISM.cc \
	core/BasePuNode.h \
	core/Logging.h \
	core/message-files/ctrlMsg_m.h \
	core/message-files/dataMsg_m.h \
	core/message-files/timerMsg_m.h \
	crModules/PuNodes/puISM.h
$O/crModules/PuNodes/puTest.o: crModules/PuNodes/puTest.cc \
	core/BasePuNode.h \
	core/Logging.h \
	core/message-files/ctrlMsg_m.h \
	core/message-files/dataMsg_m.h \
	core/message-files/timerMsg_m.h \
	crModules/PuNodes/puTest.h
$O/crModules/RFSpectrum/datarateSpectrum.o: crModules/RFSpectrum/datarateSpectrum.cc \
	crModules/RFSpectrum/datarateSpectrum.h
$O/crModules/RFSpectrum/spectrum.o: crModules/RFSpectrum/spectrum.cc \
	core/message-files/dataMsg_m.h \
	crModules/RFSpectrum/spectrum.h
$O/crModules/RFSpectrum/suChannel.o: crModules/RFSpectrum/suChannel.cc \
	core/message-files/dataMsg_m.h \
	crModules/RFSpectrum/suChannel.h
$O/crModules/SigCommLink/crScl.o: crModules/SigCommLink/crScl.cc \
	core/BaseScl.h \
	core/message-files/ctrlMsg_m.h \
	core/message-files/dataMsg_m.h \
	core/message-files/timerMsg_m.h \
	crModules/SigCommLink/crScl.h
$O/crModules/SpectrumSensor/MobilitySpecSensor.o: crModules/SpectrumSensor/MobilitySpecSensor.cc \
	core/BaseSpecSensor.h \
	core/Logging.h \
	core/message-files/ctrlMsg_m.h \
	core/message-files/dataMsg_m.h \
	core/message-files/timerMsg_m.h \
	crModules/SpectrumSensor/MobilitySpecSensor.h
$O/crModules/SpectrumSensor/crSpecSensor.o: crModules/SpectrumSensor/crSpecSensor.cc \
	core/BaseSpecSensor.h \
	core/message-files/ctrlMsg_m.h \
	core/message-files/dataMsg_m.h \
	core/message-files/timerMsg_m.h \
	crModules/SpectrumSensor/crSpecSensor.h
$O/crModules/SpectrumSensor/drmSpecSensor.o: crModules/SpectrumSensor/drmSpecSensor.cc \
	core/BaseSpecSensor.h \
	core/message-files/ctrlMsg_m.h \
	core/message-files/dataMsg_m.h \
	core/message-files/timerMsg_m.h \
	crModules/SpectrumSensor/drmSpecSensor.h
$O/crModules/Statistics/crStats.o: crModules/Statistics/crStats.cc \
	core/BaseStats.h \
	crModules/Statistics/crStats.h

