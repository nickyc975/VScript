#include "objects/VSFloatObject.hpp"

#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSCharObject.hpp"
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
NEW_IDENTIFIER(__bool__);
NEW_IDENTIFIER(__char__);
NEW_IDENTIFIER(__int__);
NEW_IDENTIFIER(__float__);

VSObject *vs_float(VSObject *, VSObject *const *args, vs_size_t nargs) {
    if (nargs == 0) {
        INCREF_RET(new VSFloatObject(0.0));
    } else if (nargs == 1) {
        VSObject *obj = args[0];
        VSObject *val = CALL_ATTR(obj, ID___float__, EMPTY_TUPLE());
        if (!IS_TYPE(val, T_FLOAT)) {
            err("%s.__float__() returned \"%s\" instead of \"float\".", TYPE_STR[obj->type], TYPE_STR[val->type]);
            terminate(TERM_ERROR);
        }

        INCREF_RET(val);
    } else {
        ERR_NARGS("float()", 1, nargs);
        terminate(TERM_ERROR);
    }
}

VSObject *vs_float_hash(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("float.__hash__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_FLOAT, "float.__hash__()");

    cfloat_t val = ((VSFloatObject *)self)->_value;
    std::size_t hash = std::hash<cfloat_t>{}(val);
    INCREF_RET(C_INT_TO_INT(hash));
}

VSObject *vs_float_lt(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("float.__lt__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_FLOAT, "float.__lt__()");
    ENSURE_TYPE(that, T_FLOAT, "float.__lt__()");

    bool res = ((VSFloatObject *)self)->_value < ((VSFloatObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_float_gt(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("float.__gt__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_FLOAT, "float.__gt__()");
    ENSURE_TYPE(that, T_FLOAT, "float.__gt__()");

    bool res = ((VSFloatObject *)self)->_value > ((VSFloatObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_float_le(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("float.__le__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_FLOAT, "float.__le__()");
    ENSURE_TYPE(that, T_FLOAT, "float.__le__()");

    bool res = ((VSFloatObject *)self)->_value <= ((VSFloatObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_float_ge(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("float.__ge__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_FLOAT, "float.__ge__()");
    ENSURE_TYPE(that, T_FLOAT, "float.__ge__()");

    bool res = ((VSFloatObject *)self)->_value >= ((VSFloatObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_float_eq(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("float.__eq__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_FLOAT, "float.__eq__()");
    ENSURE_TYPE(that, T_FLOAT, "float.__eq__()");

    bool res = ((VSFloatObject *)self)->_value == ((VSFloatObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_float_neg(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("float.__neg__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_FLOAT, "float.__neg__()");

    cfloat_t res = -((VSFloatObject *)self)->_value;
    INCREF_RET(C_FLOAT_TO_FLOAT(res));
}

VSObject *vs_float_add(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("float.__add__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_FLOAT, "float.__add__()");
    ENSURE_TYPE(that, T_FLOAT, "float.__add__()");

    cfloat_t res = ((VSFloatObject *)self)->_value + ((VSFloatObject *)that)->_value;
    INCREF_RET(C_FLOAT_TO_FLOAT(res));
}

VSObject *vs_float_sub(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("float.__sub__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_FLOAT, "float.__sub__()");
    ENSURE_TYPE(that, T_FLOAT, "float.__sub__()");

    cfloat_t res = ((VSFloatObject *)self)->_value - ((VSFloatObject *)that)->_value;
    INCREF_RET(C_FLOAT_TO_FLOAT(res));
}

VSObject *vs_float_mul(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("float.__mul__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_FLOAT, "float.__mul__()");
    ENSURE_TYPE(that, T_FLOAT, "float.__mul__()");

    cfloat_t res = ((VSFloatObject *)self)->_value * ((VSFloatObject *)that)->_value;
    INCREF_RET(C_FLOAT_TO_FLOAT(res));
}

VSObject *vs_float_div(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("float.__div__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_FLOAT, "float.__div__()");
    ENSURE_TYPE(that, T_FLOAT, "float.__div__()");

    if (((VSFloatObject *)that)->_value == 0) {
        err("divided by zero\n");
        terminate(TERM_ERROR);
    }

    cfloat_t res = ((VSFloatObject *)self)->_value / ((VSFloatObject *)that)->_value;
    INCREF_RET(C_FLOAT_TO_FLOAT(res));
}

VSObject *vs_float_bool(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("float.__bool__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_FLOAT, "float.__bool__()");

    cfloat_t res = ((VSFloatObject *)self)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_float_char(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("float.__char__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_FLOAT, "float.__char__()");

    cfloat_t res = ((VSFloatObject *)self)->_value;
    INCREF_RET(C_CHAR_TO_CHAR((cchar_t)res));
}

VSObject *vs_float_int(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("float.__int__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_FLOAT, "float.__int__()");

    cfloat_t res = ((VSFloatObject *)self)->_value;
    INCREF_RET(C_INT_TO_INT((cint_t)res));
}

VSObject *vs_float_float(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("float.__float__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_FLOAT, "float.__float__()");

    INCREF_RET(self);
}

VSObject *vs_float_str(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("float.__str__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_FLOAT, "float.__str__()");

    cfloat_t val = ((VSFloatObject *)self)->_value;
    INCREF_RET(C_STRING_TO_STRING(std::to_string(val)));
}

VSObject *vs_float_bytes(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("float.__bytes__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_FLOAT, "float.__bytes__()");

    INCREF_RET(VS_NONE);
}

const str_func_map VSFloatObject::vs_float_methods = {
    {ID___hash__, vs_float_hash},
    {ID___lt__, vs_float_lt},
    {ID___gt__, vs_float_gt},
    {ID___le__, vs_float_le},
    {ID___ge__, vs_float_ge},
    {ID___eq__, vs_float_eq},
    {ID___str__, vs_float_str},
    {ID___bytes__, vs_float_bytes},
    {ID___neg__, vs_float_neg},
    {ID___add__, vs_float_add},
    {ID___sub__, vs_float_sub},
    {ID___mul__, vs_float_mul},
    {ID___div__, vs_float_div},
    {ID___bool__, vs_float_bool},
    {ID___char__, vs_float_char},
    {ID___int__, vs_float_int},
    {ID___float__, vs_float_float}
};

VSFloatObject::VSFloatObject(cfloat_t value) : _value(value) {
    this->type = T_FLOAT;
}

VSFloatObject::~VSFloatObject() {
}

bool VSFloatObject::hasattr(std::string &attrname) {
    return vs_float_methods.find(attrname) != vs_float_methods.end();
}

VSObject *VSFloatObject::getattr(std::string &attrname) {
    auto iter = vs_float_methods.find(attrname);
    if (iter == vs_float_methods.end()) {
        ERR_NO_ATTR(this, attrname);
        terminate(TERM_ERROR);
    }

    VSFunctionObject *attr = new VSNativeFunctionObject(
        this, C_STRING_TO_STRING(attrname), vs_float_methods.at(attrname));
    INCREF_RET(attr);
}

void VSFloatObject::setattr(std::string &attrname, VSObject *attrvalue) {
    err("Unable to apply setattr on native type: \"%s\"", TYPE_STR[this->type]);
    terminate(TERM_ERROR);
}
