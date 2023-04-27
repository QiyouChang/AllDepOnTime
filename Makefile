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

all: final-$(CONFIGURATION)-v1 final-$(CONFIGURATION)-v2 final-$(CONFIGURATION)-v3 final-$(CONFIGURATION)-v4 \
    final-$(CONFIGURATION)-v5
	
final-$(CONFIGURATION)-v1: $(HEADERS) ./network_simulator_seq.cpp
	$(CXX) -o $@ $(CFLAGS) ./network_simulator_seq.cpp

final-$(CONFIGURATION)-v2: $(HEADERS) ./network_simulator_seq2.cpp
	$(CXX) -o $@ $(CFLAGS) ./network_simulator_seq2.cpp

final-$(CONFIGURATION)-v3: $(HEADERS) ./network_simulator_para.cpp
	$(CXX) -o $@ $(CFLAGS) ./network_simulator_para.cpp

final-$(CONFIGURATION)-v4: $(HEADERS) ./network_simulator_para2.cpp
	$(CXX) -o $@ $(CFLAGS) ./network_simulator_para2.cpp

final-$(CONFIGURATION)-v5: $(HEADERS) ./network_simulator_para3.cpp
	$(CXX) -o $@ $(CFLAGS) ./network_simulator_para3.cpp
clean:
	rm -rf ./final-$(CONFIGURATION)*

FILES = ./*.cpp \
		./*.h