
ifneq ($(SYSROOT),)
	CFG_SYOPTS := $(CFG_SYOPTS) --sysroot=$(SYSROOT)
endif

ifneq ($(SYSHEADERS),)
	CFG_SYOPTS := $(CFG_SYOPTS) --headers=$(SYSHEADERS)
endif

CFG_PP := $(PRE)g++ -c $(CFG_SYOPTS)
CFG_CC := $(PRE)gcc -c $(CFG_SYOPTS)
CFG_LD := $(PRE)g++
CFG_AR := $(PRE)ar
CFG_DT := $(PRE)dlltool
CFG_DP := $(PRE)makedepend
CFG_AS := $(PRE)as
CFG_RC := $(PRE)windres

#CFG_LD_FLAGS := $(CFG_LD_FLAGS) -export-all-symbols
CFG_PP_FLAGS := $(CFG_PP_FLAGS) -MMD -Wall -fno-strict-aliasing -D__int64="long long"

ifeq ($(TGT_PLATFORM),windows)
	ifeq ($(TGT_PROC),x64)
		CFG_PP_FLAGS := $(CFG_PP_FLAGS) -fno-leading-underscore
	endif
endif

# static build
ifeq ($(TGT_LINK),static)
	CFG_PP_FLAGS := $(CFG_PP_FLAGS)  -static
	CFG_LD_FLAGS := $(CFG_LD_FLAGS)  -static -static-libgcc -static-libstdc++
	
# shared build
else
	ifneq ($(TGT_PLATFORM),windows)
		CFG_PP_FLAGS := $(CFG_PP_FLAGS)  -shared -fPIC -DPIC
	else
		CFG_LD_FLAGS := $(CFG_LD_FLAGS)  -shared -shared-libgcc
	endif
endif

# Shared library
ifeq ($(PRJ_TYPE),dll)
	ifeq ($(TGT_PLATFORM),windows)
		CFG_TGT_EXT := .dll
		CFG_LD_FLAGS := $(CFG_LD_FLAGS) -Wl,-enable-auto-import
	else
		CFG_TGT_EXT := .so
		CFG_LD_FLAGS := $(CFG_LD_FLAGS) -rdynamic -Wl,-E -Wl,--export-dynamic
	endif
else
	ifeq ($(TGT_PLATFORM),windows)
		CFG_TGT_EXT := .exe
		CFG_LD_FLAGS := $(CFG_LD_FLAGS) -Wl,-enable-auto-import
	else
		CFG_TGT_EXT :=
		CFG_LD_FLAGS := $(CFG_LD_FLAGS) -rdynamic -Wl,-E -Wl,--export-dynamic
	endif
endif



