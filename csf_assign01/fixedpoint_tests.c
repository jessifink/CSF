#include <stdio.h>
#include <stdlib.h>
#include "fixedpoint.h"
#include "tctest.h"

// Test fixture object, has some useful values for testing
typedef struct {
  Fixedpoint zero;
  Fixedpoint one;
  Fixedpoint one_half;
  Fixedpoint one_fourth;
  Fixedpoint large1;
  Fixedpoint large2;
  Fixedpoint max;

  // TODO: add more objects to the test fixture
} TestObjs;

// functions to create and destroy the test fixture
TestObjs *setup(void);
void cleanup(TestObjs *objs);

// test functions
void test_compare(TestObjs *objs);
void test_whole_part(TestObjs *objs);
void test_frac_part(TestObjs *objs);
void test_create_from_hex(TestObjs *objs);
void test_format_as_hex(TestObjs *objs);
void test_negate(TestObjs *objs);
void test_add(TestObjs *objs);
void test_sub(TestObjs *objs);
void test_is_overflow_pos(TestObjs *objs);
void test_is_err(TestObjs *objs);
void test_is_zero(TestObjs *objs);
void test_halve(TestObjs *objs);
void test_double(TestObjs *objs);
// TODO: add more test functions

int main(int argc, char **argv) {
  // if a testname was specified on the command line, only that
  // test function will be executed
  if (argc > 1) {
    tctest_testname_to_execute = argv[1];
  }

  TEST_INIT();

  TEST(test_whole_part);
  TEST(test_compare);
  TEST(test_frac_part);
  TEST(test_create_from_hex);
  TEST(test_format_as_hex);
  TEST(test_negate);
  TEST(test_add);
  TEST(test_sub);
  TEST(test_is_overflow_pos);
  TEST(test_is_err);
  TEST(test_is_zero);
  TEST(test_halve);
  TEST(test_double);

  // IMPORTANT: if you add additional test functions (which you should!),
  // make sure they are included here.  E.g., if you add a test function
  // "my_awesome_tests", you should add
  //
  //   TEST(my_awesome_tests);
  //
  // here. This ensures that your test function will actually be executed.

  TEST_FINI();
}

TestObjs *setup(void) {
  TestObjs *objs = malloc(sizeof(TestObjs));

  objs->zero = fixedpoint_create(0UL);
  objs->one = fixedpoint_create(1UL);
  objs->one_half = fixedpoint_create2(0UL, 0x8000000000000000UL);
  objs->one_fourth = fixedpoint_create2(0UL, 0x4000000000000000UL);
  objs->large1 = fixedpoint_create2(0x4b19efceaUL, 0xec9a1e2418UL);
  objs->large2 = fixedpoint_create2(0xfcbf3d5UL, 0x4d1a23c24fafUL);
  objs->max = fixedpoint_create2(0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL);


  objs->max = fixedpoint_create2(0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL);
  
  return objs;
}

void cleanup(TestObjs *objs) {
  free(objs);
}

void test_halve(TestObjs *objs) {
  Fixedpoint one_halved = fixedpoint_halve(objs->one);
  ASSERT(0x8000000000000000UL == fixedpoint_frac_part(one_halved));
  ASSERT(0UL == fixedpoint_whole_part(one_halved));
  
  Fixedpoint one_half_halved = fixedpoint_halve(objs->one_half);
  ASSERT(0x4000000000000000UL == fixedpoint_frac_part(one_half_halved));
  ASSERT(0UL == fixedpoint_whole_part(one_half_halved));
}
void test_double(TestObjs *objs) {
  Fixedpoint one_doubled = fixedpoint_double(objs->one);
  ASSERT(2UL == fixedpoint_whole_part(one_doubled));
  ASSERT(0UL == fixedpoint_frac_part(one_doubled));

  Fixedpoint neg_one_doubled = fixedpoint_double(objs->one);
  fixedpoint_negate(neg_one_doubled);
  ASSERT(2UL == fixedpoint_whole_part(neg_one_doubled));
  ASSERT(0UL == fixedpoint_frac_part(neg_one_doubled));
  ASSERT(fixedpoint_is_neg(neg_one_doubled));

  Fixedpoint one_fourth_doubled = fixedpoint_double(objs->one_fourth);
  ASSERT(0UL == fixedpoint_whole_part(one_fourth_doubled));
  ASSERT(0x8000000000000000UL == fixedpoint_frac_part(one_fourth_doubled));

}

