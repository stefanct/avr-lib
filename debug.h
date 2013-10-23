#include <stdint.h>
#include "macros.h"

#define DEBUG_PIN0(reg)	BIT(D,6,reg)
#define DEBUG_PIN1(reg)	BIT(D,5,reg)
#define DEBUG_PIN2(reg)	BIT(D,1,reg)

#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)
#define VAR_NAME_VALUE(var) #var "="  VALUE(var)
