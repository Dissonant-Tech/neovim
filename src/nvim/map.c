#include <stdlib.h>
#include <stdbool.h>

#include "nvim/map.h"
#include "nvim/map_defs.h"
#include "nvim/vim.h"
#include "nvim/memory.h"

#include "nvim/lib/khash.h"

#define cstr_t_hash kh_str_hash_func
#define cstr_t_eq kh_str_hash_equal
#define uint64_t_hash kh_int64_hash_func
#define uint64_t_eq kh_int64_hash_equal
#define uint32_t_hash kh_int_hash_func
#define uint32_t_eq kh_int_hash_equal

#if defined(ARCH_64)
#define ptr_t_hash(key) uint64_t_hash((uint64_t)key)
#define ptr_t_eq(a, b) uint64_t_eq((uint64_t)a, (uint64_t)b)
#elif defined(ARCH_32)
#define ptr_t_hash(key) uint32_t_hash((uint32_t)key)
#define ptr_t_eq(a, b) uint32_t_eq((uint32_t)a, (uint32_t)b)
#endif

#define INITIALIZER(T, U) T##_##U##_initializer
#define INITIALIZER_DECLARE(T, U, ...) const U INITIALIZER(T, U) = __VA_ARGS__
#define DEFAULT_INITIALIZER {0}

#define MAP_IMPL(T, U, ...)                                                   \
  INITIALIZER_DECLARE(T, U, __VA_ARGS__);                                     \
  __KHASH_IMPL(T##_##U##_map,, T, U, 1, T##_hash, T##_eq)                     \
                                                                              \
  Map(T, U) *map_##T##_##U##_new()                                            \
  {                                                                           \
    Map(T, U) *rv = xmalloc(sizeof(Map(T, U)));                               \
    rv->table = kh_init(T##_##U##_map);                                       \
    return rv;                                                                \
  }                                                                           \
                                                                              \
  void map_##T##_##U##_free(Map(T, U) *map)                                   \
  {                                                                           \
    kh_destroy(T##_##U##_map, map->table);                                    \
    free(map);                                                                \
  }                                                                           \
                                                                              \
  U map_##T##_##U##_get(Map(T, U) *map, T key)                                \
  {                                                                           \
    khiter_t k;                                                               \
                                                                              \
    if ((k = kh_get(T##_##U##_map, map->table, key)) == kh_end(map->table)) { \
      return INITIALIZER(T, U);                                               \
    }                                                                         \
                                                                              \
    return kh_val(map->table, k);                                             \
  }                                                                           \
                                                                              \
  bool map_##T##_##U##_has(Map(T, U) *map, T key)                             \
  {                                                                           \
    return kh_get(T##_##U##_map, map->table, key) != kh_end(map->table);      \
  }                                                                           \
                                                                              \
  U map_##T##_##U##_put(Map(T, U) *map, T key, U value)                       \
  {                                                                           \
    int ret;                                                                  \
    U rv = INITIALIZER(T, U);                                                 \
    khiter_t k = kh_put(T##_##U##_map, map->table, key, &ret);                \
                                                                              \
    if (!ret) {                                                               \
      rv = kh_val(map->table, k);                                             \
    }                                                                         \
                                                                              \
    kh_val(map->table, k) = value;                                            \
    return rv;                                                                \
  }                                                                           \
                                                                              \
  U map_##T##_##U##_del(Map(T, U) *map, T key)                                \
  {                                                                           \
    U rv = INITIALIZER(T, U);                                                 \
    khiter_t k;                                                               \
                                                                              \
    if ((k = kh_get(T##_##U##_map, map->table, key)) != kh_end(map->table)) { \
      rv = kh_val(map->table, k);                                             \
      kh_del(T##_##U##_map, map->table, k);                                   \
    }                                                                         \
                                                                              \
    return rv;                                                                \
  }

MAP_IMPL(cstr_t, ptr_t, DEFAULT_INITIALIZER)
MAP_IMPL(ptr_t, ptr_t, DEFAULT_INITIALIZER)
MAP_IMPL(uint64_t, ptr_t, DEFAULT_INITIALIZER)
