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
Usage:
    FileAligner -i <files> -t <cols> [-o <file>] [-s <sep>]
    FileAligner -i input1 input2 -t 1,2,8 1,6,8 -o output -s t
    FileAligner -i input1 input2 input3 -t 10,1 -o output -s c
    FileAligner -i input1 input2 input3 input4 input5 -t 1
Options:
    -i/--inputs: <input1> ... <inputN> files with a header line
    -t/--targets: <1col1,...,1colM> ... <Ncol1,...,NcolM>
                  <col1,...,colM> for all files
    -o/--output: <output> default: 20231003-093838.txt
    -s/--separator: <table|comma|space> default: table
```
