#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
static int canvas_x = 0, canvas_y = 0;
int open(const char *path, int flags, mode_t mode);

uint32_t NDL_GetTicks() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int NDL_PollEvent(char *buf, int len) {
  int fd = open("/dev/events", 0, 0);
  int res = read(fd, buf, len);
  printf("buf: %s, len: %d", buf, strlen(buf));
  close(fd);
  return res? 1 : 0;
}

void NDL_OpenCanvas(int *w, int *h) {
  if(*w == 0 && *h == 0) {
    *w = screen_w;
    *h = screen_h;
    return;
  }
  else if(*w > screen_w || *h > screen_h) {
    printf("Error. Canvas larger than screen.\n");
  }

  canvas_x = (screen_w - *w) / 2;
  canvas_y = (screen_h - *h) / 2;
  

  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
  
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  //printf("(x, y, w, h): (%d, %d, %d, %d)\n", x, y, w, h);

  int fd = open("/dev/fb", 0, 0);

  for(int i = 0; i < h; i++) {
    lseek(fd, ((canvas_y + y + i) * screen_w + x + canvas_x) * 4, SEEK_SET);
    write(fd, pixels + w * i, w * 4);
  }
  close(fd);

}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {

  if (getenv("NWM_APP")) {
    evtdev = 3;
  }

  char buf[64];
  int fd = open("/proc/dispinfo", 0, 0);

  int len = read(fd, buf, 64);
  int id = 0, temp, temp_w = -1, temp_h = -1;

  while(id < len) {
    //printf("id:%d \n", id );
      if(strncmp(buf + id, "WIDTH", 5) == 0) {
        //printf("hit width at %d\n", id);
        temp_w = 0;
        for(temp = id + 5; temp < len; temp++) {
          if(buf[temp] >= '0' && buf[temp] <= '9')
            temp_w = temp_w * 10 + buf[temp] - '0';
          if(buf[temp] == '\n'){
            id = temp;
            break;
          }
        }
      }
      else if(strncmp(buf + id, "HEIGHT", 6) == 0) {
        //printf("hit height at %d\n", id);
        temp_h = 0;
        for(temp = id + 6; temp < len; temp++) {
          if(buf[temp] >= '0' && buf[temp] <= '9')
            temp_h = temp_h * 10 + buf[temp] - '0';
          if(buf[temp] == '\n'){
            id = temp;
            break;
          }
        }
      }
      if(temp_w != -1 && temp_h != -1)
        break;
      id++;
  }

  if(temp_w == -1 || temp_h == -1)
    assert(0);
  else {
    screen_h = temp_h;
    screen_w = temp_w;
  }
  close(fd);
  //printf("width is %d, height is %d\n", screen_w, screen_h);


  return 0;
}

void NDL_Quit() {
}
