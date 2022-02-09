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
  fp.t = valid_positive;
  return fp;
}

Fixedpoint fixedpoint_create2(uint64_t whole, uint64_t frac) {
  Fixedpoint fp;
  fp.w = whole;
  fp.f = frac;
  fp.t = valid_positive;
  return fp;
}

Fixedpoint fixedpoint_create_from_hex(const char *hex) {
  Fixedpoint fp;
  size_t len = strlen(hex);
  //need to create a fixed point first 
  char sign = hex[0];
  int index = 0;
  fp.t = valid_positive;
  if (sign == '-') {
    fp.t = valid_negative;
    index++;
  }
  char c = hex[index];
  char whole[16] = "";
  int w_index = 0;
  while (c != '.' && (index < (int)len)) {
    whole[w_index] = c;
    w_index++;
    c = hex[++index];
  } 
  whole[w_index] = '\0';
  printf("whole : %s \n", whole);
  uint64_t whole_p = (uint64_t) (strtoul(whole, NULL, 16));
  index++;
  c = hex[index];
  int f_index = 0;
  uint64_t frac_p = 0;
  size_t f_len = 0;
  if (index < (int) len) {
    char frac[16] = "";
    while (index < (int) len) {
      frac[f_index] = c;
      f_index++;
      c = hex[++index];
    }
  frac[f_index] = '\0';
  printf("frac : %s \n", frac);
  frac_p = (uint64_t) (strtoul(frac, NULL, 16));
  f_len = strlen(frac);
  frac_p = frac_p << (64 - (f_len * 4));
  }
  size_t w_len = strlen(whole);
  fp.w = whole_p;
  fp.f = frac_p;
  if (sizeof(w_len) > 16 || sizeof(f_len) > 16) {
    fp.t = err;
    fp.w = 0;
    fp.f = 0;
    return fp;
    //return a Fixedpoint value for which a fixedpoint_is_err returns true
  } 

  // if (frac_p == '\0') {
  //   return fp;
  //   //return fixedpoint_create(fp.w); 
  // }
  return fp;
}

uint64_t fixedpoint_whole_part(Fixedpoint val) {
  return val.w;
}

uint64_t fixedpoint_frac_part(Fixedpoint val) {
  return val.f;
}

int compare_abs_value(Fixedpoint left, Fixedpoint right ) {
  if (left.w > right.w) {
    return 1;
  }
  else if (left.w < right.w) {
    return -1;
  } else {
      if (left.f < right.f) {
        return -1;
      } else if (left.f > right.w) {
          return 1;
        } else {
        return 0;
      }
    }
  }

Fixedpoint fixedpoint_add(Fixedpoint left, Fixedpoint right) {
  Fixedpoint sum;
  uint64_t whole_sum;
  uint64_t frac_sum;
  // + + - ----
  // + + + ----
  // - + -   ---
  // - + + ---  // - - - 
  // + - + 
  // - - + 
  // + - - 
  if (fixedpoint_is_neg(left) == 1 && fixedpoint_is_neg(right) == 1 ) {
    whole_sum = left.w + right.w;
    frac_sum = left.f + right.f;
    sum = fixedpoint_create2(whole_sum, frac_sum);
    fixedpoint_negate(sum);
  } else if (fixedpoint_is_neg(left) == 1 && fixedpoint_is_neg(right) == 0 && compare_abs_value(left,right) == 1) {
    whole_sum = left.w - right.w;
    frac_sum = left.f - right.f;
    sum = fixedpoint_create2(whole_sum, frac_sum);
    fixedpoint_negate(sum);
  } else if (fixedpoint_is_neg(left) == 0 && fixedpoint_is_neg(right) == 1 && compare_abs_value(left,right) == 1) {
    whole_sum = left.w - right.w;
    frac_sum = left.f - right.f;
    sum = fixedpoint_create2(whole_sum, frac_sum);
  } else if (fixedpoint_is_neg(left) == 0 && fixedpoint_is_neg(right) == 0){
    whole_sum = left.w + right.w;
    frac_sum = left.f + right.f;
    sum = fixedpoint_create2(whole_sum, frac_sum);
  } else {
    fixedpoint_sub(left,right);
  }
  if (whole_sum < left.w || whole_sum < right.w) {
    if (left.t == valid_negative && right.t == valid_negative) {
      sum.t = overflow_neg;
    } else {
        sum.t = overflow_pos;
      }
    }
  if (frac_sum < left.f || frac_sum < right.f) {
    sum.w++;
    if (left.t == valid_negative && right.t == valid_negative) {
      sum.t = overflow_neg;
    } else {
        sum.t = overflow_pos;
      }
  }
  return sum;
}


