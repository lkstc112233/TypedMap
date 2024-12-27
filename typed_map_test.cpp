
#include "typed_map.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"


namespace utils::map {
namespace {

using ::testing::ElementsAre;
using ::testing::Eq;
using ::testing::IsEmpty;
using ::testing::Optional;
using ::testing::Pair;

enum TestEnum {
  TEST_ENUM_VALUE_1,
  TEST_ENUM_VALUE_2,
  TEST_ENUM_VALUE_3,
  TEST_ENUM_VALUE_4,
  TEST_ENUM_VALUE_5,
  TEST_ENUM_VALUE_6,
};

struct Constraints : public ConstraintsTemplate<TestEnum> {
  template <TestEnum T, typename E = void>
  struct TypeOf {
    using type = void;
  };
  template <TestEnum T>
  struct TypeOf<T, typename std::enable_if_t<T == TEST_ENUM_VALUE_1>> {
    using type = char;
  };
  template <TestEnum T>
  struct TypeOf<T, typename std::enable_if_t<T == TEST_ENUM_VALUE_2>> {
    using type = int;
  };
  template <TestEnum T>
  struct TypeOf<T, typename std::enable_if_t<T == TEST_ENUM_VALUE_3>> {
    using type = bool;
  };
  template <TestEnum T>
  struct TypeOf<T, typename std::enable_if_t<T == TEST_ENUM_VALUE_4>> {
    using type = std::vector<int>;
  };
  template <TestEnum T>
  struct TypeOf<T, typename std::enable_if_t<T == TEST_ENUM_VALUE_5>> {
    using type = std::map<int, int>;
  };
  template <TestEnum T>
  struct TypeOf<T, typename std::enable_if_t<T == TEST_ENUM_VALUE_6>> {
    using type = std::set<int>;
  };
};

class StrongTypeMapTest : public ::testing::Test {
 protected:
  TypedMap<Constraints, int, char, bool, std::vector<int>, std::map<int, int>, std::set<int>> map_under_test_;
};

TEST_F(StrongTypeMapTest, GetWorksOnEmptyValue) {
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_1>(), Eq(std::nullopt));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_2>(), Eq(std::nullopt));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_3>(), Eq(std::nullopt));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_4>(), IsEmpty());
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_5>(), IsEmpty());
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_6>(), IsEmpty());
}

TEST_F(StrongTypeMapTest, GetAndSetWorks) {
  map_under_test_.Set<TEST_ENUM_VALUE_1>('a');
  map_under_test_.Set<TEST_ENUM_VALUE_2>(2);
  map_under_test_.Set<TEST_ENUM_VALUE_3>(false);
  map_under_test_.Set<TEST_ENUM_VALUE_4>(std::vector<int>{1, 2});
  map_under_test_.Set<TEST_ENUM_VALUE_5>(std::map<int, int>{{1, 2}});
  map_under_test_.Set<TEST_ENUM_VALUE_6>(std::set<int>{1, 2, 3});

  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_1>(), Optional(Eq('a')));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_2>(), Optional(Eq(2)));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_3>(), Optional(Eq(false)));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_4>(), ElementsAre(1, 2));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_5>(), ElementsAre(Pair(1, 2)));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_6>(), ElementsAre(1, 2, 3));
}

TEST_F(StrongTypeMapTest, UnsetWorks) {
  map_under_test_.Set<TEST_ENUM_VALUE_1>('a');
  map_under_test_.Set<TEST_ENUM_VALUE_2>(2);
  map_under_test_.Set<TEST_ENUM_VALUE_3>(false);
  map_under_test_.Set<TEST_ENUM_VALUE_4>(std::vector<int>{1, 2});
  map_under_test_.Set<TEST_ENUM_VALUE_5>(std::map<int, int>{{1, 2}});
  map_under_test_.Set<TEST_ENUM_VALUE_6>(std::set<int>{1, 2, 3});

  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_1>(), Optional(Eq('a')));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_2>(), Optional(Eq(2)));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_3>(), Optional(Eq(false)));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_4>(), ElementsAre(1, 2));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_5>(), ElementsAre(Pair(1, 2)));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_6>(), ElementsAre(1, 2, 3));

  map_under_test_.Unset(TEST_ENUM_VALUE_1);
  map_under_test_.Unset(TEST_ENUM_VALUE_3);
  map_under_test_.Unset(TEST_ENUM_VALUE_5);

  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_1>(), Eq(std::nullopt));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_2>(), Optional(Eq(2)));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_3>(), Eq(std::nullopt));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_4>(), ElementsAre(1, 2));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_5>(), IsEmpty());
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_6>(), ElementsAre(1, 2, 3));
}

