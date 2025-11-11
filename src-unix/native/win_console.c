#include <caml/mlvalues.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/* Some older MinGW/CYGWIN distributions don't define these */
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

#ifndef ENABLE_VIRTUAL_TERMINAL_INPUT
#define ENABLE_VIRTUAL_TERMINAL_INPUT 0x0200
#endif

#ifndef DISABLE_NEWLINE_AUTO_RETURN
#define DISABLE_NEWLINE_AUTO_RETURN 0x0008
#endif

/* Enable ANSI escape sequence processing on Windows 10+ console
   Returns 1 on success, 0 on failure */
CAMLprim value caml_notty_setup_windows_console(value vunit) {
  (void) vunit;

  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);

  if (hOut == INVALID_HANDLE_VALUE || hIn == INVALID_HANDLE_VALUE) {
    return Val_int(0);
  }

  DWORD outMode = 0, inMode = 0;

  /* Enable VT processing for output (colors, cursor control, etc.) */
  if (GetConsoleMode(hOut, &outMode)) {
    outMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    /* Disable wrapping at end of line to avoid scrolling issues */
    outMode |= DISABLE_NEWLINE_AUTO_RETURN;
    if (!SetConsoleMode(hOut, outMode)) {
      return Val_int(0);
    }
  } else {
    return Val_int(0);
  }

  /* Enable VT processing for input (better key handling) */
  if (GetConsoleMode(hIn, &inMode)) {
    inMode |= ENABLE_VIRTUAL_TERMINAL_INPUT;
    /* Disable line input and echo for raw input */
    inMode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
    /* Enable window input for resize events */
    inMode |= ENABLE_WINDOW_INPUT;
    if (!SetConsoleMode(hIn, inMode)) {
      /* Input mode failure is not critical, continue */
    }
  }

  return Val_int(1);
}

#else

/* Non-Windows platforms: no-op, always return success */
CAMLprim value caml_notty_setup_windows_console(value vunit) {
  (void) vunit;
  return Val_int(1);
}

#endif
