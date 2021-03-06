#include "objects/VSIntObject.hpp"

#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSCharObject.hpp"
#include "objects/VSFloatObject.hpp"
#include "objects/VSFunctionObject.hpp"
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

VSIntObject *VSIntObject::_VS_ZERO = NULL;
VSIntObject *VSIntObject::_VS_ONE = NULL;

VSObject *vs_int(VSObject *, VSObject *const *args, vs_size_t nargs) {
    if (nargs == 0) {
        INCREF_RET(VS_ZERO);
    } else if (nargs == 1) {
        VSObject *obj = args[0];
        VSObject *val = CALL_ATTR(obj, ID___int__, EMPTY_TUPLE());
        if (!IS_TYPE(val, T_INT)) {
            err("%s.__int__() returned \"%s\" instead of int.", TYPE_STR[obj->type], TYPE_STR[val->type]);
            terminate(TERM_ERROR);
        }

        INCREF_RET(val);
    } else if (nargs == 2) {
        VSObject *obj = args[0];
        VSObject *base = args[1];
        VSObject *val = CALL_ATTR(obj, ID___int__, vs_tuple_pack(1, base));
        if (!IS_TYPE(val, T_INT)) {
            err("%s.__int__() returned \"%s\" instead of int.", TYPE_STR[obj->type], TYPE_STR[val->type]);
            terminate(TERM_ERROR);
        }

        INCREF_RET(val);
    }

    ERR_NARGS("int()", 2, nargs);
    terminate(TERM_ERROR);
    return NULL;
}

