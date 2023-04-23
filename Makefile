OUTPUTDIR := bin/

CFLAGS := -std=c++14 -fvisibility=hidden -lpthread -Wall -Wextra

ifeq (,$(CONFIGURATION))
	CONFIGURATION := release
endif

ifeq (debug,$(CONFIGURATION))
CFLAGS += -g
else
CFLAGS += -O2
endif

HEADERS := ./*.h

CXX = mpic++

.SUFFIXES:
.PHONY: all clean

all: final-$(CONFIGURATION)-v1 final-$(CONFIGURATION)-v2

final-$(CONFIGURATION)-v1: $(HEADERS) ./network_simulator_para.cpp
	$(CXX) -o $@ $(CFLAGS) ./network_simulator_para.cpp

final-$(CONFIGURATION)-v2: $(HEADERS) ./network_simulator_seq2.cpp
	$(CXX) -o $@ $(CFLAGS) ./network_simulator_seq2.cpp

# final-$(CONFIGURATION)-v2: $(HEADERS) ./network_simulator_para.cpp
# 	$(CXX) -o $@ $(CFLAGS) ./network_simulator_para.cpp

clean:
	rm -rf ./final-$(CONFIGURATION)*

FILES = ./*.cpp \
		./*.h