TEST_F(StrongTypeMapTest, KeySetWorks) {
  map_under_test_.Set<TEST_ENUM_VALUE_1>('a');
  map_under_test_.Set<TEST_ENUM_VALUE_2>(2);
  map_under_test_.Set<TEST_ENUM_VALUE_3>(false);
  map_under_test_.Set<TEST_ENUM_VALUE_4>(std::vector<int>{1, 2});
  map_under_test_.Set<TEST_ENUM_VALUE_5>(std::map<int, int>{{1, 2}});
  map_under_test_.Set<TEST_ENUM_VALUE_6>(std::set<int>{1, 2, 3});

  ASSERT_THAT(map_under_test_.KeySet(), ElementsAre(TEST_ENUM_VALUE_1, TEST_ENUM_VALUE_2, TEST_ENUM_VALUE_3,
                                                    TEST_ENUM_VALUE_4, TEST_ENUM_VALUE_5, TEST_ENUM_VALUE_6));

  map_under_test_.Unset(TEST_ENUM_VALUE_1);
  map_under_test_.Unset(TEST_ENUM_VALUE_3);
  map_under_test_.Unset(TEST_ENUM_VALUE_5);

  ASSERT_THAT(map_under_test_.KeySet(), ElementsAre(TEST_ENUM_VALUE_2, TEST_ENUM_VALUE_4, TEST_ENUM_VALUE_6));
}

TEST_F(StrongTypeMapTest, MergeWorks) {
  map_under_test_.Set<TEST_ENUM_VALUE_1>('a');
  map_under_test_.Set<TEST_ENUM_VALUE_2>(2);
  map_under_test_.Set<TEST_ENUM_VALUE_3>(false);
  map_under_test_.Set<TEST_ENUM_VALUE_4>(std::vector<int>{1, 2});

  TypedMap<Constraints, int, char, bool, std::vector<int>, std::map<int, int>, std::set<int>> another_map;

  another_map.Set<TEST_ENUM_VALUE_3>(true);
  another_map.Set<TEST_ENUM_VALUE_4>(std::vector<int>{5, 6});
  another_map.Set<TEST_ENUM_VALUE_5>(std::map<int, int>{{1, 2}});
  another_map.Set<TEST_ENUM_VALUE_6>(std::set<int>{1, 2, 3});

  map_under_test_.Merge(another_map);

  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_1>(), Optional(Eq('a')));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_2>(), Optional(Eq(2)));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_3>(), Optional(Eq(true)));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_4>(), ElementsAre(5, 6));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_5>(), ElementsAre(Pair(1, 2)));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_6>(), ElementsAre(1, 2, 3));
}

TEST_F(StrongTypeMapTest, MutableWorks) {
  map_under_test_.Mutable<TEST_ENUM_VALUE_1>() = 'a';
  map_under_test_.Mutable<TEST_ENUM_VALUE_2>() = 2;
  map_under_test_.Mutable<TEST_ENUM_VALUE_3>() = false;
  map_under_test_.Mutable<TEST_ENUM_VALUE_4>().push_back(1);
  map_under_test_.Mutable<TEST_ENUM_VALUE_5>().insert(std::make_pair(1, 2));
  map_under_test_.Mutable<TEST_ENUM_VALUE_6>().insert(2);

  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_1>(), Optional(Eq('a')));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_2>(), Optional(Eq(2)));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_3>(), Optional(Eq(false)));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_4>(), ElementsAre(1));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_5>(), ElementsAre(Pair(1, 2)));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_6>(), ElementsAre(2));
}

TEST_F(StrongTypeMapTest, GetAndSetWorksForMap) {
  map_under_test_.Set<TEST_ENUM_VALUE_5>(1, 2);
  map_under_test_.Set<TEST_ENUM_VALUE_5>(2, 3);

  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_5>(), ElementsAre(Pair(1, 2), Pair(2, 3)));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_5>(1), Optional(Eq(2)));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_5>(2), Optional(Eq(3)));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_5>(3), Eq(std::nullopt));

  map_under_test_.Set<TEST_ENUM_VALUE_5>(1, 5);

  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_5>(), ElementsAre(Pair(1, 5), Pair(2, 3)));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_5>(1), Optional(Eq(5)));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_5>(2), Optional(Eq(3)));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_5>(3), Eq(std::nullopt));
  map_under_test_.Set<TEST_ENUM_VALUE_5>(3, 7);

  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_5>(), ElementsAre(Pair(1, 5), Pair(2, 3), Pair(3, 7)));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_5>(1), Optional(Eq(5)));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_5>(2), Optional(Eq(3)));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_5>(3), Optional(Eq(7)));
}

