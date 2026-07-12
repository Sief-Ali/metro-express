#include "serial.h"

int  main(void) {

  Serial_Init(9600);

  Serial_Write("Metro Express\r\n");
  
  while (1)
  {
      uint8_t c;
  
      if (Serial_Read(&c))
      {
          Serial_WriteByte(c);
      }
  }
}