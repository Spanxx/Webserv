## **File & Stream Handling**

|Function / Class|Description|
|---|---|
|`std::ifstream`|Input file stream used to open and read files. Use `.open()` to open, `.is_open()` to check, and stream operators (`>>`, `getline`, `rdbuf`) to read.|
|`std::ifstream::rdbuf()`|Returns a pointer to the stream buffer, often used to read the entire file into a `stringstream`.|
|`std::ostringstream`|Used to build strings using the `<<` operator. Great for constructing response headers or the full HTTP response.|
|`std::istringstream`|Used to parse strings as input streams (e.g. to split or read line-by-line).|
|`std::getline()`|Reads a full line from an input stream into a `std::string`.|

---

## **String Manipulation**

|Function / Class|Description|
|---|---|
|`std::string::substr(pos, len)`|Returns a substring from the string starting at `pos` of length `len`.|
|`std::string::find()`|Finds the first occurrence of a substring and returns its position, or `std::string::npos` if not found.|
|`std::string::rfind()`|Finds the last occurrence of a substring. Useful for extracting file extensions.|
|`std::string::length()` or `.size()`|Returns the number of characters in the string.|
|`std::string::empty()`|Returns `true` if the string is empty.|
|`std::string::erase(pos, len)`|Removes part of the string.|
|`std::string::append(str)`|Appends a string to the end of another.|
|`std::string::c_str()`|Returns a C-style (`const char*`) version of the string. Useful when passing strings to old-style functions (like `open()`, etc.).|

---

## **Containers**

|Function / Class|Description|
|---|---|
|`std::map<Key, Value>`|Stores key–value pairs, like headers (`Content-Type`, etc.). Allows fast access with `map[key]`.|
|`std::vector<T>`|Dynamic array of items (e.g. storing multiple location configs). Use `.push_back()`, `.size()`, `.at()`, or iterator access.|

---

## **Utilities**

|Function / Class|Description|
|---|---|
|`std::stringstream`|Combines `ostringstream` and `istringstream`; used for reading/writing formatted data.|
|`std::ostringstream::str()`|Returns the string built by the stream.|
|`std::stringstream << val`|Adds data into the stream (any type with operator `<<` defined).|
|`std::stringstream >> val`|Reads from the stream (used to parse input).|
|`std::ostringstream::flush()`|Clears the buffer (optional).|

---

## **I/O Streams**

|Function / Class|Description|
|---|---|
|`std::cout`, `std::cerr`|Output to standard output or error (useful for debugging).|
|`std::endl`|Ends a line and flushes the output buffer.|
|`<<`, `>>`|Stream operators for inserting and extracting formatted data.|

---

## **File/Path Utilities (custom, not in std)**


C++98 has no `<filesystem>` — so you’ll need to:

- Use `stat()` (from `<sys/stat.h>`) to check if a file exists or if it's a directory.
    
- Use `open()`/`close()` (from `<fcntl.h>` / `<unistd.h>`) for lower-level file handling if needed.
    
- Implement helpers manually for:
    
    - Extracting file extensions
        
    - Checking if a path ends in `/`
        
    - Trimming whitespace
        
    - Normalizing slashes (if needed)