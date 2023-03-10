CC      ?= clang
CFLAGS  = -Wall -Wextra -Wpedantic -pedantic -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -std=c11 \
          -Werror=incompatible-pointer-types -Werror=return-type

CMD_SRC = assemble.c emulate.c disassemble.c debug.c
LIB_SRC = asm.c      asm_sdt.c  dis_br.c   emu.c       emu_mul.c    linenoise.c \
		  asm_br.c   bit_asm.c  dis_dp.c   emu_br.c    emu_sdt.c    mask.c \
		  asm_dp.c   dbg.c      dis_mul.c  emu_cond.c  emu_shift.c  str.c \
		  asm_mul.c  dis.c      dis_sdt.c  emu_dp.c    lexer.c      symtab.c

TEST_SRC = instr_common_parse.c symtab.c valid_imm_test.c

BUILD_DIR = build

# Internal, you shouldn't need to change anythong below this

MAKEFLAGS += --no-builtin-rules # No Implict rules
.SECONDARY: # Don't remove object files
V = @ # use make V= to see the commands

CFLAGS += -I include

ifdef ASAN
	CFLAGS += -fsanitize=address
	CC = clang
	BUILD_DIR = build-asan
else ifdef MSAN 
	CFLAGS += -fsanitize=memory
	CC = clang
	BUILD_DIR = build-msan
else ifdef UBSAN
	CFLAGS += -fsanitize=undefined
	CC = clang
	BUILD_DIR = build-ubsan
else ifdef TRACE
	CFLAGS += -DAEMU_TRACE
	BUILD_DIR = build-trace
else ifdef FUZZ
	CFLAGS += -DAEMU_FUZZ
	BUILD_DIR = build-fuzz
	CC = afl-clang-fast
else ifdef LEAK
	CC = clang
	BUILD_DIR = build-leak
	CFLAGS += -fsanitize=leak
endif

CMD_OBJ = $(CMD_SRC:%.c=$(BUILD_DIR)/cmd/%.o)
CMD_BIN = $(CMD_SRC:%.c=$(BUILD_DIR)/bin/%)
CMD_DEPS = $(CMD_OBJ:%.o=%.d)

LIB_OBJ = $(LIB_SRC:%.c=$(BUILD_DIR)/lib/%.o)
LIB_DEPS = $(LIB_OBJ:%.o=%.d)

TEST_OBJ = $(TEST_SRC:%.c=$(BUILD_DIR)/test/%.o)
TEST_DEPS = $(TEST_OBJ:%.o=%.d)
TEST_BIN = $(TEST_SRC:%.c=$(BUILD_DIR)/test/bin/%)

TIMESTAMP = $(BUILD_DIR)/.timestamp

DEPS = $(CMD_DEPS) $(LIB_DEPS) $(TEST_DEPS)

all: $(CMD_BIN)

$(BUILD_DIR)/%.o: %.c $(TIMESTAMP) include/mask.h
	@test -z "$(V)" || echo "CC   $@"
	$(V)$(CC) $(CFLAGS) -MMD -MP -c $< -o $@
$(BUILD_DIR)/bin/%: $(BUILD_DIR)/cmd/%.o $(LIB_OBJ)
	@test -z "$(V)" || echo "CCLD $@"
	$(V)$(CC) $(CFLAGS) -MMD -MP $^ -o $@
$(BUILD_DIR)/test/bin/%: $(BUILD_DIR)/test/%.o $(LIB_OBJ)
	@test -z "$(V)" || echo "CCLD $@"
	$(V)$(CC) $(CFLAGS) -MMD -MP $^ -o $@


$(TIMESTAMP): Makefile
	mkdir -p $(BUILD_DIR)/cmd
	mkdir -p $(BUILD_DIR)/bin
	mkdir -p $(BUILD_DIR)/lib
	mkdir -p $(BUILD_DIR)/test/bin
	touch $@

lib/mask.c: maskgen.py 
	python3 maskgen.py
	clang-format -i lib/mask.c include/mask.h
include/mask.h: lib/mask.c

debug:
	@echo "CMD_SRC = $(CMD_SRC)"
	@echo "CMD_OBJ = $(CMD_OBJ)"
	@echo "CMD_BIN = $(CMD_BIN)"

clean:
	rm -rf $(BUILD_DIR)
	rm -f lib/mask.c
	rm -f include/mask.h

fmt:
	find -name '*.c' -o -name '*.h' | xargs clang-format -i
	black .

test: $(TEST_BIN)
	@for test in $(TEST_BIN); do \
		echo $${test};           \
		$${test};                \
	done


.PHONY: clean debug fmt test

asan:
	$(MAKE) ASAN=1
msan:
	$(MAKE) MSAN=1
ubsan:
	$(MAKE) UBSAN=1
trace:
	$(MAKE) TRACE=1
fuzz:
	$(MAKE) FUZZ=1
leak:
	$(MAKE) LEAK=1
.PHONY: fuzz
assemble: $(BUILD_DIR)/bin/assemble
disassemble: $(BUILD_DIR)/bin/disassemble
emulate: $(BUILD_DIR)/bin/emulate

-include $(DEPS)
