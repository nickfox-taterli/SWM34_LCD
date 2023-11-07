#include <LowLevelIOInterface.h>
#include "SWM341.h"

#pragma module_name = "?__write"

int LL_fputc(int x)
{
  UART0->DATA = x;
  
  while(!(UART0->CTRL & UART_CTRL_TXIDLE_Msk)){};
  return x;
}

/*
* If the __write implementation uses internal buffering, uncomment
* the following line to ensure that we are called with "buffer" as 0
* (i.e. flush) when the application terminates.
*/

size_t __write(int handle, const unsigned char * buffer, size_t size)
{
  size_t nChars = 0;

  if (buffer == 0)
  {
    /*
     * This means that we should flush internal buffers.  Since we
     * don't we just return.  (Remember, "handle" == -1 means that all
     * handles should be flushed.)
     */
    return 0;
  }

  /* This template only writes to "standard out" and "standard err",
   * for all other file handles it returns failure. */
  if (handle != _LLIO_STDOUT && handle != _LLIO_STDERR)
  {
    return _LLIO_ERROR;
  }

  for (/* Empty */; size != 0; --size)
  {
    if (LL_fputc(*buffer++) < 0)
    {
      return _LLIO_ERROR;
    }

    ++nChars;
  }

  return nChars;
}

