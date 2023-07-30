NAME_WF = WFSnapshot/snapshotBench
NAME_P = PSnapshot/snapshotBench

CC ?= gcc
RM ?= @rm
MKDIR ?= @mkdir

CFLAGS := -O3 -Wall -Wextra -fopenmp

SRC_WF = WFSnapshot/src
SRC_P = PSnapshot/src
BUILD_WF = WFSnapshot/build
BUILD_P = PSnapshot/build
DATA_WF = WFSnapshot/data
DATA_P = PSnapshot/data
TEST_WF = WFSnapshot/test
TEST_P = PSnapshot/test
INCLUDES = inc

SRCS_WF = $(wildcard $(SRC_WF)/*.c)
OBJECTS_WF = $(patsubst $(SRC_WF)/%.c,$(OBJ)/%.o,$(SRCS_WF))
SRCS_P = $(wildcard $(SRC_P)/*.c)
OBJECTS_P = $(patsubst $(SRC_P)/%.c,$(OBJ)/%.o,$(SRCS_P))

all: $(BUILD_WF) $(BUILD_P) $(NAME_WF) $(NAME_WF).so $(NAME_P) $(NAME_P).so
	@echo "Built $(NAME_WF)"
	@echo "Built $(NAME_P)"
	@echo "Built $(TEST_WF)"
	@echo "Built $(TEST_P)"

$(DATA_WF):
	@echo "Creating data directory: $(DATA_WF)"
	$(MKDIR) $(DATA_WF)

$(BUILD_WF):
	@echo "Creating build directory: $(BUILD_WF)"
	$(MKDIR) $(BUILD_WF)

$(BUILD_WF)/%.o: $(SRC_WF)/%.c
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -fPIC -I$(INCLUDES) -c -o $@ $<

$(NAME_WF): $(foreach object,$(OBJECTS_WF),$(BUILD_WF)/$(object))
	@echo "Linking $(NAME_WF)"
	$(CC) $(CFLAGS) -o $@ $^

$(NAME_WF).so: $(foreach object,$(OBJECTS_WF),$(BUILD_WF)/$(object))
	@echo "Linking $(NAME_WF)"
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $^ 

test_wf: $(SRCS_WF) $(TEST_WF)/TestWFSnapshot.c $(TEST_WF)/TestSRSWRegister.c
	@echo "Compiling tests"
	$(CC) $(CFLAGS) -o $(TEST_WF)/WFTest $(SRC_WF)/mrswRegisterOpenMP.c $(SRC_WF)/srswRegisterOpenMP.c $(SRC_WF)/stampedValue.c $(SRC_WF)/WFSnapshot.c $(TEST_WF)/TestWFSnapshot.c
	$(CC) $(CFLAGS) -o $(TEST_WF)/SRSWTest $(SRC_WF)/srswRegisterOpenMP.c $(SRC_WF)/stampedValue.c $(TEST_WF)/TestSRSWRegister.c


test_p: $(SRCS_P) $(TEST_P)/TestLinerizability.c $(TEST_P)/TestPSnapshot.c
	@echo "Compiling tests"
	$(CC) $(CFLAGS) -o $(TEST_P)/LinearizabilityTest $(SRC_P)/mrmwREG.c $(SRC_P)/PSnapshot.c $(TEST_P)/TestLinerizability.c
	$(CC) $(CFLAGS) -o $(TEST_P)/PSnapshotTest $(SRC_P)/mrmwREG.c $(SRC_P)/PSnapshot.c $(TEST_P)/TestPSnapshot.c

test: test_wf test_p
	@echo "Tests created"

$(DATA_P):
	@echo "Creating data directory: $(DATA_P)"
	$(MKDIR) $(DATA_P)

$(BUILD_P):
	@echo "Creating build directory: $(BUILD_P)"
	$(MKDIR) $(BUILD_P)

$(BUILD_P)/%.o: $(SRC_P)/%.c
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -fPIC -I$(INCLUDES) -c -o $@ $<

$(NAME_P): $(foreach object,$(OBJECTS_P),$(BUILD_P)/$(object))
	@echo "Linking $(NAME_P)"
	$(CC) $(CFLAGS) -o $@ $^

$(NAME_P).so: $(foreach object,$(OBJECTS_P),$(BUILD_P)/$(object))
	@echo "Linking $(NAME_P)"
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $^ 

small-bench: $(BUILD_WF) $(NAME_WF).so $(DATA_WF) $(BUILD_P) $(NAME_P).so $(BUILD_P)
	@echo "Running small-bench ..."
	@python benchmark.py

small-plot-PSnapshot: 
	@echo "Plotting small-bench results for PSnapshot ..."
	bash -c 'cd plots && pdflatex "\newcommand{\DATAPATH}{../PSnapshot/data/$$(ls ../PSnapshot/data/ | sort -r | head -n 1)}\input{avg_plotPsnap.tex}"'
	@echo "============================================"
	@echo "Created plots/avgplotWFsnap.pdf for PSnapshot"

small-plot-WFSnapshot: 
	@echo "Plotting small-bench results for WFSnapshot ..."
	bash -c 'cd plots && pdflatex "\newcommand{\DATAPATH}{../WFSnapshot/data/$$(ls ../WFSnapshot/data/ | sort -r | head -n 1)}\input{avg_plotWFsnap.tex}"'
	@echo "============================================"
	@echo "Created plots/avgplotWFsnap.pdf for WFSnapshot"

small-plot: small-plot-PSnapshot small-plot-WFSnapshot

report: small-plot
	@echo "Compiling report ..."
	bash -c 'cd report && pdflatex report.tex'
	@echo "============================================"
	@echo "Done"

zip:
	@zip framework.zip benchmark.py Makefile README src/* plots/avg_plot.tex report/report.tex

clean:
	@echo "Cleaning build directories: $(BUILD_WF), $(BUILD_P) and binaries: $(NAME_WF) $(NAME_WF).so $(NAME_P) $(NAME_P).so"
	$(RM) -Rf $(BUILD_WF)
	$(RM) -f $(NAME_WF) $(NAME_WF).so
	$(RM) -Rf $(BUILD_P)
	$(RM) -f $(NAME_P) $(NAME_P).so
	$(RM) $(TEST_WF)/WFTest
	$(RM) $(TEST_WF)/SRSWTest
	$(RM) $(TEST_P)/LinearizabilityTest
	$(RM) $(TEST_P)/PSnapshotTest

.PHONY: clean report
