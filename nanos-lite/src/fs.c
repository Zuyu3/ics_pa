#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
  [FD_FB] = {"/dev/fb", 0, 0, invalid_read, fb_write},
  {"/dev/events", 0, 0, events_read, invalid_write},
  {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},
#include "files.h"
};


int fs_open(const char *pathname, int flags, int mode) {
  //printf("%d\n", sizeof(file_table) / sizeof(Finfo));
  for(int i = 0; i < sizeof(file_table) / sizeof(Finfo); i++) {
    if(strcmp(pathname, file_table[i].name) == 0){
      file_table[i].open_offset = 0;
      return i;
    }
  }
  panic("Error when opening a file. File \"%s\" does not exist.\n", pathname);
}

size_t fs_read(int fd, void *buf, size_t len) {
  assert(fd >=0 && fd < sizeof(file_table) / sizeof(Finfo));

  if(file_table[fd].read != NULL) {
    len = file_table[fd].read(buf, 0, len);
  }
  else {
    len = (len <= file_table[fd].size - file_table[fd].open_offset) ? len: file_table[fd].size - file_table[fd].open_offset;
    ramdisk_read(buf, file_table[fd].open_offset + file_table[fd].disk_offset, len);
    file_table[fd].open_offset += len;
  }
  return len;
}

size_t fs_write(int fd, const void *buf, size_t len) {
  assert(fd >=0 && fd < sizeof(file_table) / sizeof(Finfo));

  if(file_table[fd].write != NULL) {
    len = file_table[fd].write(buf, file_table[fd].open_offset, len);
  }
  else {
    len = (len <= file_table[fd].size - file_table[fd].open_offset) ? len: file_table[fd].size - file_table[fd].open_offset;
    ramdisk_write(buf, file_table[fd].open_offset + file_table[fd].disk_offset, len);
    file_table[fd].open_offset += len;
  }
  return len;
}

size_t fs_lseek(int fd, size_t offset, int whence) {
  switch (whence)
  {
  case SEEK_SET:
    file_table[fd].open_offset = offset;
    break;
  case SEEK_CUR:
    file_table[fd].open_offset += offset;
    break;
  case SEEK_END:
    file_table[fd].open_offset = file_table[fd].size + offset;
    break;
  default:
    panic("Undefined whence = %d when doing fs_lseek\n", whence);
    break;
  }
  return file_table[fd].open_offset;
}

int fs_close(int fd) {
  return 0;
}

void init_fs() {
  AM_GPU_CONFIG_T gpu_info = io_read(AM_GPU_CONFIG);
  file_table[FD_FB].size = gpu_info.width * gpu_info.height * 4;
}

char *get_file_name(int file_id) {
  if(file_id >=0 && file_id < sizeof(file_table) / sizeof(Finfo))
    return file_table[file_id].name;
  else
    return NULL;
}