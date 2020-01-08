#Compiler and Linker
CC          := g++
AR          := ar

TARGET_EXE  ?= MongooseServer
TARGET_DIR 	?= ./bin
BUILD_DIR 	?= ./obj
SRC_DIR		 	?= ./src
MONGOOSE_DIR ?= ./mongoose
MONGOOSE_OBJ ?= $(MONGOOSE_DIR)/mongoose.o

SRCS  			:= $(shell find $(SRC_DIR) -name *.cpp -or -name *.c )
OBJS 				:= $(foreach src, $(notdir $(SRCS)),  $(addprefix obj/, $(addsuffix .o, $(basename $(src) ) )))
DEPS 				:= $(OBJS:.o=.d)
INC_DIRS 		:= include/ src/ $(MONGOOSE_DIR)
INC_FLAGS 	:= $(addprefix -I,$(INC_DIRS))

CFLAGS    	:= -Wall -O3 -g -MMD -MP  -DMG_ENABLE_THREADS $(INC_FLAGS)
CPPFLAGS 		:=  $(CFLAGS) -std=c++11 
LDFLAGS   	:= -lm -lpthread 


$(info SRCS $(SRCS))
$(info OBJS $(OBJS))
$(info DEPS $(DEPS))
$(info BUILD_DIR $(BUILD_DIR))

$(TARGET_DIR)/$(TARGET_EXE): $(MONGOOSE_OBJ) $(OBJS) 
	@mkdir -p $(TARGET_DIR)
	$(CC) -o $(TARGET_DIR)/$(TARGET_EXE) $(OBJS) $(MONGOOSE_OBJ) $(LDFLAGS) 

$(MONGOOSE_OBJ):
	$(CC) $(CFLAGS) -c $(MONGOOSE_DIR)/mongoose.c -o $(MONGOOSE_DIR)/mongoose.o


# c source
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# c++ source
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) -r $(TARGET_DIR)

#Full Clean, Objects and Binaries
cleaner: clean
	@$(RM) -rf $(TARGETDIR)

.PHONY: clean cleaner


-include $(DEPS)

MKDIR_P ?= mkdir -p

