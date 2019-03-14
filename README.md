# mine sweeper implementation(?) in C++17

- Genre 

    Puzzle game

- Develop environment

    Microsoft Visual C++ 2017

- Language
  
    C++17

- Target age

    All

## About this software

This is an implementation of mine sweeper, traditional video game. 
This is free and unencumbered software released into the public domain. For more information, please refer to [LICENSE](./LICENSE)

## How to play

1. First, choose size of map and amount of bomb. Then type `s j-9 15` if you want map of a-i and 0-8 and 15 bombs.
2. Now, you can play mine sweeper as usual. Open cells with `o <coord[a-i]>-<coord[0-9]>` command, and set a flag with `f <coord[a-i]>-<coord[0-9]>` command.
3. If you open all the cell except bombs, program will end.

### Play example

```txt
>s j-9 10
 0 1 2 3 4 5 6 7 8
a■■■■■■■■■
b■■■■■■■■■
c■■■■■■■■■
d■■■■■■■■■
e■■■■■■■■■
f■■■■■■■■■
g■■■■■■■■■
h■■■■■■■■■
i■■■■■■■■■

>o a-0
 0 1 2 3 4 5 6 7 8
a
b 1 2 2 1   1 1 1
c■■■ 1   2■ 2
d 1 2 2 1   2■ 2
e         1 3■ 2
f         1■■ 1
g       1 2■■ 2 1
h       1■■■■■
i       1■■■■■

>f c-2
 0 1 2 3 4 5 6 7 8
a
b 1 2 2 1   1 1 1
c■■！ 1   2■ 2
d 1 2 2 1   2■ 2
e         1 3■ 2
f         1■■ 1
g       1 2■■ 2 1
h       1■■■■■
i       1■■■■■

>o c-0
 0 1 2 3 4 5 6 7 8
a
b 1 2 2 1   1 1 1
c 1■！ 1   2■ 2
d 1 2 2 1   2■ 2
e         1 3■ 2
f         1■■ 1
g       1 2■■ 2 1
h       1■■■■■
i       1■■■■■

>o e-6
 0 1 2 3 4 5 6 7 8
a
b 1 2 2 1   1 1 1
c 1■！ 1   2■ 2
d 1 2 2 1   2■ 2
e         1 3 3 2
f         1■■ 1
g       1 2■■ 2 1
h       1■■■■■
i       1■■■■■
```
