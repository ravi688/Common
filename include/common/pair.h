#pragma once

#define typedef_pair_t(T1, T2) struct __pair_##T1##_##T2##_t { T1 first; T2 second; }
#define pair_t(T1, T2) struct __pair_##T1##_##T2##_t
#define make_pair_t(T1, T2) (pair_t(T1, T2))

#define typedef_tuple2_t(T1, T2) struct __tuple_##T1##_##T2##_t { T1 first; T2 second; }
#define tuple2_t(T1, T2) struct __tuple_##T1##_##T2##_t
#define make_tuple2(T1, T2) (tuple2_t(T1, T2))

#define typedef_tuple3_t(T1, T2, T3) struct __tuple_##T1##_##T2##_##T3##_t { T1 first; T2 second; T3 third; }
#define tuple3_t(T1, T2, T3) struct __tuple_##T1##_##T2##_##T3##_t
#define make_tuple3(T1, T2, T3) (tuple3_t(T1, T2))
