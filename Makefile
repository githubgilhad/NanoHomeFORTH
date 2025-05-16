ifeq ($(MAKEFILE_LIST), Makefile)
  $(info run make in src directory)
endif
include $(dir $(realpath $(lastword $(MAKEFILE_LIST))))../Makefile
