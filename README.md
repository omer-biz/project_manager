# Project Manger

A simple cli tool to track of you projects. It simply stores the directory
and how much time you have spent in the directory.

## Compiling

```sh
$ make
```

```sh
$ ./pm --help
Usage: pm [OPTION...]
A simple program to track of my personal projects

  -a, --activate=project_name   Activate a project i.e. cd into the directory
                             and start a shell
  -l, --list                 List all the available projects
  -n, --new[=project_name]   Create a new project in the current directory, if
                             no name is provided the current directory's name
                             will be used
  -x, --create_and_activate[=project_name]
                             Creates and activates a project in the current
                             directory
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.

Report bugs to omerabdi@protonmail.com.
```
