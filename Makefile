OUTPUTDIR := bin/
#module load openmpi/4.0.2-gcc8.3.1
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

all: final-$(CONFIGURATION)-v1 final-$(CONFIGURATION)-v2 final-$(CONFIGURATION)-v3  \
	
# final-$(CONFIGURATION)-v1: $(HEADERS) ./network_simulator_seq.cpp
# 	$(CXX) -o $@ $(CFLAGS) ./network_simulator_seq.cpp

final-$(CONFIGURATION)-v1: $(HEADERS) ./network_simulator_seq2.cpp
	$(CXX) -o $@ $(CFLAGS) ./network_simulator_seq2.cpp

final-$(CONFIGURATION)-v2: $(HEADERS) ./network_simulator_para.cpp
	$(CXX) -o $@ $(CFLAGS) ./network_simulator_para.cpp

final-$(CONFIGURATION)-v3: $(HEADERS) ./network_simulator_para2.cpp
	$(CXX) -o $@ $(CFLAGS) ./network_simulator_para2.cpp

# final-$(CONFIGURATION)-v5: $(HEADERS) ./network_simulator_para5.cpp
# 	$(CXX) -o $@ $(CFLAGS) ./network_simulator_para5.cpp -fcilkplus
clean:
	rm -rf ./final-$(CONFIGURATION)*

FILES = ./*.cpp \
		./*.h
