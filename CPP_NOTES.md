# CPP NOTES

## 1) Hello World

### Linking

Multiple statements (single line of code) make a source code (code understood by humans) which is accessed by compiler to create object files (code understood by computer) that are linked together through a linker to create an **EXECUTABLE FILE**.

Library is simply some code written by others that we access using declarations (include namespace std). They are found in imported modules.

- **Compile time errors** – occur during compilation
- **Link time errors** – occur during linking
- **Run time errors** – occur during execution

---

## 2) Objects, Types and Values

- **Object** = memory space specified with what type of data can be stored in it
- A named object is called a **variable**
- Names are necessary to access the object

**Definition** = defines a variable; for example:
```cpp
int age = 12;
string name;
```

**Initializer** = name followed by `=`; for example:
```cpp
int age = 12;        // initializer: 12
string name = "Abdullah";  // initializer: "Abdullah"
```

### Operators

For strings `a` and `b`, checks which ASCII value is greater or smaller (`<`, `>`) character by character.

**Example:**
```
string a = "cat"
string b = "car"

a < b ?
  c == c  (Same ASCII val)
  a == a  (Same ASCII val)
  t != r  (diff ASCII val) → t > r, so a > b
```

### Composite Assignment Operators

```cpp
int i = 9;
cout << ++i;  // i = 10  → first increment, then print
cout << i++;  // i = 10  → first print, then increment
cout << i;    // i = 11
```

### Names

- Names starting with `_` (e.g., `_foo`) are **reserved for system entities**
- Names in ALL_CAPS (e.g., `FOO`) are **reserved for macros**

### Types and Objects

| Type     | Size     | Example         |
|----------|----------|-----------------|
| `char`   | 1 byte   | `'x'`           |
| `int`    | 4 bytes  | `12043`         |
| `double` | 8 bytes  | `7.3`           |
| `string` | variable | `"Abdullah"`    |

### Conversions

```cpp
char c   = 'x';       // x has ASCII val 120
int i1   = c;         // converts to 120
int i2   = c + 1000;  // 120 + 1000 = 1120
double d = i2 + 7.3;  // 1120 + 7.3 = 1127.3
```

Going from a **larger to smaller type** (narrowing conversions) causes data loss.

> **Example:** `12043 (int) → char` causes data loss because `char` is 1 byte and `int` is 4 bytes.

#### ⚠️ Avoiding Narrowing Bugs — Brace Initialization

```cpp
int x = 7.8;    // becomes 7 → silent data loss (problem)
int x {7.8};    // compile-time error (fix)
int x = {7.8};  // compile-time error (fix)
```

#### Direct vs Copy Initialization *(both allow narrowing)*

```cpp
int x3(3);   // Direct initialization — calls constructor directly (more efficient for classes)
int x3 = 3;  // Copy initialization — may involve an extra copy (less efficient)
```

### Type Deduction — `auto`

Instead of:
```cpp
int x    = 7;
double d = 7.4;
```

Use `auto` to let the compiler deduce the type:
```cpp
auto x = 7;    // deduced as int
auto d = 7.5;  // deduced as double
```

### Lvalue vs Rvalue

| Term   | Meaning                   |
|--------|---------------------------|
| lvalue | The **object** itself     |
| rvalue | The **value** inside the object |

---

## 3) Computation

### Vectors

```cpp
vector<obj_type> v(size);
v[2] = 44;
v.size();  // a vector knows its own size
```

#### Array vs Vector

| Feature          | Array              | Vector             |
|------------------|--------------------|--------------------|
| Size             | Static (fixed)     | Dynamic            |
| Declaration      | `int arr[4];`      | `vector<int> v;`   |
| Size at init     | Required           | Not required       |

### Range-for Loop

```cpp
for (int x : v)   // for each x in v
    cout << x;
```

### Growing a Vector — `push_back()`

Adds an element to the end of a vector:

```cpp
vector<double> v;    // v = {}
v.push_back(2.7);    // v = {2.7}
```

### Function Call vs Function Reference

```cpp
// Function call — executes push_back on vector v
v.push_back(10);

// Function reference — stores the location of push_back
auto f = &vector<int>::push_back;
vector<int> v;
(v.*f)(10);   // dereferences f, applies on object v with parameter 10
// equivalent to: v.push_back(10)
```

### C++20 — `ranges::sort()`

```cpp
ranges::sort(v);  // new sort function introduced in C++20
```
