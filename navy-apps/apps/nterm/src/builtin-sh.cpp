#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>

char handle_key(SDL_Event *ev);

static void sh_printf(const char *format, ...) {
  static char buf[256] = {};
  va_list ap;
  va_start(ap, format);
  int len = vsnprintf(buf, 256, format, ap);
  va_end(ap);
  term->write(buf, len);
}

static void sh_banner() {
  sh_printf("Built-in Shell in NTerm (NJU Terminal)\n\n");
}

static void sh_prompt() {
  sh_printf("sh> ");
}

static void sh_handle_cmd(const char *cmd) {
  printf("built-in cmd: %s\n", cmd);
  char buf[100];
  char argvs[20][30];
  int argcs = 0, index = 0;
  sscanf(cmd, "%s", buf);
  index = strlen(buf);
  while(sscanf(cmd + index, "%s", argvs[argcs])) {
    index += strlen(argvs[argcs]);
    printf("%d:  %s\n", argcs, argvs[argcs]);
    argcs++;
  }
  
  execvp(buf, NULL);
  execve(buf, NULL, NULL);

}

void builtin_sh_run() {
  sh_banner();
  sh_prompt();

  setenv("PATH", "/bin", 0);
  while (1) {
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
        const char *res = term->keypress(handle_key(&ev));
        if (res) {
          sh_handle_cmd(res);
          sh_prompt();
        }
      }
    }
    refresh_terminal();
  }
}
