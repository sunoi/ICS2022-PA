#include <proc.h>
#include <elf.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

size_t ramdisk_read(void* buf, size_t offset, size_t len);
size_t ramdisk_write(void* buf, size_t offset, size_t len);

static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr ehdr;
	ramdisk_read(&ehdr, 0, sizeof(Elf_Ehdr));
	size_t phnum = ehdr.e_phnum;
	size_t ehsize = ehdr.e_ehsize;
	//size_t entry = ehdr.e_entry;

	Elf_Phdr phdr[phnum];
	printf("%d\n", sizeof(Elf_Phdr));
	ramdisk_read(&phdr, ehsize, sizeof(Elf_Phdr)*phnum);
	for (size_t i = 0; i < phnum; i++) {
		if (phdr[i].p_type == PT_LOAD) {
			ramdisk_read((void*)phdr[i].p_vaddr, phdr[i].p_offset, phdr[i].p_memsz);
			memset((void*)(phdr[i].p_vaddr+phdr[i].p_filesz), 0, phdr[i].p_memsz-phdr[i].p_filesz);
		}
	}
	return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %d", entry);
  ((void(*)())entry) ();
}

