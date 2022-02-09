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
  size_t len = strlen(hex); //if length is more than 33 too long

  if (len > 34) {
    fp.t = err;
    return fp;
  }

  //get first element in string negative if negative
  char sign = hex[0];
  int index = 0;
  fp.t = valid_positive;
  if (hex[0] == '-') {
    fp.t = valid_negative;
    index++;
  }
  //declare empty string and fill until reaches end of whole part checking for errors in string
  char c = hex[index];
  char whole[16];
  int w_index = 0;
  while (c != '.' && (index < (int)len)) {
    if (c != 'a' && c != 'A' && c != 'b' && c != 'B' &&c != 'c' && c != 'C' && c != 'd' && c != 'D' &&
      c != 'e' && c != 'E' && c != 'f' && c != 'F' && !isdigit(c)) {
    fp.t = err;
    }
    whole[w_index] = c;
    w_index++;
    c = hex[++index];
  } 
  whole[w_index] = '\0'; //check length

  //convert string to unsigned long 
  uint64_t whole_p = (uint64_t) (strtoul(whole, NULL, 16));
  
  //skip decimal index
  index++;
  c = hex[index];

  //declare empty string and fill until reaches end of frac part checking for errors in string
  int f_index = 0;
  uint64_t frac_p = 0;
  size_t f_len = 0;
  if (index < (int) len) {
    char frac[16] = "";
    while (index < (int) len) {
      if (c != 'a' && c != 'A' && c != 'b' && c != 'B' &&c != 'c' && c != 'C' && c != 'd' && c != 'D' &&
      c != 'e' && c != 'E' && c != 'f' && c != 'F' && !isdigit(c)) {
        fp.t = err;
      }
      frac[f_index] = c;
      //printf(frac[f_index]);
      f_index++;
      c = hex[++index];
    }
    frac[f_index] = '\0'; //check length
    frac_p = (uint64_t) (strtoul(frac, NULL, 16));
    f_len = strlen(frac);
    printf("%ul", f_len);
    frac_p = frac_p << (64 - (f_len * 4));
    //error if string longer than 16 
    if (f_len >= 16) {
      fp.t = err;
    }
    }
  //   // if (f_len == 16 && frac[15] != '\0') {
  //   //   fp.t = err;
  //   //       printf("%s", "lemon");

  //   // }
  // }
  size_t w_len = strlen(whole);
  //error if string longer than 16 
  if (w_len >= 16) {
    fp.t = err;
  }
  // if (w_len == 16 && whole[15] != '\0') {
  //       printf("%s", "pear");

  //   fp.t = err;
 // }

  fp.w = whole_p;
  fp.f = frac_p;
  return fp;
}


uint64_t fixedpoint_whole_part(Fixedpoint val) {
  return val.w;
}

uint64_t fixedpoint_frac_part(Fixedpoint val) {
  return val.f;
}

/*
Function to compare magnitudes of fixedpoint values
Parameters:
  left - left hand value to be compared
  right - right hand value to be compared
Returns:
  1 if left > right
  -1 if left < right
  0 if left = right
  */
int compare_abs_value(Fixedpoint left, Fixedpoint right ) {
  int r; 
  if (left.w > right.w) {
    r = 1;
  }

  if (left.w < right.w) {
    r = -1;
  }

  if (left.w == right.w) {
    if (left.f < right.f) {
      r = -1;
    } else if (left.f > right.f) {
      r = 1;
    } else if (left.f == right.f) {
      r = 0;
    }
  }
  return r;
  }


/*Function to check for overflow between values & original
Parameters:
  val1 - first value
  val2 - second value
  orig - value being compared/checked for incidence of overflow
Returns
  1 if parameter values are indicative of overflow
  0 if not
  */
int check_overflow(uint64_t val1, uint64_t val2, uint64_t orig) {
  if (orig < val1 || orig < val2) {
    return 1;
  }
  return 0;
}

Fixedpoint add_same_sign(Fixedpoint left, Fixedpoint right) {
  Fixedpoint sum;
  uint64_t whole_sum = left.w + right.w;
    if (left.t == valid_negative) {
      sum.t = valid_negative;
    } else {
      sum.t = valid_positive;
    }
  sum.w = whole_sum;
  
  //check for overflow
  if (check_overflow(left.w, right.w, whole_sum) == 1) {
      if (left.t == valid_positive) {
      sum.t = overflow_pos;
      return sum;
    } else if (left.t == valid_negative) {
      sum.t = overflow_neg;
      return sum;
    }
  }
  uint64_t frac_sum = left.f + right.f;
  sum.f = frac_sum;
  if (check_overflow(left.f, right.f, frac_sum) == 1) {
    whole_sum++;
    sum.w = whole_sum;
  
    //check for overflow after incrementing whole_sum
    if (whole_sum < left.w || whole_sum < right.w) {
      if (left.t == valid_negative) {
        sum.t = overflow_neg;
        return sum;
      } else {
        sum.t = overflow_pos;
        return sum;
      }
    }
  }
  sum.w = whole_sum;
  sum.f = frac_sum;
  return sum;
  
} 

