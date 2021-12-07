SER = server.cpp
CLI = client.cpp

CXX = g++
CFLAGS = -std=c++11 -O2 -Wall -g 

SERBIN = server
CLIBIN = client

all: $(SERBIN) $(CLIBIN)

$(SERBIN):$(SER)
	$(CXX) $(SER) -o $@ $(CFLAGS)

$(CLIBIN):$(CLI)
	$(CXX) $(CLI) -o $@ $(CFLAGS)

clean:
	rm -rf $(SERBIN) $(CLIBIN)