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

## 语义分析任务

* 符号表管理

* val初始化检查

* if, for, while的回填

* 赋值操作左值检查

* continue, break 检查

* return　检查
