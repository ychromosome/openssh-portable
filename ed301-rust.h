/* Deterministic public core operations; implementation in ed301-ssh-capi. */
#ifndef ED301_RUST_H
#define ED301_RUST_H
#include <stddef.h>
int ed301_ssh_seed_public(unsigned char *, const unsigned char *, size_t);
int ed301_ssh_sign_context(unsigned char *, const unsigned char *, size_t,
    const unsigned char *, size_t, const unsigned char *, size_t);
int ed301_ssh_verify_context(const unsigned char *, size_t,
    const unsigned char *, size_t, const unsigned char *, size_t,
    const unsigned char *, size_t);
int ed301_ssh_validate_public(const unsigned char *, size_t);
int ed301_ssh_x_public(unsigned char *, const unsigned char *, size_t);
int ed301_ssh_x_shared(unsigned char *, const unsigned char *, size_t,
    const unsigned char *, size_t);
int ed301_ssh_x_is_weak(const unsigned char *, size_t);
#endif /* ED301_RUST_H */
