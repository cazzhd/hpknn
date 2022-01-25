## This file is subject to the terms and conditions defined in
# file 'LICENSE', which is part of Hpmoon repository.
#
# This work has been funded by:
#
# Spanish 'Ministerio de Economı́a y Competitividad' under grants number
# TIN2012-32039 and TIN2015-67020-P.
# Spanish 'Ministerio de Ciencia, Innovación y Universidades' under grant number
# PGC2018-098813-B-C31.
# European Regional Development Fund (ERDF).
#
# @file Makefile
# @author Francisco Rodríguez Jiménez
# @date 26/06/2021
# @brief File with the necessary rules to compile the project. Also, documentation is generated
# @copyright Hpknn (c) 2015 EFFICOMP

# How to know the version openmp
# export  OMP_DISPLAY_ENV="TRUE"
# echo |cpp -fopenmp -dM |grep -i open

# ************ Vars ************
CXX 		= g++
MPICXX 		?= mpic++
# CXXFLAGS	:= -std=c++0x -Wall -Wextra -g
CXXFLAGS	:= -std=c++17 -Wall -Wextra -g
OPT 		:= -O2 -funroll-loops
OMP 		= -fopenmp

BIN		:= bin
SRC		:= src
OBJ		:= obj
INCLUDE	:= include
DOC		:= docs

# ************ Plataform ************
ifeq ($(OS),Windows_NT)
	MAIN	:= hpknn.exe
	SOURCEDIRS	:= $(SRC)
	INCLUDEDIRS	:= $(INCLUDE)
	FIXPATH = $(subst /,\,$1)
	RM			:= del /q /f
	RMDIR = rmdir /S /Q
	MD	:= mkdir
else
	MAIN	:= hpknn
	SOURCEDIRS	:= $(shell find $(SRC) -type d)
	INCLUDEDIRS	:= $(shell find $(INCLUDE) -type d)
	FIXPATH = $1
	RM = rm -f
	RMDIR = rmdir
	MD	:= mkdir -p
endif

INCLUDES	:= $(patsubst %,-I%, $(INCLUDEDIRS:%/=%))
SOURCES		:= $(wildcard $(patsubst %,%/*.cpp, $(SOURCEDIRS)))
OBJECTS		:= $(subst $(SRC), $(OBJ), $(SOURCES:.cpp=.o))
OUTPUTMAIN	:= $(call FIXPATH,$(BIN)/$(MAIN))
FOLDERS_CREATE = $(OBJ) $(BIN) $(DOC)
CONF_FILE = config.json


# ************ Targets ************

all: $(FOLDERS_CREATE) $(OUTPUTMAIN)
	@echo "\n\e[32mBinary $(OUTPUTMAIN) has been created!"

# ************ Folders to create ************

$(FOLDERS_CREATE):
	@$(MD) $@

# ************ Compilation of modules ************

$(OBJ)/%.o: $(SRC)/%.cpp
	@echo "\e[33mCompiling module $< \e[0m"
	$(CXX) $(OMP) $(CXXFLAGS) $(INCLUDES) -c $<  -o $@


# ************ Linking and creating executable ************

$(OUTPUTMAIN): $(OBJECTS)
	@echo "\n\e[33mLinking and creating executable $@ \e[0m"
	$(CXX) $(OMP) $(CXXFLAGS) $(INCLUDES) -o $(OUTPUTMAIN) $(OBJECTS) $(LFLAGS)

# ************ Documentation ************

# 	Update Doxyfile with command doxygen -u
documentation:
	@echo "\e[35mCreating documentation..."
	@mkdir -p $(DOC)/html
	@doxygen $(DOC)/Doxyfile
	@printf "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n" > $(DOC)/index.html
	@printf "<html>\n <head>\n  <meta http-equiv=Refresh content=\"0;url=html/index.html\">\n </head>\n</html>" >> $(DOC)/index.html
	@echo "\e[35mDocumentation created!"

# ************ Cleaning ***************

.PHONY: clean
clean:
	-@$(RM) $(OUTPUTMAIN)
	-@$(RMDIR) $(BIN)
	@echo "\e[31mBianarians removed!"
	-@$(RM) $(OBJ)/*
	-@$(RMDIR) $(OBJ)
	@echo "\e[31mObjects removed!"
	-@$(RM) $(DOC)/html/*
	-@$(RMDIR) $(DOC)/html
	-@$(RM) $(DOC)/index.html
	@echo "\e[31mDocs removed!"
	@echo "\e[31mCleanup complete!"

run: all
	@echo "\e[0mExecuting $(OUTPUTMAIN)...\nOUTPUT:\n"
	@./$(OUTPUTMAIN) -conf config.json

info:
	@echo "\e[36mMade by Francisco Rodríguez Jiménez (cazz@correo.ugr.es)\nHpknn (c) 2015 EFFICOMP"