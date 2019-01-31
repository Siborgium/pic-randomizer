## Ransel

#### What

Ransel is small commandline tool, created in order to simplify selection and copying of random files.

#### Why

As texted above, I needed a small tool for everyday use. After few improves I decided to upload it here.

#### How to

Use is simple and quite straightforward. Run the program from commandline, specifying path the directory and (optionally) flags. It will create a directory and copy chosen files there, so you can easily pick it up, and write information about these files.


Syntax:
`ransel [path] [flags]`

Example:
`ransel pictures/ --nowrite`

Flags:

| Flag argument | Value by default | Meaning |
| ---  | --- | --- |
| --help | - | Prints help information to stdout |
| -nowrite | False | Prevents program from writing any output to console |
| -delete | False | Deletes created directory when program closes |
| -nocopy | False | Prevents program from creating a directory and copying files, may be useful for getting file paths only |
| -count=`n` | 10 | Sets N (max limit of random) to `n` |

