### pic randomizer

#### What

Pic*tures* randomizer is a small commandline tool, allowing you to easily choose few random files from specified folder. It was created for pictures, but works with any kind of files.

#### Why

As texted above, I needed a small tool for everyday use. After few improves I decided to upload it here.

#### How to

Use is simple and quite straightforward. Run the program from commandline, specifying path the directory and (optionally) flags. It will create a directory and copy chosen files there, so you can easily pick it up, and write information about these files.


Syntax:
`pic-rnd.exe [path] [flags]`

Example:
`pic-rnd.exe E:/pictures/ -nowrite`

Flags:

| Flag argument | Value by default | Meaning |
| ---  | --- | --- |
| --help | - | Prints help information to stdout |
| -nowrite | False | Prevents program from writing any output to console |
| -delete | False | Deletes created directory when program closes |
| -nocopy | False | Prevents program from creating a directory and copying files, may be useful for getting file paths only |
| -count=`n` | 10 | Sets N (max limit of random) to `n` |

#### Build

I provided few pre-compiled binaries for Windows, but if you use another OS and don't wish to use Wine, run `build.py` or compile the source file by yourself. You will need any C++ compiler, supporting C++17 and filesystem"
