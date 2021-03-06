#Compiler and Linker
CC          := g++
AR          := ar

TARGET_EXE  ?= MongooseServer
TARGET_DIR 	?= ./bin
BUILD_DIR 	?= ./obj
SRC_DIR		 	?= ./src
MONGOOSE_DIR ?= ./mongoose
MONGOOSE_OBJ ?= $(MONGOOSE_DIR)/mongoose.o
MONGOOSE_INC ?= $(MONGOOSE_DIR)

JSONCPP_DIR ?= ./jsoncpp
JSONCPP_LIB ?= $(JSONCPP_DIR)/src/lib_json/libjsoncpp.a
JSONCPP_INC ?= $(JSONCPP_DIR)/include/json

CLIENT_DIR ?= ./client
CLIENT_PUBLIC ?= $(CLIENT_DIR)/PUBLIC
CLIENT_BUILD ?= $(CLIENT_PUBLIC)/manifest.json

SRCS  			:= $(shell find $(SRC_DIR) -name *.cpp -or -name *.c )
OBJS 				:= $(foreach src, $(notdir $(SRCS)),  $(addprefix obj/, $(addsuffix .o, $(basename $(src) ) )))
DEPS 				:= $(OBJS:.o=.d)
INC_DIRS 		:= include/ src/ $(MONGOOSE_INC) $(JSONCPP_INC)
INC_FLAGS 	:= $(addprefix -I,$(INC_DIRS))

MONGOOSE_FLAGS := -DMG_ENABLE_HTTP -DMG_ENABLE_HTTP_WEBSOCKET -DMG_ENABLE_GETADDRINFO -DMG_ENABLE_HTTP_SSI
MONGOOSE_FLAGS += -DMG_ENABLE_BROADCAST -DENABLE_GETADDRINFO -DMG_ENABLE_THREADS -DMG_ENABLE_FS
#MONGOOSE_FLAGS += -DMG_ENABLE_SSL -lssl -lcrypto
CFLAGS    	:= -Wall -O3 -g -MMD -MP   $(INC_FLAGS) $(MONGOOSE_FLAGS)

CPPFLAGS 		:=  $(CFLAGS) -std=c++11 
LDFLAGS   	:= -lm -lpthread 


$(info SRCS $(SRCS))
$(info OBJS $(OBJS))
$(info DEPS $(DEPS))
$(info BUILD_DIR $(BUILD_DIR))

$(TARGET_DIR)/$(TARGET_EXE): $(MONGOOSE_OBJ) $(OBJS) $(JSONCPP_LIB) $(CLIENT_BUILD)
	@mkdir -p $(TARGET_DIR)
	$(CC) -o $(TARGET_DIR)/$(TARGET_EXE) $(OBJS) $(MONGOOSE_OBJ) $(LDFLAGS) $(JSONCPP_LIB)

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

.ONESHELL:
$(JSONCPP_LIB): 
	cd $(JSONCPP_DIR)
	cmake CMakeLists.txt
	make 
	@echo "JSONCPP Build Complete"

.ONESHELL:
$(CLIENT_BUILD):
	cd $(CLIENT_DIR)
	npm run build
	@echo "CLIENT PRODUCTION BUILD COMPLETE"

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) -r $(TARGET_DIR)
	$(RM) -r $(MONGOOSE_OBJ)

#Full Clean, Objects and Binaries
cleaner: clean
	@$(RM) -rf $(TARGETDIR)

.PHONY: clean cleaner


-include $(DEPS)

MKDIR_P ?= mkdir -p