Fixedpoint add_opposite_sign(Fixedpoint left, Fixedpoint right) {
  uint64_t whole_sum = 0;
  int res = compare_abs_value(left, right);
  
  Fixedpoint max;
  Fixedpoint min;
  Fixedpoint sum;

  //if magnitude of left is greater than right
  if (res == 1) {
    max.w = left.w;
    max.f = left.f;
    max.t = left.t;

    min.w = right.w;
    min.f = right.f;
    min.t = left.t;

  } else if (res == -1) { //magnitude left less than right
    max = right;
    min = left;

  } else if (res == 0) { //equal in magnitude
    sum.w = 0;
    sum.f = 0;
    sum.t = valid_positive;
    return sum;
  }

  whole_sum = max.w - min.w; //compute magnitude difference

  if (max.f < min.f) { //carry over in subtraction
    whole_sum = whole_sum - 1;
  }
  
  uint64_t frac_sum = max.f - min.f;

  //result takes sign of bigger magnitude
  sum.t = max.t;
  sum.w = whole_sum;
  sum.f = frac_sum;
  return sum;
  
}



Fixedpoint fixedpoint_add(Fixedpoint left, Fixedpoint right) {
  Fixedpoint sum;

  //same sign
  if ((fixedpoint_is_neg(left) == 0 && fixedpoint_is_neg(right) == 0) ||  (fixedpoint_is_neg(left) == 1 && fixedpoint_is_neg(right) == 1)) {
    sum = add_same_sign(left, right);
  }

  //opposite signs
  if ((fixedpoint_is_neg(left) == 0 && fixedpoint_is_neg(right) == 1) || (fixedpoint_is_neg(left) == 1 && fixedpoint_is_neg(right) == 0)) {
    sum = add_opposite_sign(left, right);
  } 

  return sum;
}

Fixedpoint fixedpoint_sub(Fixedpoint left, Fixedpoint right) {
  Fixedpoint fp = fixedpoint_add(left, fixedpoint_negate(right));
  return fp;
}


Fixedpoint fixedpoint_negate(Fixedpoint val) {
  if (!fixedpoint_is_zero(val)) {
    if (val.t == valid_positive) {
    val.t = valid_negative;
    } else {
      val.t = valid_positive;
    }
  }
  return val;
}
 
Fixedpoint fixedpoint_halve(Fixedpoint val) {
    uint64_t whole_h;
    uint64_t frac_h;
    if (val.f & 1) {
    if (fixedpoint_is_neg(val)) { 
      val.t  = underflow_neg;
    } else {
      val.t = underflow_pos;
    }
  }
  whole_h = val.w >> 1;
  frac_h = val.f >> 1;

  if (val.w & 1) {
    frac_h += 1UL << 63;
  }
  Fixedpoint fp = fixedpoint_create2(whole_h, frac_h);
  if (val.t == underflow_pos) {
    fp.t = underflow_pos;
  } else if (val.t == underflow_neg) {
    fp.t =underflow_neg;
  } else if (val.t == valid_negative) {
    fp.t = valid_negative;
  }
  return fp;
  //have to check for underflow 
}

Fixedpoint fixedpoint_double(Fixedpoint val) {
  Fixedpoint fp = fixedpoint_add(val, val);
  return fp;
  // uint64_t orig_w = val.w;
  // uint64_t orig_f = val.f;

  // if (val.w > UINT64_MAX) {
  //   if (val.t == valid_negative) {
  //     val.t = overflow_neg;
  //   } else {
  //     val.t = overflow_pos;
  //   }
  // }
  // val.w = val.w << 1;
  // val.f = val.f << 1;

  // return val;
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

  char *s = malloc(100); //to store hex
  strcpy(s, "<invalid>");

  int neg = 0;
  if (fixedpoint_is_neg(val) == 1) {
    neg = 1;
    
  }
  if (val.f == 0) {
    if (neg == 1) { //either mark as negative or not
      sprintf(s, "-%lx", val.w);
    } else {
      sprintf(s, "%lx", val.w);
    }
    return s;
  } else {
    if (neg == 1) {
      sprintf(s, "-%lx.%016lx", val.w, val.f); //copy values into string
    } else {
      sprintf(s, "%lx.%016lx", val.w, val.f);
    }

    
  }

  //to trim trailing zeroes
  size_t index = strlen(s) - 1;
  while (s[index] == '0') {
    index--;
  }
  s[++index] = '\0';
  return s;
}
