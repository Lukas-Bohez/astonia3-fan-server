It is completely normal to look at a repository like this and have no idea where to begin. Astonia 3 is a classic MMORPG from 2001, and its open-source C codebase is notorious for being a "wild west" of early 2000s programming: unencrypted passwords, ancient build tools, and spaghetti code. It's a massive, daunting beast.

This document now contains a practical, prioritized checklist with high-impact safety and modernization tasks. Use it as the source of truth for planning sprints and pull requests.

---

# 🗡️ Astonia 3 Fan Server: Modernization & Security Roadmap

## 0) Trust-but-verify pre-work
- [ ] Clone repository cleanly and confirm `main` is canonical (`git branch -a` returns `main`, no `master`).
- [ ] Take a snapshot with `git tag pre-modernization` and push it for safe rollback.
- [ ] Identify what constitutes the production release branch and lock it with branch protection (GitHub settings).

## 1) Phase 1: Make build portable and repeatable
1. Containerized environment (required)
   - [ ] Add `Dockerfile` for Linux build + runtime.
   - [ ] Add `docker-compose.yml` with:
     - `mysql`/`mariadb` service, secure-default user and password as env.
     - `astonia` service mapped to 4000/4001 as needed.
   - [ ] Add `./scripts/docker-run.sh` to set up working database schema (`storage.sql`, `merc.sql`, `create_tables.sql`).
2. Modern toolchain
   - [ ] Add `CMakeLists.txt` at root and for each sub-binary (`server`, `client`, `launcher`) so now cross-platform targets are supported.
   - [ ] Keep original Makefile and Makefile.win as legacy compatibility, but prefer CMake for all new CI and local builds.
3. Build testing
   - [ ] Create a local sanity Nix/shell/WSL dev script for one-command `./scripts/build.sh` and `./scripts/test.sh`.
   - [ ] Ensure `clang-tidy`/`cppcheck` run against the code with no immediate crash conditions.
   - [ ] Add `CI` status badge to `README.md`.
4. Portability hardening
   - [ ] Replace deprecated `strcpy`/`sprintf` uses with safe counterparts and `asprintf`-style logic.
   - [ ] Add `-Werror`, `-Wall`, `-Wextra`, `-Wshadow`, `-Wcast-qual`, `-Wformat-security` to compile flags in CMake.
   - [ ] Replace `int` with fixed width types in data structures that map to protocol bytes/DB fields (`short`/`long` auditing).

## 2) Phase 2: Critical security remediation (blocker)
1. Credentials & secrets
   - [ ] Identify every hardcoded DB credential, salt, secret in source.
   - [ ] Move to `config/server.conf` + `dotenv` loader.
   - [ ] Add credential template: `config/server.conf.example` (exclude real secrets).
2. Authentication storage
   - [ ] Replace plain-text user password pipeline:
     - registration in `create_account.c` / `player` create path builds secure hash.
     - login path in `account` checks hash only.
     - module: `auth/crypto.c` with `argon2` + PHC string format.
   - [ ] Add migration worker to convert existing DB passwords safely.
3. SQL injection prevention
   - [ ] Inventory all SQL query construction sites (`database.c`, `query.*`).
   - [ ] Replace raw concatenation with prepared statements using MariaDB C API or abstraction layer.
   - [ ] Add query-ready test harness in `tests/test_database.c`.
4. Network security
   - [ ] Add optional TLS wrapper on socket listens (OpenSSL or mbedtls) for “secure” mode.
   - [ ] Add optional packet-level MACing using HMAC-SHA256 to detect tampering.
   - [ ] Add token/session expiration, rate-limits for repeated login attempts.

## 3) Phase 3: Runtime & gameplay safety (critical)
1. Game server authority
   - [ ] Audit packet opcode handlers (`read`, `write`, `handle_` routes) in `server.c` and `player_driver.c`.
   - [ ] Add invariants in sections where client-provided numeric values are used (e.g., damage, coordinates, inventory indices).
   - [ ] Enforce bounds and clamp user options.
2. Memory, concurrency, misuse
   - [ ] Add `valgrind` and `ASan` test configuration; run full scenario for 30+ minutes into a script.
   - [ ] Convert raw `malloc`/`free` ownership to helper wrappers collecting source site for leaks.
   - [ ] Add `assert` and `debug` flags for pointer validity around linked list/buffer operations.
3. Logging & observability
   - [ ] Replace `printf` ad-hoc logging with structured logger (e.g., `spdlog` style in C or incumbent `zlog`).
   - [ ] Add 3 levels: error, warning, info; dynamically adjustable by config.
   - [ ] Add per message include: timestamp, thread, player ID context.
   - [ ] Add rotating log file to avoid massive disk growth (`logrotate` support or internal rotation).

## 4) Phase 4: Data and persistence hardening (should ship)
1. Schema cleanup
   - [ ] Normalize/rename schema fields in SQL definitions.
   - [ ] Add FK constraints and indexes (e.g., `player_id`, `zone_id`, `item_id`) to improve query reliability.
   - [ ] Add CASCADE/ON UPDATE semantics only where safe.
2. Upgrades & migrations
   - [ ] Add migration script folder `db/migrations` and basic versioning table `schema_migrations`.
   - [ ] Add CLI `./tools/db-migrate.sh` for stepwise schema evolution.
3. Assets and content
   - [ ] Move bulk zone and item data to a `content/` directory.
   - [ ] Add script to verify checksums of zone files and warn on corruption.

## 5) Phase 5: Developer experience & CI (high ROI)
- [ ] Add GitHub Actions `.github/workflows/ci.yml` with:
  - `lint` (clang-tidy/cppcheck), `build`, `unit-tests`, `integration-test (small server + client bot)`.
  - matrix for at least Ubuntu + Windows (via msys2).
- [ ] Add `release` workflow to produce tarball with versioned prefix.
- [ ] Add `CONTRIBUTING.md` with branch model (main protected, PR policy, code review tags).

---

## Quick wins (this sprint)
1. Add `README.md` section: “Modernization plan in `todo.md`.”
2. Implement `CMakeLists.txt` and enable `-Werror`.
3. Add `config/server.conf.example` and switch current config path accordingly.
4. Add minimal `scripts/db-init.sh` (creates database and initial tables) so `docker-compose up` can Start.

---

## Optional: Ask AI to generate scaffolding
When you are ready, I can generate complete text for:
- `Dockerfile` + `docker-compose.yml`
- `CMakeLists.txt` + `scripts/build.sh`
- `config/server.conf.example` + `config loader`
- `github actions CI workflow`

---

> Tip: Track progress in Git by small milestone commits, e.g. `phase1/container` → `phase1/cmake` → `phase2/auth`. This avoids getting lost in the refactor mess.