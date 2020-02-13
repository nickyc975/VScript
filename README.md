# VScript

一种动态强类型编程语言，语法类似C语言，采用基于栈的虚拟机。

* 查看代码示例：[code](code/)

* 查看详细介绍：[docs](docs/Intro.md)

### 编译试用

本项目的编译需要`g++`，`make`等工具。在Linux下可以直接使用包管理器安装，在windows下可以参考这篇文章：[windows 下使用make命令，编译代码](https://blog.csdn.net/Nicholas_Liu2017/article/details/78323391)

* 编译项目：

```shell
    # 编译输出结果在项目目录下的`build/`文件夹下，其中的`vs`即为可执行文件；
    make
```

* 编译并测试：

``` shell
    # 编译运行，执行test/hello.vs并输出词法分析中间结果到当前目录下的tokens.txt文件
    make test-lex

    # 编译运行，执行test/hello.vs并输出语法分析中间结果到当前目录下的astree.txt文件
    make test-parse

    # 编译运行，执行test/hello.vs并输出生成的中间代码到当前目录下的instructions.vss文件
    make test-gencode

    # 编译运行，执行test/hello.vs并输出所有中间结果
    make test
```

* 单独运行

执行`make`后在项目目录下的`build/`文件夹中即可找到可执行文件`vs`，其使用方法如下：

```
    vs [-l] [-p] [-s] <源文件>
```

其中`-l`，`-p`，`-s`三个参数分别对应`make test-lex`，`make test-parse`，`make test-gencode`。

### 已实现

* 所有变量定义，函数声明，流程控制语句；

* 变量作用域控制，内层代码块可以引用外层代码块的变量；

* 函数调用，包括地柜调用；

* 通用的列表类型，其中可以存放任意类型的数据；

* 输出语句`print`，用法类似函数调用，但可以有任意个任意类型的参数；

* 基本内存管理：目前实现了极其简单的引用计数，只能管理基本类型的值，并且没有循环引用检查；

### 待实现

* 输入语句(input)；

* 类型转换语句；

* 修复嵌套列表语法分析出错的问题；

* 更加完善的内存管理，在不考虑循环引用的情况下可以避免内存泄露；

* 更加健壮的错误处理，目前的错误处理还很残废，遇到错误基本只会退出；

* 结构体/类的支持；

* 对其他源文件的符号引用(import)；

* 字节码的序列化和反序列化；
