#include "buzzer.h"
char toggle(char state)		/* always toggle! */
{
 
  
  switch (state) {
  case 0:
    
    buzzer_set_period(1000);                  /*Play sound! */

 
    break;
  
	
  case 1:
    
    buzzer_set_period(300);                     /* higher freq sound! */
    break;
  }

  return 1;
} 

