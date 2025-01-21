.PHONY: all clean

CXX = clang++
CXXFLAGS = -Wall -Wextra -std=c++23 -O2
TARGET: funclist_test_1

all: $(TARGET)

$(TARGET:=:.o): $(TARGET:=.cpp)
	$(CXX) $(CXXFLAGS) -c -o $@ $^

$(TARGET): $(TARGET:=:.o)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(TARGET) *.o