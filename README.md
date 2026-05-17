# FileAligner

Align multiple files based on selected columns.

## Usage

```text
fileAligner v1.1.7
Align multiple files based on selected columns.
https://github.com/liu-congcong/fileAligner

Usage:
  fileAligner [options]

Options:
  -c    Columns for alignment (max: 16)
        Examples:
          '1,2 2,3' aligns file1 on columns 1, 2 and file2 on columns 2, 3
          '1,2' aligns all files on columns 1, 2
  -d    Delimiter in input files: tab | comma | space (default: tab)
  -h    Size of the hash table (1-2147483647, default: 1000000)
  -i    Path to the input files
        Each file must contain a header line
  -n    Names of columns used for alignment in output (max: 16, default: *)
  -v    Missing value (default: NA)
```
