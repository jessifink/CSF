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
  //need to create a fixed point first 
  char sign = hex[0];
  int index = 0;
  fp.t = valid_positive;
  if (hex[0] == '-') {
    printf("%s", "heyo");
    fp.t = valid_negative;
    index++;
  }
  char c = hex[index];
  char whole[16] = "";
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
  uint64_t whole_p = (uint64_t) (strtoul(whole, NULL, 16));
  //printf("whole : %s \n", whole);
  index++;
  c = hex[index];
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
      f_index++;
      c = hex[++index];
    }
    frac[f_index] = '\0'; //check length
    //printf("frac : %s \n", frac);
    frac_p = (uint64_t) (strtoul(frac, NULL, 16));
    f_len = strlen(frac);
    frac_p = frac_p << (64 - (f_len * 4));
    if (f_len >= 16) {
      fp.t = err;
  }
  }
  size_t w_len = strlen(whole);
  if (w_len >= 16 ) {
    fp.t = err;
    //return a Fixedpoint value for which a fixedpoint_is_err returns true
  } 
  //check if whole and fractional are valid hex string with helper function
  //is each character a number or abcdef (capital and lowercase)
  //strchr checks a given string for first ocurrence of a char. contains
  fp.w = whole_p;
  fp.f = frac_p;
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

  if (left.w < right.w) {
    return -1;
  }

  if (left.w == right.w) {
    if (left.f < right.f) {
      return -1;
    } else if (left.f > right.f) {
      return 1;
    } else if (left.f == right.f) {
      return 0;
    }
  }

  
  /*if (left.w > right.w) {
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
    }*/

  }


int carry_frac(uint64_t l, uint64_t r){
  uint64_t sum = l + r;
  if (sum < l || sum < r) {
    return 1;
  } 
  return 0;
}

