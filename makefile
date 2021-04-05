TARGETS = child gates_ver1

# Targets to build when running `make`
all: $(TARGETS)

# Phony targets. Executed always.
.PHONY: all clean

# Targets to build `child` binary
child: child.o
	gcc child.o -o child
child.o: child.c
	gcc child.c -c -o child.o

# Targets to build `gates_ver1` binary
gates_ver1: gates_ver1.o
	gcc gates_ver1.o -o gates_ver1
gates_ver1.o: gates_ver1.c
	gcc gates_ver1.c -c -o gates_ver1.o

# Run `make clean` to delete binaries and object files.
clean:
	rm -rf $(TARGETS) *.o
