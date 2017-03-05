# Compiler flags
CFLAGS := -w -m32 -D'main(a,b)=main(a,char**argv)'

# Compile selfie.c into selfie executable
selfie: selfie.c
	$(CC) $(CFLAGS) $< -o $@

# Consider these targets as targets, not files
.PHONY : test clean

# Test self-compilation, self-execution, and self-hosting
test: selfie
	./selfie -c selfie.c -o selfie1.m #compile selfie using selfie
	./selfie -c kernel.c -o kernel.m #compile kernel using selfie
	./selfie -c userSource.c -o userBinary.m #compile userSource using selfie
	./selfie -l kernel.m -m userBinary.m 8 3 #execute up to 3 instances of given userBinary
	./selfie -l kernel.m -m selfie1.m 8 3 #selfie self execution

# Clean up
clean:
	rm -rf *.m
	rm -rf *.s
	rm -rf selfie
