CFLAGS	=	-Wall -Wextra
CFLAGS	+=	-nostdinc -nostdlib -fno-builtin

.s.o:
	as -o $@ $<

all: fd.img

fd.img: boot.bin sys.bin
	cat boot.bin sys.bin > $@

boot.bin: boot.o
	ld -o $@ $< -T boot.ld

sys.bin: sys.o main.o
	ld -o $@ sys.o main.o -T sys.ld

boot.o: boot.s

sys.o: sys.s

main.o: main.c
	gcc $(CFLAGS) -o $@ $<

clean:
	rm -f *~ *.o *.bin *.dat *.img

run: fd.img
	qemu -fda fd.img
