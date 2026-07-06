```text
┌──────────────────────────────┐
│            vikSH             │
│      a tiny shell in C       │
└──────────────────────────────┘
```

# vikSH

A tiny shell written in C because I wanted to understand how shells actually work instead of just using one.
It's simple. No fancy scripting, no job control, no pipes (yet). Just enough to run commands and slowly grow into something better.

## Build

```bash
git clone https://github.com/IONODO/vikSH.git
cd vikSH
make
./viksh
```

## Current features

Built-in commands:

- `cd`
- `ls`
- `mkdir`
- `rmdir`
-and more
run `help` to know more

External programs work too:

```bash
vim
nano
gcc
brave
firefox
...
```

Basically, if it's in your `PATH`, vikSH can launch it.

## Things that don't exist (yet)

- pipes (`|`)
- redirection (`>`, `<`)
- command history
- tab completion
- background jobs (`&`)
- environment variables

Coming soon...

## Why?

No reason other than curiosity, would i suggest replacing your current shell for this, hell no, but can you use it without worrying a system wipeout? ADBOLUTELY
The program is easy and clean enough for you to add in your own builtin functions and make your own shell as well.
