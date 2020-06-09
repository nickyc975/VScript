// 返回一个将参数加上n的函数
func inc_n_func(n) {
    func inc_n(v) {
        return v + n;
    }
    return inc_n;
}

// 接收一个列表和一个函数，对列表中的每一个元素调用接收的函数
func map(lst, f) {
    var res = [];
    for (var i = 0; i < lst.len(); i += 1) {
        var mapped = f(lst[i]);
        res.append(mapped);
    }
    return res;
}

//　生成一个将参数加一的函数
var inc_one = inc_n_func(1);

// 将列表的每一项加一
print(map([1, 2, 3], inc_one));