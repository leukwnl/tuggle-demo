# Top level NDK Makefile for Android application
# Module settings for CUGL
SUPPORT_AUDIO := true
SUPPORT_SCENE2 := true
SUPPORT_SCENE3 := true
SUPPORT_PHYSICS2 := true
SUPPORT_NETCODE  := true
SUPPORT_DISTRIB_PHYSICS2 := true

# We just invoke the subdirectories
include $(call all-subdir-makefiles)
