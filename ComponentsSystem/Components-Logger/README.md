# Logger component

## Simple to use logger with parallel / synchronous output, used to be main component in my applications. Also supports output for some Qt types, in future new types will be added (I have no need in this improvement now)

### REQUIRES Components-Core repository

---

## Example of usage:
```cpp
#include <Components/Logger/Logger.h>

int main(int argc, char* argv[]) {

    // Required! if did not set, logger throws exception of type std::runtime_error on first write try
    // Set directory (may not be absolute path) to save logs with file name like "1970-12-31_23-59-59.log"
    // File limit in undefined, so cleaning directory from old logs is not logger's responsibility
    COMPLOG_SET_LOGSDIR(".");

    // It's same (spaces and endline placed automatically)
    COMPLOG_DEBUG("Hello,", "world!");
    COMPLOG_DEBUG("Hello, world!");

    // Sync variant (prints immediately, can be in different order than parallel output before or later)
    COMPLOG_SYNC_DEBUG("Hello, world!");
    COMPLOG_SYNC_DEBUG("Hello, world number", 1, "with about of", 123.123, "symbols in code!");

    // Output types
    COMPLOG_EMPTY   ("My output string!"); // Equals to: My output string!
    COMPLOG_DEBUG   ("My output string!"); // Equals to: 1970-01-01T01:01:01.001 [ DEBG ] My output string!
    COMPLOG_INFO    ("My output string!"); // Equals to: 1970-01-01T01:01:01.001 [ INFO ] My output string!
    COMPLOG_WARNING ("My output string!"); // Equals to: 1970-01-01T01:01:01.001 [ WARN ] My output string!
    COMPLOG_ERROR   ("My output string!"); // Equals to: 1970-01-01T01:01:01.001 [ FAIL ] My output string!
    COMPLOG_OK      ("My output string!"); // Equals to: 1970-01-01T01:01:01.001 [  OK  ] My output string!

    // All parallel output placed into logger, will be print on program exit (stack unfolding), if didn't have time for it
    return 0;
}
```