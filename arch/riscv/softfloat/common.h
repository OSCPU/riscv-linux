// from <stdint.h>
typedef signed char int_fast8_t;
typedef long int int_fast16_t;
typedef long int int_fast32_t;
typedef long int int_fast64_t;
typedef unsigned char uint_fast8_t;
typedef unsigned long int uint_fast16_t;
typedef unsigned long int uint_fast32_t;
typedef unsigned long int uint_fast64_t;
typedef unsigned char uint_least8_t;

#define __INT64_C(c)	c ## L
#define __UINT64_C(c)	c ## UL

#define INT64_C(c)	c ## L
#define UINT64_C(c)	c ## UL

# define INT32_MIN    (-2147483647-1)
# define INT64_MIN    (-__INT64_C(9223372036854775807)-1)
# define INT32_MAX    (2147483647)
# define INT64_MAX    (__INT64_C(9223372036854775807))
# define UINT32_MAX   (4294967295U)
# define UINT64_MAX   (__UINT64_C(18446744073709551615))
