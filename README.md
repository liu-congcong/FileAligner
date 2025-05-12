# FileAligner

Align multiple files based on selected columns.

## Usage

```bash
filenAligner v1.0.0
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
  -o    Path to the output file
  -s    Column separator in input files: table (default) | comma | space
```
