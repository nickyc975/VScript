#include "objects/VSBoolObject.hpp"

#include "error.hpp"
#include "objects/VSCharObject.hpp"
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
NEW_IDENTIFIER(__not__);
NEW_IDENTIFIER(__and__);
NEW_IDENTIFIER(__or__);
NEW_IDENTIFIER(__xor__);
NEW_IDENTIFIER(__bool__);
NEW_IDENTIFIER(__char__);
NEW_IDENTIFIER(__int__);
NEW_IDENTIFIER(__float__);

VSBoolObject *VSBoolObject::_VS_TRUE = NULL;
VSBoolObject *VSBoolObject::_VS_FALSE = NULL;

VSObject *vs_bool(VSObject *, VSObject *const *args, vs_size_t nargs) {
    if (nargs == 0) {
        INCREF_RET(VS_FALSE);
    }

    if (nargs == 1) {
        VSObject *obj = args[0];
        VSObject *val = CALL_ATTR(obj, ID___bool__, EMPTY_TUPLE());
        if (!IS_TYPE(val, T_BOOL)) {
            err("%s.__bool__() returned \"%s\" instead of bool.", TYPE_STR[obj->type], TYPE_STR[val->type]);
            terminate(TERM_ERROR);
        }

        INCREF_RET(val);
    }

    ERR_NARGS("bool()", 1, nargs);
    terminate(TERM_ERROR);
}