void test_whole_part(TestObjs *objs) {
  ASSERT(0UL == fixedpoint_whole_part(objs->zero));
  ASSERT(1UL == fixedpoint_whole_part(objs->one));
  ASSERT(0UL == fixedpoint_whole_part(objs->one_half));
  ASSERT(0UL == fixedpoint_whole_part(objs->one_fourth));
  ASSERT(0x4b19efceaUL == fixedpoint_whole_part(objs->large1));
  ASSERT(0xfcbf3d5UL == fixedpoint_whole_part(objs->large2));
}

void test_compare(TestObjs *objs) {
  Fixedpoint one_neg = fixedpoint_negate(objs->one);
  ASSERT(fixedpoint_compare(objs->one,one_neg) == 1);
  ASSERT(fixedpoint_compare(one_neg,objs->one) == -1);
  ASSERT(fixedpoint_compare(objs->one_fourth, objs->one_half) == -1);
  ASSERT(fixedpoint_compare(objs->one_half, objs->one_fourth) == 1);
  ASSERT(fixedpoint_compare(objs->one_half, objs->one_half) == 0);
  
}

void test_frac_part(TestObjs *objs) {
  ASSERT(0UL == fixedpoint_frac_part(objs->zero));
  ASSERT(0UL == fixedpoint_frac_part(objs->one));
  ASSERT(0x8000000000000000UL == fixedpoint_frac_part(objs->one_half));
  ASSERT(0x4000000000000000UL == fixedpoint_frac_part(objs->one_fourth));
  ASSERT(0xec9a1e2418UL == fixedpoint_frac_part(objs->large1));
  ASSERT(0x4d1a23c24fafUL == fixedpoint_frac_part(objs->large2));
}

void test_create_from_hex(TestObjs *objs) {
  (void) objs;

  Fixedpoint v = fixedpoint_create_from_hex("0");
  ASSERT(0UL == fixedpoint_whole_part(v));
  ASSERT(0UL == fixedpoint_frac_part(v));

  Fixedpoint v2 = fixedpoint_create_from_hex("0.0");
  ASSERT(0UL == fixedpoint_whole_part(v2));
  ASSERT(0UL == fixedpoint_frac_part(v2));

  Fixedpoint v3 = fixedpoint_create_from_hex("1");
  ASSERT(1UL == fixedpoint_whole_part(v3));
  ASSERT(0UL == fixedpoint_frac_part(v3));
  
  Fixedpoint v6 = fixedpoint_create_from_hex("-1");
  ASSERT(1UL == fixedpoint_whole_part(v6));
  ASSERT(0UL == fixedpoint_frac_part(v6));
  ASSERT(fixedpoint_is_neg(v6));

  Fixedpoint v5 = fixedpoint_create_from_hex("a");
  ASSERT(0xaUL == fixedpoint_whole_part(v5));
  ASSERT(0UL == fixedpoint_frac_part(v5));

  Fixedpoint v4 = fixedpoint_create_from_hex("1.0");
  ASSERT(0UL == fixedpoint_frac_part(v4));
  printf("%lu \n", fixedpoint_whole_part(v4) );
  //print in hexidecimal is lx 
  ASSERT(1UL == fixedpoint_whole_part(v4));


  Fixedpoint val1 = fixedpoint_create_from_hex("f6a5865.00f2");
  ASSERT(fixedpoint_is_valid(val1));
  ASSERT(0xf6a5865UL == fixedpoint_whole_part(val1));
  ASSERT(0x00f2000000000000UL == fixedpoint_frac_part(val1));

  val1 = fixedpoint_create_from_hex("-f6a5865.00f2");
  ASSERT(fixedpoint_is_valid(val1));
  ASSERT(fixedpoint_is_neg(val1));
  ASSERT(0xf6a5865UL == fixedpoint_whole_part(val1));
  ASSERT(0x00f2000000000000UL == fixedpoint_frac_part(val1));

  Fixedpoint val2 = fixedpoint_create_from_hex("-126a0b9b1f810.201cfa1");
  ASSERT(fixedpoint_is_valid(val2));
  ASSERT(fixedpoint_is_neg(val2));
  ASSERT(0x126a0b9b1f810UL == fixedpoint_whole_part(val2));
  ASSERT(0x201cfa1000000000UL == fixedpoint_frac_part(val2));
  

}

