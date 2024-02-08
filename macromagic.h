#ifndef MACROMAGIC_H
#define MACROMAGIC_H
#ifndef CAT
#define CAT(a, ...) PRIMITIVE_CAT(a, __VA_ARGS__)
#define PRIMITIVE_CAT(a, ...) a ## __VA_ARGS__
#endif
#define FALSE 0
#define TRUE 1
#endif