VSObject *vs_bool_hash(VSObject *self, VSObject *const *, vs_size_t nargs) {
    ENSURE_TYPE(self, T_BOOL, "bool.__hash__()");

    if (nargs != 0) {
        ERR_NARGS("bool.__hash__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    INCREF_RET(C_INT_TO_INT((cint_t)BOOL_TO_C_BOOL(self)));
}

VSObject *vs_bool_lt(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("bool.__lt__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[1];
    ENSURE_TYPE(self, T_BOOL, "bool.__lt__()");
    ENSURE_TYPE(that, T_BOOL, "bool.__lt__()");

    bool res = ((VSBoolObject *)self)->_value < ((VSBoolObject *)that)->_value;
    INCREF_RET(C_BOOL_TO_BOOL(res));
}

VSObject *vs_bool_gt(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("bool.__gt__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[1];
    ENSURE_TYPE(self, T_BOOL, "bool.__gt__()");
    ENSURE_TYPE(that, T_BOOL, "bool.__gt__()");

    bool res = ((VSBoolObject *)self)->_value > ((VSBoolObject *)that)->_value;
    INCREF_RET(C_BOOL_TO_BOOL(res));
}

VSObject *vs_bool_le(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("bool.__le__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[1];
    ENSURE_TYPE(self, T_BOOL, "bool.__le__()");
    ENSURE_TYPE(that, T_BOOL, "bool.__le__()");

    bool res = ((VSBoolObject *)self)->_value <= ((VSBoolObject *)that)->_value;
    INCREF_RET(C_BOOL_TO_BOOL(res));
}

VSObject *vs_bool_ge(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("bool.__ge__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[1];
    ENSURE_TYPE(self, T_BOOL, "bool.__ge__()");
    ENSURE_TYPE(that, T_BOOL, "bool.__ge__()");

    bool res = ((VSBoolObject *)self)->_value >= ((VSBoolObject *)that)->_value;
    INCREF_RET(C_BOOL_TO_BOOL(res));
}

VSObject *vs_bool_eq(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("bool.__eq__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[1];
    ENSURE_TYPE(self, T_BOOL, "bool.__eq__()");
    ENSURE_TYPE(that, T_BOOL, "bool.__eq__()");

    bool res = ((VSBoolObject *)self)->_value == ((VSBoolObject *)that)->_value;
    INCREF_RET(C_BOOL_TO_BOOL(res));
}

VSObject *vs_bool_str(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("bool.__str__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_BOOL, "bool.__str__()");

    INCREF_RET(C_STRING_TO_STRING(BOOL_TO_C_BOOL(self) ? "true" : "false"));
}

VSObject *vs_bool_bytes(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("bool.__bytes__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_BOOL, "bool.__bytes__()");

    INCREF_RET(VS_NONE);
}

VSObject *vs_bool_not(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("bool.__not__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_BOOL, "bool.__not__()");

    INCREF_RET(
        C_BOOL_TO_BOOL(
            !BOOL_TO_C_BOOL(self)));
}

VSObject *vs_bool_and(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("bool.__and__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[1];
    ENSURE_TYPE(self, T_BOOL, "bool.__and__()");
    ENSURE_TYPE(that, T_BOOL, "bool.__and__()");

    bool res = ((VSBoolObject *)self)->_value && ((VSBoolObject *)that)->_value;
    INCREF_RET(C_BOOL_TO_BOOL(res));
}

VSObject *vs_bool_or(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("bool.__or__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[1];
    ENSURE_TYPE(self, T_BOOL, "bool.__or__()");
    ENSURE_TYPE(that, T_BOOL, "bool.__or__()");

    bool res = ((VSBoolObject *)self)->_value || ((VSBoolObject *)that)->_value;
    INCREF_RET(C_BOOL_TO_BOOL(res));
}

VSObject *vs_bool_xor(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("bool.__xor__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[1];
    ENSURE_TYPE(self, T_BOOL, "bool.__xor__()");
    ENSURE_TYPE(that, T_BOOL, "bool.__xor__()");

    bool res = ((VSBoolObject *)self)->_value ^ ((VSBoolObject *)that)->_value;
    INCREF_RET(C_BOOL_TO_BOOL(res));
}

VSObject *vs_bool_bool(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("bool.__bool__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_BOOL, "bool.__bool__()");

    INCREF_RET(self);
}

VSObject *vs_bool_char(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("bool.__char__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_BOOL, "bool.__char__()");

    INCREF_RET(C_CHAR_TO_CHAR((cchar_t)BOOL_TO_C_BOOL(self)));
}

VSObject *vs_bool_int(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("bool.__int__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_BOOL, "bool.__int__()");

    INCREF_RET(C_INT_TO_INT((cint_t)BOOL_TO_C_BOOL(self)));
}

VSObject *vs_bool_float(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("bool.__float__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_BOOL, "bool.__float__()");

    INCREF_RET(C_FLOAT_TO_FLOAT((cfloat_t)BOOL_TO_C_BOOL(self)));
}

const str_func_map VSBoolObject::vs_bool_methods = {
    {ID___hash__, vs_bool_hash},
    {ID___lt__, vs_bool_lt},
    {ID___gt__, vs_bool_gt},
    {ID___le__, vs_bool_le},
    {ID___ge__, vs_bool_ge},
    {ID___eq__, vs_bool_eq},
    {ID___str__, vs_bool_str},
    {ID___bytes__, vs_bool_bytes},
    {ID___not__, vs_bool_not},
    {ID___and__, vs_bool_and},
    {ID___or__, vs_bool_or},
    {ID___xor__, vs_bool_xor},
    {ID___bool__, vs_bool_bool},
    {ID___char__, vs_bool_char},
    {ID___int__, vs_bool_int},
    {ID___float__, vs_bool_float}
};

VSBoolObject::VSBoolObject(cbool_t value) : _value(value) {
    this->type = T_BOOL;
}

bool VSBoolObject::hasattr(std::string &attrname) {
    return vs_bool_methods.find(attrname) != vs_bool_methods.end();
}

VSObject *VSBoolObject::getattr(std::string &attrname) {
    auto iter = vs_bool_methods.find(attrname);
    if (iter == vs_bool_methods.end()) {
        ERR_NO_ATTR(this, attrname);
        terminate(TERM_ERROR);
    }

    VSFunctionObject *attr = new VSNativeFunctionObject(
        this, C_STRING_TO_STRING(attrname), vs_bool_methods.at(attrname));
    INCREF_RET(attr);
}

void VSBoolObject::setattr(std::string &attrname, VSObject *attrvalue) {
    err("Unable to apply setattr on native type: \"%s\"", TYPE_STR[this->type]);
    terminate(TERM_ERROR);
}