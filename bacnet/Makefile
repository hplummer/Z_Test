# path to the top of the vigs directory
FC_DIR = ../..

# the base name for the library
LIB_NAME = libbacnet

#
# To make a shared library, uncomment the following:
#
#SHARED = 1
#SHARED_VER_MAJOR = 1
#SHARED_VER_MINOR = 0
#

# list of sources
SRCS =	BacnetUtils.cpp BacnetValue.cpp BacnetAppTypes.cpp BacnetProperties.cpp BacnetObject.cpp \
		BacnetDevice.cpp BacnetServer.cpp BacnetVsbConverter.cpp BacnetValueGetterSetter.cpp

# extra preprocessor defines
LOCAL_DEFINES = -std=gnu++0x
ifdef DEBUG
LOCAL_DEFINES += -DDEBUGVSBHP
endif


LOCAL_INCLUDES = .. $(FC_DIR)/facs/fc $(FC_DIR)/facs/vsb 

# extra package directories
LOCAL_PACKAGES = $(FC_DIR)/facs/fc $(FC_DIR)/facs/vsb

# extra libraries
LOCAL_LIBS = fc vsb

# include the default library makefile
include $(FC_DIR)/mk/lib.mk