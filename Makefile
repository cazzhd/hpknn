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

# ************ Vars ************
CXX 		= g++
MPICXX		?= mpic++
MPIEXEC		?= mpiexec --bind-to none --map-by node
COMPILER	:= $(if $(filter true,${mpi}),$(MPICXX),$(CXX))
# EXECUTOR	:= $(if $(filter true,${mpi}),$(MPIEXEC),)
NP			:= $(if ${np},-np $(np),)
NT			:= $(if ${nt},-x OMP_NUM_THREADS=$(nt),)
MODE		:= $(if ${mode},-mode $(mode),)

CXXFLAGS	:= -std=c++17 -Wall -g
CPR         := -lcpr
OPT 		:= -O2 -funroll-loops
OMP 		= -fopenmp
GPROF 		= -pg

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
	@if [ $< = "src/main.cpp" ]; then\
		$(MPICXX) $(GPROF) $(OMP) $(CXXFLAGS) $(INCLUDES) -c $< -o $@;\
	else\
		$(MPICXX) $(OMP) $(CXXFLAGS) $(INCLUDES) -c $< -o $@;\
	fi


# ************ Linking and creating executable ************

$(OUTPUTMAIN): $(OBJECTS)
	@echo "\n\e[33mLinking and creating executable $@ \e[0m"
	$(MPICXX) $(GPROF) $(OMP) $(CXXFLAGS) $(INCLUDES) -o $(OUTPUTMAIN) $(OBJECTS) $(CPR)

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
	-@if [ -d $(BIN) ]; then $(RMDIR) $(BIN); fi
	@echo "\e[31mBianarians removed!"
	-@$(RM) $(OBJ)/*
	-@if [ -d $(OBJ) ]; then $(RMDIR) $(OBJ); fi
	@echo "\e[31mObjects removed!"
	-@$(RM) $(DOC)/html/*
	-@if [ -d $(DOC)/html ]; then $(RMDIR) $(DOC)/html; fi
	-@$(RM) $(DOC)/index.html
	@echo "\e[31mDocs removed!"
	@echo "\e[31mCleanup complete!"

run: all
	@echo "\e[0mExecuting $(OUTPUTMAIN)...\n $(MODE) $(NP) $(NT) -conf config.json\nOUTPUT:\n"
	@$(MPIEXEC) $(NP) $(NT) ./$(OUTPUTMAIN) $(MODE) -conf config.json

info:
	@echo "\e[36mMade by Francisco Rodríguez Jiménez (cazz@correo.ugr.es)\nHpknn (c) 2015 EFFICOMP"

# Show version omp when execute program
# export  OMP_DISPLAY_ENV="TRUE"
openmp_version:
	@echo "\e[36mOpenMP version: $(shell echo | cpp -fopenmp -dM | grep -i open | cut -d '_' -f 2)"

openmpi_version:
	@echo "\e[36mOpenMPI version: $(shell mpirun --version | head -1 | cut -d " " -f 4)"