void test_format_as_hex(TestObjs *objs) {
  char *s;

  s = fixedpoint_format_as_hex(objs->zero);
  ASSERT(0 == strcmp(s, "0"));
  free(s);

  s = fixedpoint_format_as_hex(objs->one);
  ASSERT(0 == strcmp(s, "1"));
  free(s);

  Fixedpoint neg = fixedpoint_create(1);
  neg.t = valid_negative;
  s = fixedpoint_format_as_hex(neg);
  ASSERT(0 == strcmp(s, "-1"));
  free(s);

  s = fixedpoint_format_as_hex(objs->one_half);
  ASSERT(0 == strcmp(s, "0.8"));
  free(s);

  s = fixedpoint_format_as_hex(objs->one_fourth);
  ASSERT(0 == strcmp(s, "0.4"));
  free(s);

  s = fixedpoint_format_as_hex(objs->large1);
  ASSERT(0 == strcmp(s, "4b19efcea.000000ec9a1e2418"));
  free(s);

  s = fixedpoint_format_as_hex(objs->large2);
  ASSERT(0 == strcmp(s, "fcbf3d5.00004d1a23c24faf"));
  free(s);
}

void test_negate(TestObjs *objs) {
  // none of the test fixture objects are negative
  ASSERT(!fixedpoint_is_neg(objs->zero));
  ASSERT(!fixedpoint_is_neg(objs->one));
  ASSERT(!fixedpoint_is_neg(objs->one_half));
  ASSERT(!fixedpoint_is_neg(objs->one_fourth));
  ASSERT(!fixedpoint_is_neg(objs->large1));
  ASSERT(!fixedpoint_is_neg(objs->large2));

  // negate the test fixture values
  Fixedpoint zero_neg = fixedpoint_negate(objs->zero);
  Fixedpoint one_neg = fixedpoint_negate(objs->one);
  Fixedpoint one_half_neg = fixedpoint_negate(objs->one_half);
  Fixedpoint one_fourth_neg = fixedpoint_negate(objs->one_fourth);
  Fixedpoint large1_neg = fixedpoint_negate(objs->large1);
  Fixedpoint large2_neg = fixedpoint_negate(objs->large2);

  // zero does not become negative when negated
  ASSERT(!fixedpoint_is_neg(zero_neg));

  // all of the other values should have become negative when negated
  ASSERT(fixedpoint_is_neg(one_neg));
  ASSERT(fixedpoint_is_neg(one_half_neg));
  ASSERT(fixedpoint_is_neg(one_fourth_neg));
  ASSERT(fixedpoint_is_neg(large1_neg));
  ASSERT(fixedpoint_is_neg(large2_neg));

  // magnitudes should stay the same
  ASSERT(0UL == fixedpoint_whole_part(objs->zero));
  ASSERT(1UL == fixedpoint_whole_part(objs->one));
  ASSERT(0UL == fixedpoint_whole_part(objs->one_half));
  ASSERT(0UL == fixedpoint_whole_part(objs->one_fourth));
  ASSERT(0x4b19efceaUL == fixedpoint_whole_part(objs->large1));
  ASSERT(0xfcbf3d5UL == fixedpoint_whole_part(objs->large2));
  ASSERT(0UL == fixedpoint_frac_part(objs->zero));
  ASSERT(0UL == fixedpoint_frac_part(objs->one));
  ASSERT(0x8000000000000000UL == fixedpoint_frac_part(objs->one_half));
  ASSERT(0x4000000000000000UL == fixedpoint_frac_part(objs->one_fourth));
  ASSERT(0xec9a1e2418UL == fixedpoint_frac_part(objs->large1));
  ASSERT(0x4d1a23c24fafUL == fixedpoint_frac_part(objs->large2));
}

