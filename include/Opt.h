#ifndef _OPT_H_
#define _OPT_H_

// Enumerated type to encode optimization levels
// O2 includes O2 optimizations as well as all lower optimization levels
typedef enum Opt {
    O0,     // No optimizations
    O1,     // Peephole optimizations
    O2,     // Dead code removal, loop unrolling
} Opt;

#endif