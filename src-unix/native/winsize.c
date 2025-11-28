#include <caml/mlvalues.h>

/* Portable unused parameter macro */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
#define unused(x) [[maybe_unused]] x
#elif defined(__GNUC__)
#define unused(x) __attribute__((unused)) x
#else
#define unused(x) x
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

CAMLprim value caml_notty_winsize (unused(value vfd))
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hConsole == INVALID_HANDLE_VALUE) return Val_int(0);

  if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
    /* Use window dimensions (visible area) not buffer size */
    int columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    return Val_int((columns << 16) + ((rows & 0x7fff) << 1));
  }
  return Val_int(0);
}

CAMLprim value caml_notty_winch_number (unused(value vunit)) {
  /* Windows doesn't have SIGWINCH, return 0 to indicate no signal */
  return Val_int(0);
}

#else

#include <sys/ioctl.h>
#include <signal.h>

#ifdef __HAIKU__
/* On some platforms, ioctl() is declared in <unistd.h>. */
#include <unistd.h>
#endif

CAMLprim value caml_notty_winsize (value vfd) {
  int fd = Int_val (vfd);
  struct winsize w;
  if (ioctl (fd, TIOCGWINSZ, &w) >= 0)
    return Val_int ((w.ws_col << 16) + ((w.ws_row & 0x7fff) << 1));
  return Val_int (0);
}

CAMLprim value caml_notty_winch_number (unused(value vunit)) {
  return Val_int (SIGWINCH);
}

#endif