void test_add(TestObjs *objs) {
  (void) objs;
  Fixedpoint sum = fixedpoint_add(objs->one, objs->one);
  ASSERT(2UL == fixedpoint_whole_part(sum));
  Fixedpoint lhs, rhs;

  lhs = objs->zero;
  rhs = objs->one;
  sum = fixedpoint_add(lhs, rhs);
  ASSERT(!fixedpoint_is_neg(sum));
  ASSERT(1UL == fixedpoint_whole_part(sum));

  lhs = objs->zero;
  rhs = objs->zero;
  sum = fixedpoint_add(lhs, rhs);
  ASSERT(!fixedpoint_is_neg(sum));
  ASSERT(0UL == fixedpoint_whole_part(sum));

  lhs = objs->large1;
  lhs = fixedpoint_negate(lhs);
  ASSERT(fixedpoint_is_neg((lhs)) == 1);
  rhs = objs->one;
  sum = fixedpoint_add(lhs, rhs);
  ASSERT(fixedpoint_is_neg(sum));

  (void) objs;
  sum = fixedpoint_add(objs->one, objs->one);
  ASSERT(2UL == fixedpoint_whole_part(sum));
  
  //Fixedpoint lhs, rhs;

  lhs = fixedpoint_create_from_hex("-c7252a193ae07.7a51de9ea0538c5");
  rhs = fixedpoint_create_from_hex("d09079.1e6d601");
  sum = fixedpoint_add(lhs, rhs);
  ASSERT(fixedpoint_is_neg(sum));
  ASSERT(0xc7252a0c31d8eUL == fixedpoint_whole_part(sum));
  ASSERT(0x5be47e8ea0538c50UL == fixedpoint_frac_part(sum));

  lhs = fixedpoint_create_from_hex("8bd.0e34492025065");
  rhs = fixedpoint_create_from_hex("5d7b061d6.034f5d");
  sum = fixedpoint_add(lhs, rhs);
  ASSERT(!fixedpoint_is_neg(sum));
  ASSERT(0x5d7b06a93UL == fixedpoint_whole_part(sum));
  ASSERT(0x1183a62025065000UL == fixedpoint_frac_part(sum));
  
  lhs = fixedpoint_create_from_hex("-8a6a9f92d72.82a9b99ad4e76");
  rhs = fixedpoint_create_from_hex("-8a93a62c25996.e09875");
  sum = fixedpoint_add(lhs, rhs);
  //ASSERT(fixedpoint_is_neg(sum));
  ASSERT(0x8b1e10cbb8709UL == fixedpoint_whole_part(sum));
  ASSERT(0x63422e9ad4e76000UL == fixedpoint_frac_part(sum));

  lhs = objs->max;
  rhs = objs->max;
  sum = fixedpoint_add(lhs, rhs);
  ASSERT(fixedpoint_is_overflow_pos(sum));

  lhs = objs->max;
  lhs.t = valid_negative;
  rhs = objs->max;
  rhs.t = valid_negative;
  sum = fixedpoint_add(lhs, rhs);
  ASSERT(fixedpoint_is_overflow_neg(sum));
  lhs.t = valid_positive;
  rhs.t = valid_positive;

  lhs = objs->zero;
  rhs = objs->zero;
  sum = fixedpoint_add(lhs, rhs);
  ASSERT(fixedpoint_is_zero(sum));
  ASSERT(fixedpoint_is_valid(sum));

}

