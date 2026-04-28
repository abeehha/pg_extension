#include "postgres.h"
#include "fmgr.h"

PG_MODULE_MAGIC;

typedef struct {
    int32 major;
    int32 minor;
    int32 patch;
} Semver;

PG_FUNCTION_INFO_V1(semver_in);

Datum semver_in(PG_FUNCTION_ARGS) {
    char *str = PG_GETARG_CSTRING(0);

    int major, minor, patch;
    char extra;

    if (sscanf(str, "%d.%d.%d%c", &major, &minor, &patch, &extra) != 3) {
    ereport(ERROR,
        (errmsg("invalid input syntax for type semver: \"%s\"", str)));
    }

    Semver *result = (Semver *) palloc(sizeof(Semver));
    result->major = major;
    result->minor = minor;
    result->patch = patch;

    PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(semver_out);

Datum semver_out(PG_FUNCTION_ARGS) {
    Semver *sv = (Semver *) PG_GETARG_POINTER(0);

    char *result = (char *) palloc(32);

    snprintf(result, 32, "%d.%d.%d",
             sv->major, sv->minor, sv->patch);

    PG_RETURN_CSTRING(result);
}