VSObject *vs_int_hash(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("int.__hash__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_INT, "int.__hash__()");

    INCREF_RET(new VSIntObject(((VSIntObject *)self)->_value));
}

VSObject *vs_int_lt(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("int.__lt__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_INT, "int.__lt__()");
    ENSURE_TYPE(that, T_INT, "int.__lt__()");

    bool res = ((VSIntObject *)self)->_value < ((VSIntObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_int_gt(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("int.__gt__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_INT, "int.__gt__()");
    ENSURE_TYPE(that, T_INT, "int.__gt__()");

    bool res = ((VSIntObject *)self)->_value > ((VSIntObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_int_le(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("int.__le__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_INT, "int.__le__()");
    ENSURE_TYPE(that, T_INT, "int.__le__()");

    bool res = ((VSIntObject *)self)->_value <= ((VSIntObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_int_ge(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("int.__ge__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_INT, "int.__ge__()");
    ENSURE_TYPE(that, T_INT, "int.__ge__()");

    bool res = ((VSIntObject *)self)->_value >= ((VSIntObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_int_eq(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("int.__eq__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_INT, "int.__eq__()");
    ENSURE_TYPE(that, T_INT, "int.__eq__()");

    bool res = ((VSIntObject *)self)->_value == ((VSIntObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_int_str(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("int.__str__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_INT, "int.__str__()");

    cint_t val = ((VSIntObject *)self)->_value;
    INCREF_RET(C_STRING_TO_STRING(std::to_string(val)));
}

VSObject *vs_int_bytes(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("int.__bytes__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_INT, "int.__bytes__()");

    INCREF_RET(VS_NONE);
}

VSObject *vs_int_neg(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("int.__neg__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_INT, "int.__neg__()");

    cint_t res = -((VSIntObject *)self)->_value;
    INCREF_RET(C_INT_TO_INT(res));
}

VSObject *vs_int_add(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("int.__add__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_INT, "int.__add__()");
    ENSURE_TYPE(that, T_INT, "int.__add__()");

    cint_t res = ((VSIntObject *)self)->_value + ((VSIntObject *)that)->_value;
    INCREF_RET(C_INT_TO_INT(res));
}

VSObject *vs_int_sub(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("int.__sub__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_INT, "int.__sub__()");
    ENSURE_TYPE(that, T_INT, "int.__sub__()");

    cint_t res = ((VSIntObject *)self)->_value - ((VSIntObject *)that)->_value;
    INCREF_RET(C_INT_TO_INT(res));
}

VSObject *vs_int_mul(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("int.__mul__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_INT, "int.__mul__()");
    ENSURE_TYPE(that, T_INT, "int.__mul__()");

    cint_t res = ((VSIntObject *)self)->_value * ((VSIntObject *)that)->_value;
    INCREF_RET(C_INT_TO_INT(res));
}

VSObject *vs_int_div(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("int.__div__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_INT, "int.__div__()");
    ENSURE_TYPE(that, T_INT, "int.__div__()");

    if (((VSIntObject *)that)->_value == 0) {
        err("divided by zero\n");
        terminate(TERM_ERROR);
    }

    cint_t res = ((VSIntObject *)self)->_value / ((VSIntObject *)that)->_value;
    INCREF_RET(C_INT_TO_INT(res));
}

VSObject *vs_int_mod(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("int.__mod__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_INT, "int.__mod__()");
    ENSURE_TYPE(that, T_INT, "int.__mod__()");

    if (((VSIntObject *)that)->_value == 0) {
        err("mod by zero\n");
        terminate(TERM_ERROR);
    }

    cint_t res = ((VSIntObject *)self)->_value % ((VSIntObject *)that)->_value;
    INCREF_RET(C_INT_TO_INT(res));
}

VSObject *vs_int_bool(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("int.__bool__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_INT, "int.__bool__()");

    cint_t res = ((VSIntObject *)self)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_int_char(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("int.__char__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_INT, "int.__char__()");

    cint_t res = ((VSIntObject *)self)->_value;
    INCREF_RET(C_CHAR_TO_CHAR((cchar_t)res));
}

VSObject *vs_int_int(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("int.__int__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_INT, "int.__int__()");

    INCREF_RET(self);
}

VSObject *vs_int_float(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("int.__float__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_INT, "int.__float__()");

    cint_t res = ((VSIntObject *)self)->_value;
    INCREF_RET(C_FLOAT_TO_FLOAT((cfloat_t)res));
}

const str_func_map VSIntObject::vs_int_methods = {
    {ID___hash__, vs_int_hash},
    {ID___lt__, vs_int_lt},
    {ID___gt__, vs_int_gt},
    {ID___le__, vs_int_le},
    {ID___ge__, vs_int_ge},
    {ID___eq__, vs_int_eq},
    {ID___str__, vs_int_str},
    {ID___bytes__, vs_int_bytes},
    {ID___neg__, vs_int_neg},
    {ID___add__, vs_int_add},
    {ID___sub__, vs_int_sub},
    {ID___mul__, vs_int_mul},
    {ID___div__, vs_int_div},
    {ID___mod__, vs_int_mod},
    {ID___bool__, vs_int_bool},
    {ID___char__, vs_int_char},
    {ID___int__, vs_int_int},
    {ID___float__, vs_int_float}
};

VSIntObject::VSIntObject(cint_t value) : _value(value) {
    this->type = T_INT;
}

VSIntObject::~VSIntObject() {
}

bool VSIntObject::hasattr(std::string &attrname) {
    return vs_int_methods.find(attrname) != vs_int_methods.end();
}

VSObject *VSIntObject::getattr(std::string &attrname) {
    auto iter = vs_int_methods.find(attrname);
    if (iter == vs_int_methods.end()) {
        ERR_NO_ATTR(this, attrname);
        terminate(TERM_ERROR);
    }

    VSFunctionObject *attr = new VSNativeFunctionObject(
        this, C_STRING_TO_STRING(attrname), vs_int_methods.at(attrname));
    INCREF_RET(attr);
}

void VSIntObject::setattr(std::string &, VSObject *) {
    err("Unable to apply setattr on native type: \"%s\"", TYPE_STR[this->type]);
    terminate(TERM_ERROR);
}