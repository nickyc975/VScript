# VScript

A dynamic type programming language.

## Lexical

### Keywords

* Type

```
    none

    bool

    char

    int

    float

    string
```

* Declaration

```
    var

    val

    func
```

* Control

```
    if

    elif

    else

    while

    for

    return

    break

    continue
```

### Operator

```
    + - * / %

    = += -= *= /= %=

    & | ! &= |=

    < > <= >= == !=
```

### Delimiter

```
    , ;

    ( ) [ ] { }
```

### Constant

```
    none: ^none$

    bool: ^(true|false)$

    char: ^\'.\'$

    int: ^-?(\d|[1-9]\d+)$

    float: ^-?(\d|[1-9]\d+)\.\d*$

    string: ^\".+\"$
```

### Identifier

```
    ^[A-Za-z_]\w*$
```
