/*
 * Part of Astonia Server 3.5 (c) Daniel Brockhaus. Please read license.txt.
 *
 * Primary: use libargon2 if available. Fallback: old placeholder to preserve compatibility.
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "argon.h"

#if defined(USE_ARGON2)
#include <argon2.h>
#include <stdlib.h>

static int build_peppered_password(const char *password, const char *pepper, char *out, size_t out_len) {
    if (!out || !password) return -1;
    size_t pw_len = strlen(password);
    size_t pepper_len = (pepper ? strlen(pepper) : 0);
    size_t total = pw_len + (pepper_len ? pepper_len + 1 : 0);

    if (total + 1 > out_len) return -1;
    memcpy(out, password, pw_len);
    if (pepper_len) {
        out[pw_len] = ':';
        memcpy(out + pw_len + 1, pepper, pepper_len);
    }
    out[total] = '\0';
    return 0;
}

static int random_salt(uint8_t *salt, size_t salt_len) {
#ifdef _WIN32
    if (!RtlGenRandom(salt, salt_len)) return -1;
    return 0;
#else
    FILE *f = fopen("/dev/urandom", "rb");
    if (!f) return -1;
    size_t r = fread(salt, 1, salt_len, f);
    fclose(f);
    return (r == salt_len) ? 0 : -1;
#endif
}
#endif

int argon2id_hash_password(char *out_encoded, size_t out_encoded_len, const char *password, const char *pepper_optional) {
    if (!out_encoded || !password) return -1;

#if defined(USE_ARGON2)
    char pwdbuf[1024];
    if (build_peppered_password(password, pepper_optional, pwdbuf, sizeof(pwdbuf)) != 0) return -1;

    const uint32_t t_cost = 2;
    const uint32_t m_cost = 1 << 16;
    const uint32_t parallelism = 1;
    const size_t salt_len = 16;
    const size_t hash_len = 32;

    uint8_t salt[salt_len];
    if (random_salt(salt, salt_len) != 0) {
        return -1;
    }

    int rc = argon2id_hash_encoded(t_cost, m_cost, parallelism,
                                   pwdbuf, strlen(pwdbuf),
                                   salt, salt_len,
                                   hash_len,
                                   out_encoded, out_encoded_len);

    volatile char *clear = pwdbuf;
    for (size_t i = 0; i < sizeof(pwdbuf); i++) clear[i] = 0;

    return (rc == ARGON2_OK) ? 0 : -1;
#else
    if (pepper_optional && *pepper_optional) {
        return snprintf(out_encoded, out_encoded_len, "P:%s:%s", password, pepper_optional) >= (int)out_encoded_len ? -1 : 0;
    }
    return snprintf(out_encoded, out_encoded_len, "P:%s", password) >= (int)out_encoded_len ? -1 : 0;
#endif
}

int argon2id_verify_password(const char *stored_encoded, const char *password, const char *pepper_optional) {
    if (!stored_encoded || !password) return -1;

#if defined(USE_ARGON2)
    char pwdbuf[1024];
    if (build_peppered_password(password, pepper_optional, pwdbuf, sizeof(pwdbuf)) != 0) return -1;

    int rc = argon2id_verify(stored_encoded, pwdbuf, strlen(pwdbuf));

    volatile char *clear = pwdbuf;
    for (size_t i = 0; i < sizeof(pwdbuf); i++) clear[i] = 0;

    if (rc == ARGON2_OK) return 1;
    if (rc == ARGON2_VERIFY_MISMATCH) return 0;
    return -1;
#else
    if (stored_encoded[0] != 'P' || stored_encoded[1] != ':') return -1;

    if (pepper_optional && *pepper_optional) {
        char expected[512];
        int ret = snprintf(expected, sizeof(expected), "P:%s:%s", password, pepper_optional);
        if (ret < 0 || ret >= (int)sizeof(expected)) return -1;
        return strcmp(stored_encoded, expected) == 0 ? 1 : 0;
    }

    char expected[512];
    int ret = snprintf(expected, sizeof(expected), "P:%s", password);
    if (ret < 0 || ret >= (int)sizeof(expected)) return -1;
    return strcmp(stored_encoded, expected) == 0 ? 1 : 0;
#endif
}

#if 0
// -------------------------
// Public API
// -------------------------

// Returns 0 on success, -1 on failure.
// out_encoded should be stored in DB (VARCHAR(255) recommended).
int argon2id_hash_password(char *out_encoded, size_t out_encoded_len, const char *password, const char *pepper_optional) {
    if (!out_encoded || out_encoded_len == 0 || !password) return -1;

    // --- Parameters (tune these) ---
    // Start conservative; increase later to hit a target time (e.g. 50-200ms).
    const uint32_t t_cost = 2; // iterations
    const uint32_t m_cost_kib = 19 * 1024; // memory in KiB (19 MiB baseline-ish)
    const uint32_t parallelism = 1; // lanes/threads
    const size_t salt_len = 16;
    const size_t hash_len = 32;

    uint8_t salt[16];
    if (csprng_bytes(salt, salt_len) != 0) {
        return -1;
    }

    // Optionally pepper the password (if you don’t want pepper, pass NULL).
    // Note: This buffer should be large enough for password + ":" + pepper.
    // Adjust if you allow very long passwords/pepper.
    char pwbuf[512];
    build_peppered_password(password, pepper_optional, pwbuf, sizeof(pwbuf));

    // Compute the required encoded length and ensure caller buffer is big enough.
    size_t need = (size_t)argon2_encodedlen(
        t_cost, m_cost_kib, parallelism,
        (uint32_t)salt_len, (uint32_t)hash_len,
        Argon2_id);
    if (need + 1 > out_encoded_len) { // +1 just to be safe for '\0'
        // Caller buffer too small
        return -1;
    }

    int rc = argon2id_hash_encoded(
        t_cost, m_cost_kib, parallelism,
        pwbuf, strlen(pwbuf),
        salt, salt_len,
        hash_len,
        out_encoded, out_encoded_len);

    // Clear pwbuf from memory (best-effort)
    memset(pwbuf, 0, sizeof(pwbuf));

    if (rc != ARGON2_OK) {
        return -1;
    }
    return 0;
}

// Returns 1 if correct, 0 if incorrect, -1 on error.
int argon2id_verify_password(const char *stored_encoded, const char *password, const char *pepper_optional) {
    if (!stored_encoded || !password) return -1;

    char pwbuf[512];
    build_peppered_password(password, pepper_optional, pwbuf, sizeof(pwbuf));

    int rc = argon2id_verify(stored_encoded, pwbuf, strlen(pwbuf));

    memset(pwbuf, 0, sizeof(pwbuf));

    if (rc == ARGON2_OK) return 1;
    if (rc == ARGON2_VERIFY_MISMATCH) return 0;

    // Other errors: malformed hash string, out of memory, etc.
    return -1;
}
#endif