void test_sub(TestObjs *objs) {
  (void) objs;

  Fixedpoint lhs, rhs, diff;

  lhs = fixedpoint_create_from_hex("-ccf35aa3a04a3b.b105");
  rhs = fixedpoint_create_from_hex("f676e8.58");
  diff = fixedpoint_sub(lhs, rhs);
  ASSERT(fixedpoint_is_neg(diff));
  ASSERT(0xccf35aa496c124UL == fixedpoint_whole_part(diff));
  ASSERT(0x0905000000000000UL == fixedpoint_frac_part(diff));

  memset((void*) &lhs, 0, sizeof(lhs));
  memset((void*) &rhs, 0, sizeof(rhs));
  memset((void*) &diff, 0, sizeof(diff));
  lhs = fixedpoint_create_from_hex("-126a0b9b1f810.201cfa1");
  rhs = fixedpoint_create_from_hex("-7480fe1b4.07a5");

  printf("rwhole: %x rfrac: %x", rhs.w, rhs.f);
  printf("lwhole: %x lfrac: %x", lhs.w, lhs.f);

  diff = fixedpoint_sub(lhs, rhs);
  ASSERT(fixedpoint_is_neg(diff));
  ASSERT(0x1269971a2165cUL == fixedpoint_whole_part(diff));
  printf("%x", fixedpoint_frac_part(diff));

  Fixedpoint val1 = fixedpoint_create_from_hex("-126a0b9b1f810.201cfa1");
  ASSERT(fixedpoint_is_valid(val1));
  ASSERT(0x126a0b9b1f810UL == fixedpoint_whole_part(val1));
  ASSERT(0x201cfa1UL == fixedpoint_frac_part(val1));
  ASSERT(0x1780ce536UL == fixedpoint_whole_part(diff));

  ASSERT(0x1877fa1UL == fixedpoint_frac_part(diff));
  
  lhs = fixedpoint_create_from_hex("16fbf0ce.977a8");
  rhs = fixedpoint_create_from_hex("18f08d605.105c9b0f3fc9950");
  diff = fixedpoint_sub(lhs, rhs);
  ASSERT(fixedpoint_is_neg(diff));

  


}

void test_is_overflow_pos(TestObjs *objs) {
  Fixedpoint sum;

  sum = fixedpoint_add(objs->max, objs->one);
  ASSERT(fixedpoint_is_overflow_pos(sum));

  sum = fixedpoint_add(objs->one, objs->max);
  ASSERT(fixedpoint_is_overflow_pos(sum));

  Fixedpoint negative_one = fixedpoint_negate(objs->one);

  sum = fixedpoint_sub(objs->max, negative_one);
  ASSERT(fixedpoint_is_overflow_pos(sum));
}

void test_is_err(TestObjs *objs) {
  (void) objs;

  // too many characters
  Fixedpoint err1 = fixedpoint_create_from_hex("88888888888888889.6666666666666666");
  ASSERT(fixedpoint_is_err(err1));

  // too many characters
  Fixedpoint err2 = fixedpoint_create_from_hex("6666666666666666.88888888888888889");
  ASSERT(fixedpoint_is_err(err2));

  // this one is actually fine
  Fixedpoint err3 = fixedpoint_create_from_hex("-6666666666666666.8888888888888888");
  ASSERT(fixedpoint_is_valid(err3));
  ASSERT(!fixedpoint_is_err(err3));

  // whole part is too large
  Fixedpoint err4 = fixedpoint_create_from_hex("88888888888888889");
  ASSERT(fixedpoint_is_err(err4));

  // fractional part is too large
  Fixedpoint err5 = fixedpoint_create_from_hex("7.88888888888888889");
  ASSERT(fixedpoint_is_err(err5));

  // invalid hex digits in whole part
  Fixedpoint err6 = fixedpoint_create_from_hex("123xabc.4");
  ASSERT(fixedpoint_is_err(err6));

  // invalid hex digits in fractional part
  Fixedpoint err7 = fixedpoint_create_from_hex("7.0?4");
  ASSERT(fixedpoint_is_err(err7));
}

void test_is_zero(TestObjs *objs) {
  Fixedpoint z;
  z = objs->zero;
  ASSERT(fixedpoint_is_zero(z));
}

// TODO: implement more test functions

