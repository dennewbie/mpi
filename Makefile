MPICC=mpicc
OPTS=-g -O0
TARGETS=latency pi saxpy ring hello
all: $(TARGETS)

.PREFIXES:

%: %.c
	$(MPICC) $(OPTS) $< -o $@

clean:
	rm -rf $(TARGETS) *.o
