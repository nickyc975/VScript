#include "objects/VSBytesObject.hpp"

#include "error.hpp"
#include "objects/VSFunctionObject.hpp"
#include "objects/VSStringObject.hpp"
#include "objects/VSTupleObject.hpp"

NEW_IDENTIFIER(__bytes__);

VSObject *vs_bytes(VSObject *, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("bytes()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *obj = args[0];
    VSObject *res = CALL_ATTR(obj, ID___bytes__, EMPTY_TUPLE());
    if (!IS_TYPE(res, T_BYTES)) {
        err("%s.__bytes__() returned \"%s\" object instead of bytes", TYPE_STR[obj->type], TYPE_STR[res->type]);
        terminate(TERM_ERROR);
    }
    INCREF_RET(res);
}