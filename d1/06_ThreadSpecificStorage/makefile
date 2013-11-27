LD_FLAGS := -lboost_thread

CPP_FILES := $(wildcard *.cpp)
OBJ_FILES := $($(notdir $(CPP_FILES:.cpp=.o)))

all: $(CPP_FILES)
	g++ $(LD_FLAGS) -o $(basename $?) $?

