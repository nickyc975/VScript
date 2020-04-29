#include "objects/VSCharObject.hpp"

#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSFloatObject.hpp"
#include "objects/VSFunctionObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSStringObject.hpp"
#include "objects/VSTupleObject.hpp"

NEW_IDENTIFIER(__hash__);
NEW_IDENTIFIER(__lt__);
NEW_IDENTIFIER(__gt__);
NEW_IDENTIFIER(__le__);
NEW_IDENTIFIER(__ge__);
NEW_IDENTIFIER(__eq__);
NEW_IDENTIFIER(__str__);
NEW_IDENTIFIER(__bytes__);
NEW_IDENTIFIER(__neg__);
NEW_IDENTIFIER(__add__);
NEW_IDENTIFIER(__sub__);
NEW_IDENTIFIER(__mul__);
NEW_IDENTIFIER(__div__);
NEW_IDENTIFIER(__mod__);
NEW_IDENTIFIER(__bool__);
NEW_IDENTIFIER(__char__);
NEW_IDENTIFIER(__int__);
NEW_IDENTIFIER(__float__);

VSObject *vs_char(VSObject *, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("char()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *obj = args[0];
    VSObject *val = CALL_ATTR(obj, ID___char__, EMPTY_TUPLE());
    if (!IS_TYPE(val, T_CHAR)) {
        err("%s.__char__() returned \"%s\" instead of char.", TYPE_STR[obj->type], TYPE_STR[val->type]);
        terminate(TERM_ERROR);
    }

    INCREF_RET(val);
}

VSObject *vs_char_hash(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("char.__hash__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_CHAR, "char.__hash__()");

    INCREF_RET(C_INT_TO_INT((((VSCharObject *)self)->_value)));
}

VSObject *vs_char_lt(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("char.__lt__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_BOOL, "char.__lt__()");
    ENSURE_TYPE(that, T_BOOL, "char.__lt__()");

    cbool_t res = ((VSCharObject *)self)->_value < ((VSCharObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_char_gt(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("char.__gt__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_BOOL, "char.__gt__()");
    ENSURE_TYPE(that, T_BOOL, "char.__gt__()");

    cbool_t res = ((VSCharObject *)self)->_value > ((VSCharObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_char_le(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("char.__le__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_BOOL, "char.__le__()");
    ENSURE_TYPE(that, T_BOOL, "char.__le__()");

    cbool_t res = ((VSCharObject *)self)->_value <= ((VSCharObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_char_ge(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("char.__ge__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_BOOL, "char.__ge__()");
    ENSURE_TYPE(that, T_BOOL, "char.__ge__()");

    cbool_t res = ((VSCharObject *)self)->_value >= ((VSCharObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_char_eq(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("char.__eq__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_BOOL, "char.__eq__()");
    ENSURE_TYPE(that, T_BOOL, "char.__eq__()");

    cbool_t res = ((VSCharObject *)self)->_value == ((VSCharObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_char_str(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("char.__str__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_CHAR, "char.__str__()");

    auto str = std::string();
    str.push_back(((VSCharObject *)self)->_value);
    INCREF_RET(C_STRING_TO_STRING((str)));
}

VSObject *vs_char_bytes(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("char.__bytes__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_CHAR, "char.__bytes__()");

    INCREF_RET(VS_NONE);
}

VSObject *vs_char_neg(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("char.__neg__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_CHAR, "char.__neg__()");

    cchar_t res = -((VSCharObject *)self)->_value;
    INCREF_RET(C_CHAR_TO_CHAR(res));
}

VSObject *vs_char_add(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("char.__add__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_BOOL, "char.__add__()");
    ENSURE_TYPE(that, T_BOOL, "char.__add__()");

    cchar_t res = ((VSCharObject *)self)->_value + ((VSCharObject *)that)->_value;
    INCREF_RET(C_CHAR_TO_CHAR(res));
}

VSObject *vs_char_sub(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("char.__sub__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_BOOL, "char.__sub__()");
    ENSURE_TYPE(that, T_BOOL, "char.__sub__()");

    cchar_t res = ((VSCharObject *)self)->_value - ((VSCharObject *)that)->_value;
    INCREF_RET(C_CHAR_TO_CHAR(res));
}

VSObject *vs_char_mul(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("char.__mul__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_BOOL, "char.__mul__()");
    ENSURE_TYPE(that, T_BOOL, "char.__mul__()");

    cchar_t res = ((VSCharObject *)self)->_value * ((VSCharObject *)that)->_value;
    INCREF_RET(C_CHAR_TO_CHAR(res));
}

VSObject *vs_char_div(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("char.__div__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_BOOL, "char.__div__()");
    ENSURE_TYPE(that, T_BOOL, "char.__div__()");

    if (((VSCharObject *)that)->_value == 0) {
        err("divided by zero\n");
        terminate(TERM_ERROR);
    }

    cchar_t res = ((VSCharObject *)self)->_value / ((VSCharObject *)that)->_value;
    INCREF_RET(C_CHAR_TO_CHAR(res));
}

VSObject *vs_char_mod(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("char.__mod__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_BOOL, "char.__mod__()");
    ENSURE_TYPE(that, T_BOOL, "char.__mod__()");

    if (((VSCharObject *)that)->_value == 0) {
        err("mod by zero\n");
        terminate(TERM_ERROR);
    }

    cchar_t res = ((VSCharObject *)self)->_value % ((VSCharObject *)self)->_value;
    INCREF_RET(C_CHAR_TO_CHAR(res));
}

VSObject *vs_char_bool(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("char.__bool__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_CHAR, "char.__bool__()");

    cchar_t val = ((VSCharObject *)self)->_value;
    INCREF_RET(val ? VS_TRUE : VS_FALSE);
}

VSObject *vs_char_char(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("char.__char__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_CHAR, "char.__char__()");

    INCREF_RET(self);
}

VSObject *vs_char_int(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("char.__int__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_CHAR, "char.__int__()");

    INCREF_RET(C_INT_TO_INT((cint_t)CHAR_TO_C_CHAR(self)));
}

VSObject *vs_char_float(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("char.__float__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_CHAR, "char.__float__()");

    INCREF_RET(C_FLOAT_TO_FLOAT(((cfloat_t)CHAR_TO_C_CHAR(self))));
}

const str_func_map VSCharObject::vs_char_methods = {
    {ID___hash__, vs_char_hash},
    {ID___lt__, vs_char_lt},
    {ID___gt__, vs_char_gt},
    {ID___le__, vs_char_le},
    {ID___ge__, vs_char_ge},
    {ID___eq__, vs_char_eq},
    {ID___str__, vs_char_str},
    {ID___bytes__, vs_char_bytes},
    {ID___neg__, vs_char_neg},
    {ID___add__, vs_char_add},
    {ID___sub__, vs_char_sub},
    {ID___mul__, vs_char_mul},
    {ID___div__, vs_char_div},
    {ID___mod__, vs_char_mod},
    {ID___bool__, vs_char_bool},
    {ID___char__, vs_char_char},
    {ID___int__, vs_char_int},
    {ID___float__, vs_char_float}
};

VSCharObject::VSCharObject(cchar_t value) : _value(value) {
    this->type = T_CHAR;
}

VSCharObject::~VSCharObject() {
}

bool VSCharObject::hasattr(std::string &attrname) {
    return vs_char_methods.find(attrname) != vs_char_methods.end();
}

VSObject *VSCharObject::getattr(std::string &attrname) {
    auto iter = vs_char_methods.find(attrname);
    if (iter == vs_char_methods.end()) {
        ERR_NO_ATTR(this, attrname);
        terminate(TERM_ERROR);
    }

    VSFunctionObject *attr = new VSNativeFunctionObject(
        this, C_STRING_TO_STRING(attrname), vs_char_methods.at(attrname));
    INCREF_RET(attr);
}

void VSCharObject::setattr(std::string &attrname, VSObject *attrvalue) {
    err("Unable to apply setattr on native type: \"%s\"", TYPE_STR[this->type]);
    terminate(TERM_ERROR);
}