TEST_F(StrongTypeMapTest, AddWorks) {
  map_under_test_.Add<TEST_ENUM_VALUE_4>(2);
  map_under_test_.Add<TEST_ENUM_VALUE_4>(1);
  map_under_test_.Add<TEST_ENUM_VALUE_4>(3);
  map_under_test_.Add<TEST_ENUM_VALUE_6>(2);
  map_under_test_.Add<TEST_ENUM_VALUE_6>(1);
  map_under_test_.Add<TEST_ENUM_VALUE_6>(3);

  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_4>(), ElementsAre(2, 1, 3));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_6>(), ElementsAre(1, 2, 3));
}

TEST_F(StrongTypeMapTest, GetRawAsWorks) {
  map_under_test_.SetRaw(TEST_ENUM_VALUE_1, 'a');
  map_under_test_.SetRaw(TEST_ENUM_VALUE_2, 2);
  map_under_test_.SetRaw(TEST_ENUM_VALUE_3, false);
  map_under_test_.SetRaw(TEST_ENUM_VALUE_4, std::vector<int>{1, 2});
  map_under_test_.SetRaw(TEST_ENUM_VALUE_5, std::map<int, int>{{1, 2}});
  map_under_test_.SetRaw(TEST_ENUM_VALUE_6, std::set<int>{1, 2, 3});

  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_1>(), Optional(Eq('a')));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_2>(), Optional(Eq(2)));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_3>(), Optional(Eq(false)));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_4>(), ElementsAre(1, 2));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_5>(), ElementsAre(Pair(1, 2)));
  ASSERT_THAT(map_under_test_.Get<TEST_ENUM_VALUE_6>(), ElementsAre(1, 2, 3));
}

TEST_F(StrongTypeMapTest, SetRawAsWorks) {
  map_under_test_.Set<TEST_ENUM_VALUE_1>('a');
  map_under_test_.Set<TEST_ENUM_VALUE_2>(2);
  map_under_test_.Set<TEST_ENUM_VALUE_3>(false);
  map_under_test_.Set<TEST_ENUM_VALUE_4>(std::vector<int>{1, 2});
  map_under_test_.Set<TEST_ENUM_VALUE_5>(std::map<int, int>{{1, 2}});
  map_under_test_.Set<TEST_ENUM_VALUE_6>(std::set<int>{1, 2, 3});

  ASSERT_THAT(map_under_test_.GetRawAs<char>(TEST_ENUM_VALUE_1), Optional(Eq('a')));
  ASSERT_THAT(map_under_test_.GetRawAs<int>(TEST_ENUM_VALUE_2), Optional(Eq(2)));
  ASSERT_THAT(map_under_test_.GetRawAs<bool>(TEST_ENUM_VALUE_3), Optional(Eq(false)));
  ASSERT_THAT(map_under_test_.GetRawAs<std::vector<int>>(TEST_ENUM_VALUE_4), ElementsAre(1, 2));
  ASSERT_THAT((map_under_test_.GetRawAs<std::map<int, int>>(TEST_ENUM_VALUE_5)), ElementsAre(Pair(1, 2)));
  ASSERT_THAT(map_under_test_.GetRawAs<std::set<int>>(TEST_ENUM_VALUE_6), ElementsAre(1, 2, 3));
}

TEST_F(StrongTypeMapTest, MaybeSetWorks) {
  map_under_test_.Set<TEST_ENUM_VALUE_1>('a');

  ASSERT_THAT(map_under_test_.GetRawAs<char>(TEST_ENUM_VALUE_1), Optional(Eq('a')));

  std::optional<char> to_set;

  map_under_test_.MaybeSet<TEST_ENUM_VALUE_1>(to_set);
  ASSERT_THAT(map_under_test_.GetRawAs<char>(TEST_ENUM_VALUE_1), Optional(Eq('a')));

  to_set = 'b';
  map_under_test_.MaybeSet<TEST_ENUM_VALUE_1>(to_set);
  ASSERT_THAT(map_under_test_.GetRawAs<char>(TEST_ENUM_VALUE_1), Optional(Eq('b')));
}

}  // namespace
}  // namespace utils::map