Fixedpoint fixedpoint_sub(Fixedpoint left, Fixedpoint right) {
  Fixedpoint fp;
  uint64_t whole_sum;
  uint64_t frac_sum;

  // - - - 
  // + - + --
  // - - + -- 
  // + - - --
  if (!(fixedpoint_is_neg(left) == 0 && fixedpoint_is_neg(right))) {
    whole_sum = left.w - right.w;
    frac_sum = left.f - right.f;
    fp.w = whole_sum;
    fp.f = frac_sum;
    } else if (fixedpoint_is_neg(left) == 1 && fixedpoint_is_neg(right) == 0 && compare_abs_value(left,right) == -1) {
    whole_sum = right.w - left.w;
    frac_sum = right.f - left.f;
    fp = fixedpoint_create2(whole_sum, frac_sum);
  } else if (fixedpoint_is_neg(left) == 0 && fixedpoint_is_neg(right) == 1 && compare_abs_value(left,right) == -1) {
    whole_sum = right.w - left.w;
    frac_sum = right.f - left.f;
    fp = fixedpoint_create2(whole_sum, frac_sum);
    fixedpoint_negate(fp);
  } else {
    whole_sum = left.w - right.w;
    frac_sum = left.f - right.f;
    fp = fixedpoint_create2(whole_sum, frac_sum);
  }

  return fp;
}





Fixedpoint fixedpoint_negate(Fixedpoint val) {
  if (!fixedpoint_is_zero(val)) {
    val.t = valid_negative;
  }
  return val;
}
 
Fixedpoint fixedpoint_halve(Fixedpoint val) {
    if ((val.f & 1) == 1) {
    if (val.t == valid_positive) { 
      val.t  = underflow_neg;
    } else {
      val.t = underflow_pos;
    }
  }
  val.w = val.w >> 1;
  val.f = val.f >> 1;
  return val;
  //have to check for underflow 
}

Fixedpoint fixedpoint_double(Fixedpoint val) {
  
  uint64_t orig_w = val.w;
  uint64_t orig_f = val.f;

  if (val.w > UINT64_MAX) {
    if (val.t == valid_negative) {
      val.t = overflow_neg;
    } else {
      val.t = overflow_pos;
    }
  }
  val.w = val.w << 1;
  val.f = val.f << 1;

  return val;
}

int fixedpoint_compare(Fixedpoint left, Fixedpoint right) {
  if (!(left.t == valid_negative) && right.t == valid_negative) {
    return 1;
  }
  if (!(right.t == valid_negative) && left.t == valid_negative ) {
    return -1;
  }
  if (left.t == valid_negative && right.t == valid_negative) {
    if (left.w > right.w) {
      return -1;
    } else if (right.w > left.w) {
      return 1;
    } else {
      if (left.f > right.f) {
        return -1;
      } else if (right.f > left.f) {
        return 1;
      } else {
        return 0;
      }
    }
  }
  if (!(left.t == valid_negative && right.t == valid_negative)) {
    if (left.w > right.w) {
      return 1;
    } else if (right.w > left.w) {
      return -1;
    } else {
      if (left.f > right.f) {
        return 1;
      } else if (right.f > left.f) {
        return -1;
      } else {
        return 0;
      }
    }
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
  if (val.t == err) {  
    return 1;
  }
  return 0;
}

int fixedpoint_is_neg(Fixedpoint val) {
  if (val.t == valid_negative) {
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
  if (val.t == valid_positive || val.t == valid_negative) {
    return 1;
  }
  return 0;
}

char *fixedpoint_format_as_hex(Fixedpoint val) {
  char *s = malloc(20);
  strcpy(s, "<invalid>");
  //sprintf()
  return s;
}