.PHONY: all clean

CXX = clang++
CXXFLAGS = -Wall -Wextra -std=c++23 -O2
TARGET = funclist_test_1

all: $(TARGET)

# Kompilacja pliku obiektowego
$(TARGET:.cpp=.o): $(TARGET).cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $^

# Łączenie pliku wykonywalnego
$(TARGET): $(TARGET:.cpp=.o)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Czyszczenie plików
clean:
	rm -f $(TARGET) *.o
