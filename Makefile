# Define the compiler.
CXX = g++

# Define the compiler flags (ChatGPT).
CXXFLAGS = -Wall -std=c++17 -fPIC -I/home/turauth/repos/HW03/vcpkg/installed/x64-linux/include \
	$(shell pkg-config --cflags Qt5Widgets Qt5Gui Qt5Core)

# Define the linker flags (ChatGPT).
LDFLAGS = -L/home/turauth/repos/HW03/vcpkg/installed/x64-linux/lib -lcurl -ljsoncpp -lz -lssl -lcrypto \
	$(shell pkg-config --libs Qt5Widgets Qt5Gui Qt5Core)

# Define the target executable.
TARGET = hw3

# Define the source files.
SRCS = hw3.cpp

# Define the object files.
OBJS = $(SRCS:.cpp=.o)

# Rule to build the target executable (ChatGPT).
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Rule to build object files.
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files.
clean:
	rm -f $(OBJS) $(TARGET)