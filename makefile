CXX = g++
CFLAGS = -std=c++11 -O2 -Wall -lpthread -g
BUILDIR = build

TARGET = webserver

SRCS = $(wildcard ./*.cpp \
				  ./buffer/*.cpp \
				  ./http/*.cpp \
				  ./pool/*.cpp \
				  ./server/*.cpp)

# FILES = $(notdir $(SRCS))
OBJS = $(SRCS:%.cpp=%.o)
# BUILDOBJS = $(OBJS:%.o=$(BUILDIR)/%.o)

INCLUDES = -I. \
		   -Iserver/ \
		   -Ipool/ \
		   -Ihttp/ \
		   -Ibuffer/

LIBS = -lpthread

$(TARGET):$(OBJS)
	$(CXX) -o $@ $^ $(LIBS)

%.o:%.cpp
	$(CXX) -o $@ -c $< $(CFLAGS) $(INCLUDES)

clean:
	rm -rf $(OBJS)
	rm $(TARGET)

.PHONY: all clean