CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

BUILD_DIR = build
APP = $(BUILD_DIR)/lab22
TEST_APP = $(BUILD_DIR)/lab22_tests

all: $(APP)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(APP): main.cpp tests.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) main.cpp tests.cpp -o $(APP)

run: $(APP)
	./$(APP)

test: $(TEST_APP)
	./$(TEST_APP)

$(TEST_APP): tests.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -DTESTS_MAIN tests.cpp -o $(TEST_APP)

clean:
	rm -f $(APP) $(TEST_APP) lab22 lab22_tests
