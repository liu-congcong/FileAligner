# FileAligner

Align files according to the selected columns

## Usage

```bash
git clone https://github.com/liu-congcong/FileAligner
cd FileAligner
gcc FileAligner.c -o FileAligner
```

```bash
FileAligner -h
Align files according to the selected columns (https://github.com/liu-congcong/FileAligner)
Usage:
    FileAligner -i <files> -t <cols> -o <file> [-s <sep>]
    e.g.: FileAligner -i input1 input2 -t 1,2,8 1,6,8 -o output -s t
Options:
    -i/--inputs: <input1> ... <inputN> files with a header line
    -t/--targets: <1col1,...,1colM> ... <Ncol1,...,NcolM>
    -o/--output: <output>
    -s/--seperator: <table|comma|space>
```
