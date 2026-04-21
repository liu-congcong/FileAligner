# FileAligner

Align multiple files based on selected columns.

## Usage

```text
filenAligner v1.1.4
Align multiple files based on selected columns.
https://github.com/liu-congcong/fileAligner

Usage:
  filenAligner [options]

Options:
  -i    Path to the input files
        A header line for each file should be existed
  -t    Target columns for alignment
        Examples:
          '1,2 2,3' aligns file1 on columns 1, 2 and file2 on columns 2, 3
          '1,2' aligns all files on columns 1, 2
  -h    Size of the hash table (1-2147483647, default: 1000000)
  -s    Column separator in input files: table|comma|space (default: table)
  -v    Value of the empty column (default: "")
```
