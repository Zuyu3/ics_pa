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

uint32_t NDL_GetTicks() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int NDL_PollEvent(char *buf, int len) {
  FILE *fp = fopen("/dev/events", "r");
  int res = fread(buf, len, 1, fp);
  fclose(fp);
  return res? 1 : 0;
}

void NDL_OpenCanvas(int *w, int *h) {
  if(*w == 0 && *h == 0) {
    *w == screen_w;
    *h == screen_h;
    return;
  }
  else if(*w > screen_w || *h > screen_h) {
    printf("Error. Canvas larger than screen.\n");
  }
  

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
  char tokens[32];
  FILE *fp = fopen("/proc/dispinfo", "r");
  int config_len =  fscanf(fp, "%s", buf);

  int id = 0, temp, temp_w = -1, temp_h = -1;
  while(id < config_len) {
      if(strcmp(buf + id, "WIDTH") == 0) {
        printf("hit width at %d\n", id);
        temp_w = 0;
        for(temp = id + 5; temp < config_len; temp++) {
          if(buf[temp] >= '0' && buf[temp] <= '9')
            temp_w = temp_w * 10 + buf[temp] - '0';
          if(buf[temp] == '\n'){
            id = temp;
            break;
          }
        }
      }
      else if(strcmp(buf + id, "HEIGHT") == 0) {
        printf("hit height at %d\n", id);
        temp_h = 0;
        for(temp = id + 6; temp < config_len; temp++) {
          if(buf[temp] >= '0' && buf[temp] <= '9')
            temp_h = temp_h * 10 + buf[temp] - '0';
          if(buf[temp] == '\n'){
            id = temp;
            break;
          }
        }
      }
      id++;
  }

  if(temp_w == -1 || temp_h == -1)
    assert(0);
  else {
    screen_h = temp_h;
    screen_w = temp_w;
  }

  printf("width is %d, height is %d\n", screen_w, screen_h);


  return 0;
}

void NDL_Quit() {
}
