### Setting Erika's variables:
# ERIKA_FILES= ../../..
# APPBASE = .
# 
# EEOPT =


RTDRUID_CONFIGURATOR_NUMBER:=1273



############################################################################
#
# User options
#
############################################################################
EEOPT += DEBUG


############################################################################
#
# Automatic options
#
############################################################################
EEOPT += __RTD_LINUX__
EEOPT += __PIC30__
EEOPT += __33FJ256MC710__
EEOPT += __PIC33FJ256MC710__
EEOPT += __EE_FLEX__
EEOPT += __USE_LEDS__
EEOPT += __MICROCHIP_DSPIC30__
EEOPT += __PIC30_SPLIM__
EEOPT += __EDF__
EEOPT += __ALARMS__
EEOPT += __MONO__
EEOPT += __ALLOW_NESTED_IRQ__
EEOPT += __DISABLE_EEOPT_DEFINES__


############################################################################
#
# Erika base directory
#
############################################################################
ifdef ERIKA_FILES
ifdef EEBASE
    $(warning EEBASE is set, but it has been overridden by ERIKA_FILES)
endif
EEBASE := ${ERIKA_FILES}

else # ERIKA_FILES

ifndef EEBASE
        EEBASE := /home/student/.eclipse/org.eclipse.platform_3.5.0_155965261/plugins/com.eu.evidence.ee_1.6.0.20111019_2125/ee_files
else
        $(warning The usage of EEBASE is deprecated. Please use ERIKA_FILES)
endif
endif # ERIKA_FILES
# ERIKA_FILES has fulfilled its role. Make sure it's not used inside Erika makefiles
ERIKA_FILES :=


############################################################################
#
# PIC 30
#
############################################################################
APPBASE := 
OUTBASE := Debug

PIC30_ASMDIR := /opt/microchip/mplabx/asm30
PIC30_GCCDIR := /opt/microchip/mplabc30/v3.30c


############################################################################
#
# Init MCU for PIC30 (PIC33FJ256MC710)
#
############################################################################
PIC30_MODEL           := 33FJ256MC710
PIC30_LINKERSCRIPT    := p33FJ256MC710.gld
PIC30_DEV_LIB         := libp33FJ256MC710-elf.a
PIC30_INCLUDE_C       := p33FJ256MC710.h
PIC30_INCLUDE_S       := p33FJ256MC710.inc


############################################################################
#
# Flags
#
############################################################################
CFLAGS  += 
ASFLAGS += 
LDFLAGS += 
LDDEPS  += 
LIBS    += 



############################################################################
#
# Sources
#
############################################################################
APP_SRCS += $(OUTBASE)/eecfg.c setup.c e_can1.c code.c


############################################################################
#
# end
#
############################################################################
include $(EEBASE)/pkg/cfg/rules.mk
