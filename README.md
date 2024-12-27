# TypedMap

A strongly-typed map structure for C++20 where keys are enum values and values are type-safe through template specialization.

## Features

- Strong type safety through compile-time checks
- Support for both simple types and containers
- Enum-based keys with type-safe values
- Container operations (vector, map, set)
- Optional value handling
- Map merging capabilities

## Requirements

- C++20 or later
- Standard Template Library (STL)

## Usage

### 1. Define Your Enum and Constraints

First, define an enum and create a constraints class that maps enum values to their corresponding types:

```cpp
enum MyEnum {
  STRING_VALUE,
  INT_VALUE,
  VECTOR_VALUE
};

struct MyConstraints : public ConstraintsTemplate<MyEnum> {
  template <MyEnum T, typename E = void>
  struct TypeOf {
    using type = void;
  };
  
  template <MyEnum T>
  struct TypeOf<T, typename std::enable_if_t<T == STRING_VALUE>> {
    using type = std::string;
  };
  
  template <MyEnum T>
  struct TypeOf<T, typename std::enable_if_t<T == INT_VALUE>> {
    using type = int;
  };
  
  template <MyEnum T>
  struct TypeOf<T, typename std::enable_if_t<T == VECTOR_VALUE>> {
    using type = std::vector<int>;
  };
};
```

### 2. Create and Use TypedMap

```cpp
// Create the map
TypedMap<MyConstraints, std::string, int, std::vector<int>> map;

// Set values
map.Set<STRING_VALUE>("hello");
map.Set<INT_VALUE>(42);
map.Set<VECTOR_VALUE>(std::vector<int>{1, 2, 3});

// Get values
auto str = map.Get<STRING_VALUE>();        // Returns std::optional<std::string>
auto num = map.Get<INT_VALUE>();           // Returns std::optional<int>
auto vec = map.Get<VECTOR_VALUE>();        // Returns std::vector<int>

// Container operations
map.Add<VECTOR_VALUE>(4);                  // Adds to vector
map.Mutable<VECTOR_VALUE>().push_back(5);  // Direct container access
```

### 3. Advanced Features

#### Optional Values
```cpp
std::optional<int> maybe_value = 42;
map.MaybeSet<INT_VALUE>(maybe_value);  // Sets only if value exists
```

#### Map Operations
```cpp
auto keys = map.KeySet();              // Get all set keys
map.Unset(STRING_VALUE);              // Remove a value
map.Merge(other_map);                 // Merge with another TypedMap
```

## Implementation Details

The implementation uses several modern C++ features:
- Template metaprogramming
- Concepts (C++20)
- std::variant for type-safe union
- std::optional for nullable values
- Ranges (C++20) for key set operations

For detailed implementation, see the source code:
```cpp:typed_map.h
startLine: 1
endLine: 230
```

For usage examples, see the test file:
```cpp:typed_map_test.cpp
startLine: 1
endLine: 254
```

## License

MIT License - See LICENSE file for details.

## Contributing

Contributions are welcome! Please feel free to submit pull requests.
