Name: cexcept
Summary: Exception handling in C
Authors: Adam M. Costello and Cosmin Truta
Version: 2.0.1-optipng
Base version: 2.0.1
URL: http://www.nicemice.net/cexcept/

Changes:
- Made exception__prev volatile to avoid "variable might be clobbered
  by longjmp" warnings when a function contains multiple Try blocks.

Patch: cexcept-2-0-1-optipng.diff
