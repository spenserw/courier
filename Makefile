MKDIR_P = mkdir -p

CLIENT_DIR=client
SERVER_DIR=server

# DEPS
SHARED_DEPS = $(filter %.h,$(wildcard include/*))
CLIENT_DEPS_PATH=include/$(CLIENT_DIR)
CLIENT_DEPS = $(filter %.h,$(wildcard $(CLIENT_DEPS_PATH)/*))
SERVER_DEPS_PATH=include/$(SERVER_DIR)
SERVER_DEPS = $(filter %.h,$(wildcard $(SERVER_DEPS_PATH)/*))

# SOURCES
CLIENT_SRC_PATH=src/$(CLIENT_DIR)
CLIENT_SRC=$(filter %.cpp,$(wildcard $(CLIENT_SRC_PATH)/*))

SERVER_SRC_PATH=src/$(SERVER_DIR)
SERVER_SRC=$(filter %.cpp,$(wildcard $(SERVER_SRC_PATH)/*))

SHARED_SRC_PATH=src
SHARED_SRC=$(filter %.cpp,$(wildcard $(SHARED_SRC_PATH)/*))

# OBJECTS
CLIENT_OBJ_DIR_PATH=$(CLIENT_SRC_PATH)/obj
CLIENT_SRC_OBJS=$(notdir $(subst .cpp,.o,$(CLIENT_SRC))) # sources => objects
CLIENT_OBJS_PATHS=$(patsubst %,$(CLIENT_OBJ_DIR_PATH)/%,$(CLIENT_SRC_OBJS)) # build obj path

SERVER_OBJ_DIR_PATH=$(SERVER_SRC_PATH)/obj
SERVER_SRC_OBJS=$(notdir $(subst .cpp,.o,$(SERVER_SRC))) # sources => objects
SERVER_OBJS_PATHS=$(patsubst %,$(SERVER_OBJ_DIR_PATH)/%,$(SERVER_SRC_OBJS)) # build obj path

SHARED_OBJ_DIR_PATH=$(SHARED_SRC_PATH)/obj
SHARED_SRC_OBJS=$(notdir $(subst .cpp,.o,$(SHARED_SRC))) # sources => objects
SHARED_OBJS_PATHS=$(patsubst %,$(SHARED_OBJ_DIR_PATH)/%,$(SHARED_SRC_OBJS)) # build obj path

# COMPILE
CC = g++
SHARED_CFLAGS := -std=c++17 -Wall -Iinclude/ -Ithirdparty/
CLIENT_CFLAGS := -I$(CLIENT_DEPS_PATH)
SERVER_CFLAGS := -I$(SERVER_DEPS_PATH)
LFLAGS = -Llib/ -lGameNetworkingSockets_s -lprotobuf -lcrypto -lpthread

# BIN
CLIENT_BIN=courier_test_client
SERVER_BIN=courier

# RULES
all: $(SERVER_BIN) $(CLIENT_BIN)

# CLIENT
client: client_obj_dir $(CLIENT_BIN)

$(CLIENT_BIN): $(CLIENT_OBJS_PATHS) $(SHARED_OBJS_PATHS)

$(CLIENT_OBJ_DIR_PATH)/%.o: $(CLIENT_SRC_PATH)/%.cpp $(CLIENT_DEPS) $(SHARED_DEPS)
	$(CC) -c -o $@ $< $(SHARED_CFLAGS) $(CLIENT_CFLAGS)

# SERVER
server: server_obj_dir $(SERVER_BIN)

$(SERVER_BIN): $(SERVER_OBJS_PATHS) $(SHARED_OBJS_PATHS)
	$(CC) -o $(SERVER_BIN) $(SERVER_OBJS_PATHS) $(SHARED_OBJS_PATHS) $(SHARED_CFLAGS) $(SERVER_CFLAGS) $(LFLAGS)

$(SERVER_OBJ_DIR_PATH)/%.o: $(SERVER_SRC_PATH)/%.cpp $(SERVER_DEPS) $(SHARED_DEPS)
	$(CC) -c -o $@ $< $(SHARED_CFLAGS) $(SERVER_CFLAGS)

# SHARED
# $(SHARED_OBJ_DIR_PATH)/%.o: $(SHARED_SRC_PATH)/%.cpp $(SHARED_DEPS)
# 	$(CC) -c -o $@ $< $(SHARED_CFLAGS)

.PHONY: client_obj_dir
client_obj_dir: $(CLIENT_OBJ_DIR)

.PHONY: server_obj_dir
server_obj_dir: $(SERVER_OBJ_DIR)

$(CLIENT_OBJ_DIR):
	${MKDIR_P} $(CLIENT_OBJ_DIR_PATH)

$(SERVER_OBJ_DIR):
	${MKDIR_P} $(SERVER_OBJ_DIR_PATH)

.PHONY: debug
debug : CFLAGS += -ggdb
debug : $(OBJS) 
	$(CC) -o $(BIN_NAME) $(OBJS) $(CFLAGS) $(LFLAGS)

.PHONY: clean
clean:
	rm -f $(SERVER_OBJ_DIR_PATH)/*.o
	rm -f $(CLIENT_OBJ_DIR_PATH)/*.o
