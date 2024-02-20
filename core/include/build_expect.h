#if !defined(__GNUC__) || __GNUC__ < 3
#define __builtin_expect(x, expected_value) (x)
#endif

#define build_likely(x) __builtin_expect(!!(x), 1)
#define build_unlikely(x) __builtin_expect(!!(x), 0)