int check_overflow(uint64_t val1, uint64_t val2, uint64_t orig) {
  if (orig < val1 || orig < val2) {
    return 1;
  }
  return 0;
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


//sign is same: two positive
//add magnituude of whole parts & check for overflow
//add fractional parts & check fraction for overflow
//if fraction has overflow, add 1 for overflow, and check whole for overflow! important edge case

//opposite sign
//compare magnitudes -- subtract smaller from the bigger regardless of sign
//find magnitude difference
//to carry, check if left is less than the right frac value. If so, subtract
//1 from left, then just subtract. 
//sign for result: result takes sign of bigger magnitude

//add magnitudes, check for overflow
//sign is different

//same sign
if ((fixedpoint_is_neg(left) == 0 && fixedpoint_is_neg(right) == 0) ||  (fixedpoint_is_neg(left) == 1 && fixedpoint_is_neg(right) == 1)) {
  whole_sum = left.w + right.w;
  //check for overflow
  if (check_overflow(left.w, right.w, whole_sum) == 1) {
    if (left.t == valid_positive) {
      printf("%s", "hello");
      sum.t = overflow_pos;
      return sum;
    } else if (left.t == valid_negative) {
      printf("%s", "jessi");
      sum.t = overflow_neg;
      return sum;
    }
  }
  frac_sum = left.f + right.f;
  if (check_overflow(left.f, right.f, frac_sum) == 1) {
    whole_sum = whole_sum + 1;
    if (left.t == valid_negative) {
      sum.t = overflow_neg;
      return sum;
      printf("%s", "pear");
    } else {
      sum.t = overflow_pos;
      return sum;
      printf("%s", "mango");
    }

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
  sum.t = left.t;
  return sum;
}

//opposite signs
if ((fixedpoint_is_neg(left) == 0 && fixedpoint_is_neg(right) == 1) || (fixedpoint_is_neg(left) == 1 && fixedpoint_is_neg(right) == 0)) {
  int res = compare_abs_value(left, right);
  
  Fixedpoint max;
  Fixedpoint min;
  if (res == 1) {
    max = left;
    min = right;
  } else {
    max = right;
    min = left;
  }

  whole_sum = max.w - min.w;
  
  if (max.f < min.f) {
    max.w--;
  }
  frac_sum = max.f - min.f;

  //result takes sign of bigger magnitude
  sum.t = max.t;
  sum.w = whole_sum;
  sum.f = frac_sum;
  return sum;
}
}


  /*

  if (fixedpoint_is_neg(left) == 0 && fixedpoint_is_neg(right) == 0) {
    whole_sum = left.w + right.w;
    frac_sum = left.f + right.f;
    whole_sum += carry_frac(left.f, right.f);
    if (whole_sum < left.w || whole_sum < right.w) {
      sum.t = overflow_pos;
    }
  }

  if (fixedpoint_is_neg(left) == 1 && fixedpoint_is_neg(right) == 1) {
    whole_sum = left.w + right.w;
    frac_sum = left.f + right.f;
    whole_sum += carry_frac(left.f, right.f);
    if (whole_sum < left.w || whole_sum < right.w) {
      sum.t = overflow_neg;
    }
    sum = fixedpoint_negate(sum);
  }

  if (fixedpoint_is_neg(left) == 0 && fixedpoint_is_neg(right) == 1) {
    if (compare_abs_value(left, right) == 1) {
      whole_sum = left.w - right.w;
      frac_sum = left.f - right.f;
      whole_sum += carry_frac(left.f, right.f);
    } else if (compare_abs_value(left, right) == -1) {
      whole_sum = right.w - left.w;
      frac_sum = right.f - left.f;
      whole_sum += carry_frac(left.f, right.f);
      sum = fixedpoint_negate(sum);
    }
  }

    if (fixedpoint_is_neg(left) == 1 && fixedpoint_is_neg(right) == 0) {
    if (compare_abs_value(left, right) == 1) {
      whole_sum = right.w - left.w;
      frac_sum = right.w - left.w;
      whole_sum += carry_frac(left.f, right.f);
      sum = fixedpoint_negate(sum);
    } else if (compare_abs_value(left, right) == -1) {
      whole_sum = right.w - left.w;
      frac_sum = right.f - left.f;
      whole_sum += carry_frac(left.f, right.f);
    }
  }
  sum.w = whole_sum;
  sum.f = frac_sum;
  return sum;

  
  }*/


  /*
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
  */
//}


Fixedpoint fixedpoint_sub(Fixedpoint left, Fixedpoint right) {
  Fixedpoint fp;
  uint64_t whole_sum;
  uint64_t frac_sum;
  if (fixedpoint_is_neg(right)) {
    right.t = valid_positive;
  }
  fp = fixedpoint_add(left, fixedpoint_negate(right));
  return fp;

  // - - - 
  // + - + --
  // - - + -- 
  // + - - --

  /*
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
  */
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
      return val;
    } else {
      val.t = underflow_pos;
      return val;
    }
  }
  val.w = val.w >> 1;
  val.f = val.f >> 1;
  return val;
  //have to check for underflow 
}

Fixedpoint fixedpoint_double(Fixedpoint val) {
  return fixedpoint_add(val,val);
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

  char *s = malloc(100);
  strcpy(s, "<invalid>");

  //1. check if negative and add neg sign
  //if no frac part, copy over data with sprintf. 

  if (fixedpoint_is_neg(val) == 1) {
    s[0] = '-';
  }
  if (val.f == 0) {
    sprintf(s, "%lx", val.w);
    return s;
  } else {
    sprintf(s, "%lx.%016lx", val.w, val.f);
  }

  //to trim trailing zeroes
  size_t index = strlen(s) - 1;
  char c = s[index];
  while (s == '0') {
    index--;
  }
  s[index] = '\0';
  
  //to trim trailing zeroes, start at the end of the array.
  //while loop to step backwards starting from last digit until 0
  //if it is zero, set it to null character
  return s;
}
