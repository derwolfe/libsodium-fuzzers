#include <assert.h>
#include <sodium.h>
#include <cstring>
#include <string>

#include "fake_random.h"

int is_subarray(const unsigned char *needle, const size_t needle_size,
                const unsigned char *haystack, const size_t haystack_size) {
  // haystack must be bigger than needle.
  if (haystack_size < needle_size) {
    return -1;
  }

  size_t ct;
  int check;

  for (ct = 0; ct < haystack_size; ct++) {
    // advance through the haystack.
    const size_t new_haystack_size = sizeof(haystack) - ct;

    // no more checks to check. Full key cannot be present in a smaller array.
    if (new_haystack_size < needle_size) {
      break;
    }

    check = std::memcmp(haystack + ct, needle, new_haystack_size);
    if (check == 0) {
      continue;
    } else {
      return 1;
    }
  }
  return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const unsigned char *data, size_t size) {
  int initialized = sodium_init();
  assert(initialized >= 0);

  setup_fake_random(data, size);

  unsigned char key[crypto_auth_KEYBYTES];
  unsigned char mac[crypto_auth_BYTES];

  sodium_memzero(mac, crypto_auth_BYTES);
  unsigned char mac_check[crypto_auth_BYTES];

  std::memcpy(mac_check, &mac, crypto_auth_BYTES);

  crypto_auth_keygen(key);

  // auth should never fail for a key generated by crypto_auth_keygen
  int auth_check = crypto_auth(mac, data, size, key);
  assert(auth_check == 0);

  // the mac should change from its default to a new value after auth is called.
  int mac_changed = std::memcmp(mac, mac_check, crypto_auth_BYTES);
  assert(mac_changed != 0);

  // verification should not fail if the mac and key are successfully generated
  // by sodium
  int verify_check = crypto_auth_verify(mac, data, size, key);
  assert(verify_check == 0);

  // the key should not be leaked into the mac buffer
  int leaked = is_subarray(key, crypto_auth_KEYBYTES, mac, crypto_auth_BYTES);
  assert(leaked == 0);

  return 0;
}
