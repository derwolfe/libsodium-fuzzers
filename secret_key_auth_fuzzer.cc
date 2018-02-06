#include <assert.h>
#include <sodium.h>
#include <algorithm>
#include <cstring>
#include <string>

#include "fake_random.h"

int is_subarray(const unsigned char * haystack, const size_t haystack_size,
                 const unsigned char * need, const size_t needle_size) {
  if (haystack_size < needle_size) {
    return -1;
  }

  // iterate over the haystack, memcmp-ing each time. What about buffer over reads?
  size_t ct = 0;
  for (ct; ct++, ct < haystack_size) {

  }
}

extern "C" int LLVMFuzzerTestOneInput(const unsigned char *data, size_t size) {
  int initialized = sodium_init();
  assert(initialized >= 0);

  setup_fake_random(data, size);

  unsigned char key[crypto_auth_KEYBYTES];
  unsigned char mac[crypto_auth_BYTES];

  unsigned char mac_check[crypto_auth_BYTES];

  std::memcpy(mac_check, &mac, crypto_auth_BYTES);

  crypto_auth_keygen(key);
  crypto_auth(mac, data, size, key);

  // the mac should change from its default to a new value after auth is called.
  int mac_changed = std::memcmp(mac, mac_check, crypto_auth_BYTES);
  assert(mac_changed != 0);

  // verification should not fail if the mac is generated by sodium.
  int check = crypto_auth_verify(mac, data, size, key);
  assert(check == 0);

  // none of the key material should be present in the mac
  auto key_end = std::end(key);
  // unsigned char *offset = std::search(std::begin(mac), std::end(mac),
  //                                     std::begin(key), std::end(key));
  // if (offset != std::end(key)) {
  // }
  return 0;
}
