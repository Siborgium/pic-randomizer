## Ransel

#### What

Ransel is small commandline tool, created in order to simplify random selection and copying of files.

#### Why

As mentioned above, I have needed a small utility for everyday use. After few improves I decided to upload it here.

#### Synopsis

**ransel** selects *count* files from *DIRECTORY* and then performs certain operations on them according to the given flags.

Syntax:
`ransel [OPTIONS] DIRECTORY`

Example:
`Example: ransel --count=15 --list example/`

Options:

| Short name | Full name | Default value | Description |
| ---  | --- | --- | --- |
| -h | --help | --- | Display help message and quit |
| -l | --list | --- | List all selected files |
| -c | --copy | --- | Copy selected files to the directory |
| -C | --count | 10 | Set count of files to \<N\> |
