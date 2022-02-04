#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "fixedpoint.h"

// You can remove this once all of the functions are fully implemented
static Fixedpoint DUMMY;

Fixedpoint fixedpoint_create(uint64_t whole) {
  Fixedpoint fp;
  fp.w = whole;
  fp.f = 0;
  Tag t = valid;
  return fp;
}

Fixedpoint fixedpoint_create2(uint64_t whole, uint64_t frac) {
  Fixedpoint fp;
  fp.w = whole;
  fp.f = frac;
  Tag t = valid;
  return fp;
}

Fixedpoint fixedpoint_create_from_hex(const char *hex) {
  // TODO: implement
  assert(0);
  return DUMMY;
}

uint64_t fixedpoint_whole_part(Fixedpoint val) {
  return val.w;
}

uint64_t fixedpoint_frac_part(Fixedpoint val) {
  return val.f;
}

Fixedpoint fixedpoint_add(Fixedpoint left, Fixedpoint right) {
  Fixedpoint whole_sum = left.w + right.w;
  Fixedpoint frac_sum = left.f + right.f;
  Fixedpoint sum = fixedpoint_create2(whole_sum, frac_sum);
  //if I understood correctly I think this is right 
   if (sum < left || sum < right) {
     if (left > 0 && right > 0) {
       sum.t = overflow_pos;
     } else if (left < 0 && right < 0) {
       sum.t = overflow_neg;
     } else if (left < 0 && right > 0 && abs(right) > abs(left)) {
       sum.t = overflow_pos
     } else {
       sum.t = overflow_neg;
     }
  }
  return sum;
}

Fixedpoint fixedpoint_sub(Fixedpoint left, Fixedpoint right) {
  Fixedpoint whole_sum = left.w - right.w;
  Fixedpoint frac_sum = left.f - right.f;
  Fixedpoint sum = fixedpoint_create2(whole_sum, frac_sum);
  if (sum > left || sum > right) {
    //there is overflow 
  }
  return sum;
}

Fixedpoint fixedpoint_negate(Fixedpoint val) {
  val.t = negative;
  return val;
}

Fixedpoint fixedpoint_halve(Fixedpoint val) {
  val = val >> 1;
  if (val & 1 == 0) {// I think this is 0 but could be reversed 
    if (val.t != negaative) { 
      val.t = underflow_neg;
    } else {
      val.t = underflow_pos;
    }
  }
  return val;
  //have to check for underflow 
}

Fixedpoint fixedpoint_double(Fixedpoint val) {
  Fixedpoint val_doubled = val << 1;
  if (val_doubled < val) {
    val_doubled.t = overflow_pos;
  }
  return val_doubled;
  //have to check for overflow 
}

int fixedpoint_compare(Fixedpoint left, Fixedpoint right) {
  // TODO: implement
  assert(0);
  return 0;
}

int fixedpoint_is_zero(Fixedpoint val) {
  if ((val.w == 0) && (val.f == 0)) {
    return 1;
  } else {
    return 0;
  }
}

int fixedpoint_is_err(Fixedpoint val) {
  // TODO: implement
  assert(0);
  return 0;
}

int fixedpoint_is_neg(Fixedpoint val) {
  if (val.t == negative) {
    return 1;
  }
  return 0;
}

int fixedpoint_is_overflow_neg(Fixedpoint val) {
  if (val.t == overflow_neg) {
    return 1;
  }
  return 0;
}

int fixedpoint_is_overflow_pos(Fixedpoint val) {
  if (val.t == overflow_pos) {
    return 1;
  }
  return 0;
}

int fixedpoint_is_underflow_neg(Fixedpoint val) {
  if (val.t == underflow_neg) {
    return 1;
  }
  return 0;
}

int fixedpoint_is_underflow_pos(Fixedpoint val) {
  if (val.t == underflow_pos) {
    return 1;
  }
  return 0;
}

int fixedpoint_is_valid(Fixedpoint val) {
  if (val.t == valid) {
    return 1;
  }
  return 0;
}

char *fixedpoint_format_as_hex(Fixedpoint val) {
  // TODO: implement
  assert(0);
  char *s = malloc(20);
  strcpy(s, "<invalid>");
  return s;
}

//overflow pos - add two numbers carry the one 
// two big numbers add together too big to be able tofit in spot cuts off number is stored is smaller than the two numbers that you added together 
//overflow negative - is just adding negaative numbers become overally negative 
//underflow  pos - if you have something in the least significatn bit spot and divide by two its going to get shifted out of existence   
// bit masking take two bit numbers if you and it with a 1 -- 100101101 &  1 if both that 0 in any other case 
//underflow neg - same thing but with negative numbers 
