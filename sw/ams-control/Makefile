all : main.c irq.c bootstrap.S
	riscv32-unknown-elf-gcc main.c irq.c bootstrap.S -o main -march=rv32i -mabi=ilp32 -nostartfiles -Wl,--no-relax
	
sim: all
	../../vp/build/bin/ams-vp --intercept-syscalls main
	
sim-opt: all
	ams-vp --intercept-syscalls --use-dmi --tlm 1000 main
	
dump-elf: all
	riscv32-unknown-elf-readelf -a main
	
dump-code: all
	riscv32-unknown-elf-objdump -D main
	
dump-comment: all
	objdump -s --section .comment main
	
clean:
	rm -f main
	rm -f dump.txt
	rm -f testbench_trace.vcd
	rm -f testbench.dat
