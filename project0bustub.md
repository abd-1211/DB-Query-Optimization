# Project #0 — Cpp Primer

## Count-Min Sketch Implementation (`bustub`)

---

### Constructor

- Declared a `sketch_` 2D vector to store the Count-Min Sketch in the `.h` file.
- Modified the constructor to initialize the sketch with each `depth` (row) having `width` (column) vectors.
- Added a guard to throw `std::invalid_argument` if either `width` or `depth` is zero.

```cpp
CountMinSketch(uint32_t width, uint32_t depth) : width_(width), depth_(depth) {
  if (width == 0 || depth == 0)
    throw std::invalid_argument("Width and Depth must be greater than 0");
  sketch_ = std::vector<std::vector<uint32_t>>(depth_, std::vector<uint32_t>(width_));
}
```

---

### Move Constructor

- Declared using `noexcept` to signal no exceptions will be thrown during the move.
- Transferred ownership of `sketch_` and `hash_functions_` from the source object to the new object using `std::move`, avoiding a deep copy.
- Copies scalar members `width_` and `depth_` directly (cheap to copy, no heap allocation).



---

### Move Assignment Operator

- Checks for self-assignment using `this != &other` to avoid corrupting the object.
- Scalar members `width_` and `depth_` are copied directly.
- `sketch_` and `hash_functions_` are moved using `std::move` to transfer heap ownership without copying.
- Returns `*this` to support chained assignments.




---

### Insert

- Acquires a `std::lock_guard<std::mutex>` at the start to make the operation **thread-safe**; the lock is released automatically when the function goes out of scope.
- Loops over each of the `depth_` hash functions.
- For each hash function `i`, computes the column index as `hash_functions_[i](item) % width_`.
- Increments `sketch_[i][column]` by 1 to record the item's occurrence.


---

### Merge

- Validates that both sketches have identical `width_` and `depth_`; throws `std::invalid_argument` if they differ.
- Iterates over every cell `[i][j]` in the 2D sketch matrix.
- Adds the corresponding cell value from `other.sketch_` into `this->sketch_`, combining the frequency counts from both sketches.



---

### Count

- Initializes `min_count` to `UINT32_MAX` as a sentinel (any real count will be smaller).
- Loops over all `depth_` hash functions to compute each row's column index for the given item.
- Takes the **minimum** across all rows using `std::min` — this is the core property of Count-Min Sketch that provides an upper-bound estimate of the true frequency.
- Returns the minimum count found.



---

### Clear

- Iterates over every cell `[i][j]` in the full `depth_ × width_` matrix.
- Resets each cell to `0`, effectively wiping all recorded frequency counts and returning the sketch to its initial state.



---

### TopK

- Iterates over every item in the provided `candidates` vector and calls `Count()` to get its estimated frequency.
- Stores each `(item, count)` pair in a results vector.
- Sorts the results vector in **descending order** by count using a lambda comparator, so the most frequent items appear first.
- Truncates the results to the top `k` entries using `resize()` if there are more than `k` candidates.
- Returns the final vector of up to `k` `(KeyType, uint32_t)` pairs.

## Debugging

- Edgetest1 failed - Did not check for valid width and depth before assigning sketch_
