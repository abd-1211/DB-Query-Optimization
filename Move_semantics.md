# Move Semantics

> Move semantics let you **transfer** a resource from one object to another instead of copying it, making operations significantly cheaper for heap-owning types.

---

## Value Categories

| Category | Syntax | Meaning |
|----------|--------|---------|
| **Lvalue** | `T&` | A named, persistent location in memory |
| **Rvalue** | `T&&` | A temporary, anonymous value that is about to vanish |

```cpp
int x = 5;      // x  is an lvalue — it has a name and persists
int&& r = 5;    // 5  is an rvalue — a temporary with no name
```

---

## `std::move`

Casts an lvalue to an rvalue, signalling to the compiler that ownership may be transferred.

```cpp
std::move(ptr);   // transfer ownership
ptr = nullptr;    // manual cleanup — move does NOT invalidate the source automatically
```

> ⚠️ **Important:** `std::move` does **not** destroy the original lvalue. You are responsible for leaving it in a valid (typically null/empty) state after the move.

---

## Forwarding / Universal References

A special reference that can bind to **both** lvalues and rvalues. Used in templates when the value category of the argument is unknown.

```cpp
template <typename T>
void wrapper(T&& arg) {
    // T&& here is a forwarding reference, NOT necessarily an rvalue ref
    target(std::forward<T>(arg));   // preserves original value category
}
```

| Scenario | Use |
|----------|-----|
| Known rvalue | `std::move()` |
| Unknown (template) | `std::forward<T>()` |

> Use `std::forward` instead of `std::move` in templates — you don't know whether the argument was originally an lvalue or rvalue, and `std::move` would unconditionally cast it to an rvalue.

---

## Why Move Semantics?

- **Performance** — moving a heap buffer is O(1) (pointer swap); copying is O(n)
- **RAII alignment** — rvalues signal temporaries, enabling the compiler to call move constructors instead of copy constructors automatically
- **Trying to move lvalues without `std::move` results in a deep copy** — wasting performance with no benefit

---

## Quick Reference

```cpp
std::string a = "hello";
std::string b = std::move(a);  // b owns the buffer; a is now valid-but-empty
a = "";                        // explicit cleanup of the moved-from object
```
