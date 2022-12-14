#include <proc.h>
#include <elf.h>
#include <fs.h>
#include <am.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

size_t ramdisk_read(void* buf, size_t offset, size_t len);
size_t ramdisk_write(void* buf, size_t offset, size_t len);
int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);
size_t fs_disk_offset(int fd);

static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr ehdr;
	//ramdisk_read(&ehdr, 0, sizeof(Elf_Ehdr));
	//printf("%s\n", filename);
	int fd = fs_open(filename, 0, 0);
	if (fd < 0) assert(0);
	fs_lseek(fd, 0, SEEK_SET);
	fs_read(fd, &ehdr, sizeof(Elf_Ehdr));
	char magic[] = {0x7f, 0x45, 0x4c, 0x46, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	assert(strcmp((char*)ehdr.e_ident, magic) == 0);
	size_t phnum = ehdr.e_phnum;
	size_t phoff = ehdr.e_phoff;
	//size_t entry = ehdr.e_entry;
	//printf("%d\n", entry);

	Elf_Phdr phdr[phnum];
	size_t disk_offset = fs_disk_offset(fd);
	ramdisk_read(&phdr, disk_offset+phoff, sizeof(Elf_Phdr)*phnum);

	for (size_t i = 0; i < phnum; i++) {
		if (phdr[i].p_type == PT_LOAD) {
			fs_lseek(fd, phdr[i].p_offset, SEEK_SET);
			fs_read(fd, (void*)phdr[i].p_vaddr, phdr[i].p_filesz);
			//ramdisk_read((void*)phdr[i].p_vaddr, phdr[i].p_offset, phdr[i].p_memsz);
			memset((void*)(phdr[i].p_vaddr+phdr[i].p_filesz), 0, phdr[i].p_memsz-phdr[i].p_filesz);
		}
	}
	fs_close(fd);
	return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}
