#include <unity.h>

#include "mwa/ProfileInput.h"

void test_username_is_normalized() {
  const auto result = mwa::normalizeProfileInput(" @WikiZell ");
  TEST_ASSERT_TRUE(result.valid);
  TEST_ASSERT_EQUAL_STRING("WikiZell", result.username.c_str());
}

void test_public_url_is_normalized() {
  const auto result = mwa::normalizeProfileInput("https://makerworld.com/en/@WikiZell?tab=home");
  TEST_ASSERT_TRUE(result.valid);
  TEST_ASSERT_EQUAL_STRING("WikiZell", result.username.c_str());
}

void test_invalid_input_is_rejected() {
  TEST_ASSERT_FALSE(mwa::normalizeProfileInput("not a valid user").valid);
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_username_is_normalized);
  RUN_TEST(test_public_url_is_normalized);
  RUN_TEST(test_invalid_input_is_rejected);
  return UNITY_END();
}
