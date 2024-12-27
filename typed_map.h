#ifndef TYPED_MAP_H
#define TYPED_MAP_H

#include <optional>
#include <ranges>
#include <variant>
#include <vector>

namespace utils::map {
namespace internal {
template <typename T>
concept IsContainer = requires(T t) {
  t.empty();
};
template <typename T>
concept IsNotContainer = !IsContainer<T>;

template <class, template <class...> class>
inline constexpr bool is_specialization = false;

template <template <class...> class T, class... Args>
inline constexpr bool is_specialization<T<Args...>, T> = true;
}  // namespace internal

// Client needs to implement this class.
template <typename EnumType>
struct ConstraintsTemplate {
  using enum_type = EnumType;
  static_assert(std::is_enum_v<EnumType>, "EnumType must be an enum type.");
  template <EnumType T, typename E = void>
  struct TypeOf {
    using type = void;
  };
};

template <typename Constraints, typename... Types>
class TypedMap {
 public:
  using EnumType = Constraints::enum_type;

 private:
  class StorageType {
   public:
    template <typename, typename...>
    friend class TypedMap;

    using EnumType = Constraints::enum_type;

   private:
    template <EnumType enum_value>
    decltype(auto) Get() const {
      std::optional<typename Constraints::template TypeOf<enum_value>::type> result;
      if (std::holds_alternative<typename Constraints::template TypeOf<enum_value>::type>(
              internal_storage_)) {
        result = std::get<typename Constraints::template TypeOf<enum_value>::type>(internal_storage_);
      }
      return result;
    }

    template <EnumType enum_value, typename KeyType>
    decltype(auto) Get(KeyType key) const {
      using desired_type = typename Constraints::template TypeOf<enum_value>::type;
      std::optional<desired_type::mapped_type> result;
      if (std::holds_alternative<desired_type>(internal_storage_)) {
        if (auto it = std::get<desired_type>(internal_storage_).find(key); it !=
            std::get<desired_type>(internal_storage_).end()) {
          result = it->second;
        }
      }
      return result;
    }

    template <EnumType enum_value>
    decltype(auto) Mutable() {
      using desired_type = typename Constraints::template TypeOf<enum_value>::type;
      if (!std::holds_alternative<desired_type>(internal_storage_)) {
        internal_storage_ = desired_type();
      }
      return std::get<desired_type>(internal_storage_);
    }

    template <EnumType enum_value, typename ValueType>
    void Set(ValueType&& value) {
      internal_storage_ = std::forward<ValueType>(value);
    }

    template <EnumType enum_value, typename KeyType, typename ValueType>
    void Set(KeyType key, ValueType&& value) {
      using desired_type = typename Constraints::template TypeOf<enum_value>::type;
      if (std::holds_alternative<desired_type>(internal_storage_)) {
        std::get<desired_type>(internal_storage_)[key] = std::move(value);
      } else {
        internal_storage_ = desired_type{{key, value}};
      }
    }

    template <EnumType enum_value, typename ValueType>
    void Add(ValueType&& value) {
      using desired_type = typename Constraints::template TypeOf<enum_value>::type;
      if (!std::holds_alternative<desired_type>(internal_storage_)) {
        internal_storage_ = desired_type();
      }
      if constexpr (requires(desired_type v) { v.push_back(std::forward<ValueType>(value)); }) {
        std::get<desired_type>(internal_storage_).push_back(std::forward<ValueType>(value));
      } else {
        std::get<desired_type>(internal_storage_).insert(std::forward<ValueType>(value));
      }
    }

   private:
    std::variant<std::monostate, Types...> internal_storage_;
  };

 public:
  void Unset(EnumType type) { storage_.erase(type); }

  TypedMap& Merge(const TypedMap& from) {
    for (auto& status : from.storage_) {
      storage_[status.first] = status.second;
    }
    return *this;
  }

  std::set<EnumType> KeySet() const {
    auto kv = storage_ | std::views::keys;
    return {kv.begin(), kv.end()};
  }

  template <internal::IsNotContainer T>
  std::optional<T> GetRawAs(EnumType enum_value) const {
    auto storage = storage_.find(enum_value);
    if (storage == storage_.end()) {
      return std::nullopt;
    }
    if (!std::holds_alternative<T>(storage->second.internal_storage_)) {
      return std::nullopt;
    }
    return std::get<T>(storage->second.internal_storage_);
  }

  template <internal::IsContainer T>
  T GetRawAs(EnumType enum_value) const {
    T default_result;
    auto storage = storage_.find(enum_value);
    if (storage == storage_.end()) {
      return default_result;
    }
    if (!std::holds_alternative<T>(storage->second.internal_storage_)) {
      return default_result;
    }
    return std::get<T>(storage->second.internal_storage_);
  }

  template <typename ValueType>
  TypedMap& SetRaw(EnumType enum_value, ValueType value) {
    storage_[enum_value].internal_storage_ = value;
    return *this;
  }

  template <EnumType enum_value>
  decltype(auto) Get() const {
    using desired_type = decltype(StorageType().Get<enum_value>());
    if constexpr (requires(desired_type v) { v->empty(); }) {
      // For containers, return the shared default instance if the value doesn't exist.
      desired_type result;
      if (auto val = storage_.find(enum_value); val != storage_.end()) {
        result = val->second.Get<enum_value>();
      }
      if (result) {
        return static_cast<desired_type::value_type>(std::move(*result));
      } else {
        return desired_type::value_type();
      }
    } else {
      if (auto result = storage_.find(enum_value); result != storage_.end()) {
        return result->second.Get<enum_value>();
      }
      return static_cast<desired_type>(std::nullopt);
    }
  }

  template <EnumType enum_value, typename KeyType>
  decltype(auto) Get(KeyType key) const {
    using desired_type = decltype(StorageType().Get<enum_value>(key));
    if (auto result = storage_.find(enum_value); result != storage_.end()) {
      return result->second.Get<enum_value>(key);
    }
    return static_cast<desired_type>(std::nullopt);
  }

  template <EnumType enum_value>
  decltype(auto) Mutable() {
    return storage_[enum_value].Mutable<enum_value>();
  }

  template <EnumType enum_value, typename ValueType>
  TypedMap& Set(ValueType&& value) {
    storage_[enum_value].Set<enum_value>(std::forward<ValueType>(value));
    return *this;
  }

  template <EnumType enum_value, typename KeyType, typename ValueType>
  TypedMap& Set(KeyType key, ValueType&& value) {
    storage_[enum_value].Set<enum_value>(std::move(key), std::forward<ValueType>(value));
    return *this;
  }

  // Maybe call the corresponding Set.
  // If parameter is std::nullopt, does nothing.
  // Otherwise, call Set with parameter.
  template <EnumType enum_value, typename ParamType>
  TypedMap& MaybeSet(std::optional<ParamType> param) {
    if (param) {
      return Set<enum_value>(*param);
    }
    return *this;
  }

  template <EnumType enum_value, typename ValueType>
  TypedMap& Add(ValueType&& value) {
    storage_[enum_value].Add<enum_value>(std::forward<ValueType>(value));
    return *this;
  }

 private:
  std::map<EnumType, StorageType> storage_;
};
}  // namespace utils::map

#endif  // TYPED_MAP_H