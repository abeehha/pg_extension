#include "postgres.h"
#include "fmgr.h"

#include <stdio.h>

PG_MODULE_MAGIC;

typedef struct
{
    int32 major;
    int32 minor;
    int32 patch;
} Semver;

int semver_compare(Semver *a, Semver *b)
{
    if (a->major != b->major)
        return a->major - b->major;

    if (a->minor != b->minor)
        return a->minor - b->minor;

    return a->patch - b->patch;
}

PG_FUNCTION_INFO_V1(semver_in);
Datum
semver_in(PG_FUNCTION_ARGS)
{
    char *str = PG_GETARG_CSTRING(0);

    int major;
    int minor;
    int patch;

    char extra;

    if (sscanf(str,
               "%d.%d.%d%c",
               &major,
               &minor,
               &patch,
               &extra) != 3)
    {
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
Datum
semver_out(PG_FUNCTION_ARGS)
{
    Semver *sv = (Semver *) PG_GETARG_POINTER(0);

    char *result = (char *) palloc(32);

    snprintf(result,
             32,
             "%d.%d.%d",
             sv->major,
             sv->minor,
             sv->patch);

    PG_RETURN_CSTRING(result);
}

PG_FUNCTION_INFO_V1(semver_lt);
Datum
semver_lt(PG_FUNCTION_ARGS)
{
    Semver *a = (Semver *) PG_GETARG_POINTER(0);
    Semver *b = (Semver *) PG_GETARG_POINTER(1);

    PG_RETURN_BOOL(semver_compare(a, b) < 0);
}

PG_FUNCTION_INFO_V1(semver_le);
Datum
semver_le(PG_FUNCTION_ARGS)
{
    Semver *a = (Semver *) PG_GETARG_POINTER(0);
    Semver *b = (Semver *) PG_GETARG_POINTER(1);

    PG_RETURN_BOOL(semver_compare(a, b) <= 0);
}

PG_FUNCTION_INFO_V1(semver_eq);
Datum
semver_eq(PG_FUNCTION_ARGS)
{
    Semver *a = (Semver *) PG_GETARG_POINTER(0);
    Semver *b = (Semver *) PG_GETARG_POINTER(1);

    PG_RETURN_BOOL(semver_compare(a, b) == 0);
}

PG_FUNCTION_INFO_V1(semver_ne);
Datum
semver_ne(PG_FUNCTION_ARGS)
{
    Semver *a = (Semver *) PG_GETARG_POINTER(0);
    Semver *b = (Semver *) PG_GETARG_POINTER(1);

    PG_RETURN_BOOL(semver_compare(a, b) != 0);
}

PG_FUNCTION_INFO_V1(semver_ge);
Datum
semver_ge(PG_FUNCTION_ARGS)
{
    Semver *a = (Semver *) PG_GETARG_POINTER(0);
    Semver *b = (Semver *) PG_GETARG_POINTER(1);

    PG_RETURN_BOOL(semver_compare(a, b) >= 0);
}

PG_FUNCTION_INFO_V1(semver_gt);
Datum
semver_gt(PG_FUNCTION_ARGS)
{
    Semver *a = (Semver *) PG_GETARG_POINTER(0);
    Semver *b = (Semver *) PG_GETARG_POINTER(1);

    PG_RETURN_BOOL(semver_compare(a, b) > 0);
}

PG_FUNCTION_INFO_V1(semver_cmp);
Datum
semver_cmp(PG_FUNCTION_ARGS)
{
    Semver *a = (Semver *) PG_GETARG_POINTER(0);
    Semver *b = (Semver *) PG_GETARG_POINTER(1);

    PG_RETURN_INT32(semver_compare(a, b));
}

PG_FUNCTION_INFO_V1(major);
Datum
major(PG_FUNCTION_ARGS)
{
    Semver *v = (Semver *) PG_GETARG_POINTER(0);

    PG_RETURN_INT32(v->major);
}

PG_FUNCTION_INFO_V1(minor);
Datum
minor(PG_FUNCTION_ARGS)
{
    Semver *v = (Semver *) PG_GETARG_POINTER(0);

    PG_RETURN_INT32(v->minor);
}

PG_FUNCTION_INFO_V1(patch);
Datum
patch(PG_FUNCTION_ARGS)
{
    Semver *v = (Semver *) PG_GETARG_POINTER(0);

    PG_RETURN_INT32(v->patch);
}

PG_FUNCTION_INFO_V1(bump_minor);
Datum
bump_minor(PG_FUNCTION_ARGS)
{
    Semver *old = (Semver *) PG_GETARG_POINTER(0);

    Semver *new_version =
        (Semver *) palloc(sizeof(Semver));

    new_version->major = old->major;
    new_version->minor = old->minor + 1;
    new_version->patch = 0;

    PG_RETURN_POINTER(new_version);
}

PG_FUNCTION_INFO_V1(is_compatible);
Datum
is_compatible(PG_FUNCTION_ARGS)
{
    Semver *a = (Semver *) PG_GETARG_POINTER(0);
    Semver *b = (Semver *) PG_GETARG_POINTER(1);

    bool compatible =
        (a->major == b->major) &&
        (semver_compare(b, a) <= 0);

    PG_RETURN_BOOL(compatible);
}
