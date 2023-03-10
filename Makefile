
all:
	$(MAKE) -C src
asan:
	$(MAKE) -C src asan
msan:
	$(MAKE) -C src msan
ubsan:
	$(MAKE) -C src ubsan
leak:
	$(MAKE) -C src leak

world: all asan msan ubsan leak

.PHONY: clean-world
clean-world:
	rm -rf src/build*

server: all
	ruby arm11_testsuite/testserver.rb src/build/bin/	 
server-asan: asan
	ruby arm11_testsuite/testserver.rb src/build-asan/bin/
server-msan: msan
	ruby arm11_testsuite/testserver.rb src/build-msan/bin/
server-ubsan: ubsan
	ruby arm11_testsuite/testserver.rb src/build-ubsan/bin/
server-leak: leak
	ruby arm11_testsuite/testserver.rb src/build-leak/bin/

test: world
	make -C src test
	make -C tests test