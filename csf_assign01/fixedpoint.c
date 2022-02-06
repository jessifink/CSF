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
  fp.t = valid;
  return fp;
}

Fixedpoint fixedpoint_create2(uint64_t whole, uint64_t frac) {
  Fixedpoint fp;
  fp.w = whole;
  fp.f = frac;
  fp.t = valid;
  return fp;
}

Fixedpoint fixedpoint_create_from_hex(const char *hex) {
  Fixedpoint fp;
  size_t len = strlen(hex);

  char sign = hex[0];
  int index = 0;
  if (sign == '-') {
    fp.t = negative;
    index++;
  }
  char c = hex[index];
  char * whole = "";
  int w_index = 0;
  while (c != '.') {
    whole[w_index] = c;
    w_index++;
    c = hex[index++];
  }

  uint64_t whole_p = (uint64_t) (strtoul(whole, NULL, 16));

  index++;
  char * frac = "";
  int f_index = 0;
  while (index < len) {
    frac[f_index] = c;
    f_index++;
    c = hex[index++];
  }
  uint64_t frac_p = (uint64_t) (strtoul(frac, NULL, 16));
  size_t f_len = strlen(frac_p);
  size_t w_len = strlen(whole_p);

  frac_p = frac_p << (f_len * 4);
  whole_p = whole_p << (w_len * 4);

  uint64_t f = (uint64_t) frac_p;
  uint64_t w = (uint64_t) whole_p;
  fp.w = w;
  fp.f = f;

  return fp;
}

uint64_t fixedpoint_whole_part(Fixedpoint val) {
  return val.w;
}

uint64_t fixedpoint_frac_part(Fixedpoint val) {
  return val.f;
}

Fixedpoint fixedpoint_add(Fixedpoint left, Fixedpoint right) {
  uint64_t whole_sum = left.w + right.w;
  uint64_t frac_sum = left.f + right.f;
  Fixedpoint sum = fixedpoint_create2(whole_sum, frac_sum);
  //if I understood correctly I think this is right 
   if (whole_sum < left.w || whole_sum < right.w) {
     if (left.t == negative && right.t == negative) {
       sum.t = overflow_neg;
     } else {
       sum.t = overflow_pos;
       }
   }

    if (frac_sum < left.f || frac_sum < right.f) {
     if (left.t == negative && right.t == negative) {
       sum.t = overflow_neg;
     } else {
       sum.t = overflow_pos;
       }
   }


  return sum;
}

Fixedpoint fixedpoint_sub(Fixedpoint left, Fixedpoint right) {
  uint64_t whole_sum = left.w - right.w;
  uint64_t frac_sum = left.f - right.f;
  Fixedpoint sum = fixedpoint_create2(whole_sum, frac_sum);
  if (whole_sum > left.w || whole_sum > right.w) {
    //there is overflow 
    if ( !(left.t == negative && right.t == negative)) {
      if (right.t == negative) {
        sum.t = overflow_pos;
      }
      if (left.t == negative) {
        sum.t = overflow_neg;
      }
    }
  }
  return sum;
}

Fixedpoint fixedpoint_negate(Fixedpoint val) {
  val.t = negative;
  return val;
}

Fixedpoint fixedpoint_halve(Fixedpoint val) {
  val.w = val.w >> 1;
  val.f = val.f >> 1;

  if (val.w & 1 == 0) {// I think this is 0 but could be reversed 
    if (val.t != negative) { 
      val.t  = underflow_neg;
    } else {
      val.t = underflow_pos;
    }
  }

    if (val.f & 1 == 0) {// I think this is 0 but could be reversed 
    if (val.t != negative) { 
      val.t  = underflow_neg;
    } else {
      val.t = underflow_pos;
    }
  }
  return val;
  //have to check for underflow 
}

Fixedpoint fixedpoint_double(Fixedpoint val) {
  
  uint64_t orig_w = val.w;
  uint64_t orig_f = val.f;

  val.w = val.w << 1;
  val.f = val.f << 1;

  if (val.w < orig_w) {
    if (val.t == negative) {
      val.t == overflow_neg;
    } else {
      val.t = overflow_pos;
    }
  }
  return val;
  //have to check for overflow 
}

int fixedpoint_compare(Fixedpoint left, Fixedpoint right) {
  if (!(left.t == negative) || right.t == negative) {
    return 1;
  } else if (!(right.t == negative) || left.t == negative) {
    return -1;
  }
  else {
    if (left.t == negative && right.t == negative) {
      if (left.w < right.w) {
        return 1;
      } else if ( left.w > right.w) {
        return -1;
      } else if (left.w == right.w) {
        if (left.f > right.f) {
          return -1;
        } else if (left.f < right.f) {
          return 1;
        } else {
          return 0;
        }
      }
    }
  }
    
    if (left.w == right.w) {
    if (left.f < right.f) {
      return -1;
    } else if (left.f > right.f) {
      return 1;
    } else {
      return 0;
    }
  }

  if (left.w < right.w) {
    return -1;
  }
  if (left.w > right.w) {
    return 1;
  }

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
//underflow  pos - if you have something in the least significatn bit spot and divide by two its going to get shifted out of existence   
// bit masking take two bit numbers if you and it with a 1 -- 100101101 &  1 if both that 0 in any other case 
//underflow neg - same thing but with negative numbers