NAME		= webserv

# Directories
SRC_DIR		= src
INC_DIR		= includes
OBJ_DIR		= obj

# Compiler and flags
CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98
INCLUDES	= -I$(INC_DIR)

# Source files
SRCS		= $(SRC_DIR)/main.cpp \
			  $(SRC_DIR)/config/ConfigParser.cpp \
			  $(SRC_DIR)/server/Server.cpp \
			  $(SRC_DIR)/server/Socket.cpp \
			  $(SRC_DIR)/http/HttpRequest.cpp \
			  $(SRC_DIR)/http/HttpResponse.cpp \
			  $(SRC_DIR)/cgi/Cgi.cpp \
			  $(SRC_DIR)/fileupload/FileUploadHandler.cpp \
			  $(SRC_DIR)/response/ResponseHandler.cpp \
			  $(SRC_DIR)/utils/Utils.cpp

# Object files
OBJS		= $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Colors
GREEN		= \033[0;32m
RED			= \033[0;31m
BLUE		= \033[0;34m
YELLOW		= \033[0;33m
NC			= \033[0m # No Color

# Rules
all: $(NAME)

$(NAME): $(OBJS)
	@echo -e "$(BLUE)Linking $(NAME)...$(NC)"
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo -e "$(GREEN)✅ $(NAME) compiled successfully!$(NC)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo -e "$(YELLOW)Compiling $<...$(NC)"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo -e "$(RED)Cleaning object files...$(NC)"
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo -e "$(RED)Cleaning $(NAME)...$(NC)"
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re debug test
