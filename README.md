# PostgreSQL `semver` Extension

This project implements a PostgreSQL extension in C that adds a `semver` data type for semantic versions in the form `major.minor.patch`, such as `1.2.3`.

The extension supports parsing, output, comparison operators, utility functions, `MIN` and `MAX` aggregates, B-tree indexing, and explicit casts to and from `text`.

## System Requirements

The project was developed for PostgreSQL 16 on Linux.

Required packages:

- `postgresql-16`
- `postgresql-server-dev-16`
- `build-essential`
- `make`
- `gcc`

On Ubuntu, install them with:

```bash
sudo apt update
sudo apt install postgresql-16 postgresql-server-dev-16 build-essential
```

Make sure PostgreSQL is running:

```bash
sudo service postgresql start
```

## Build and Install

From the project directory:

```bash
make clean
make
sudo make install
```

Create the extension in a database:

```bash
sudo -u postgres psql mydb
```

Then inside `psql`:

```sql
CREATE EXTENSION semver;
```

If you are reinstalling after code changes, recreate the extension:

```sql
DROP EXTENSION IF EXISTS semver CASCADE;
CREATE EXTENSION semver;
```

## Regression Tests

Run:

```bash
sudo -u postgres make installcheck
```

The regression test is defined by:

- `sql/semver_test.sql`
- `expected/semver_test.out`

It checks input/output, malformed input rejection, all six comparison operators, utility functions, ordering, aggregates, a combined compatibility query, text casts, and B-tree index creation.

## Demo

Run:

```bash
psql mydb -f demo.sql
```

The demo creates sample package versions, sorts them, queries version ranges, uses aggregates and utility functions, demonstrates casts, and shows an indexed range query with `EXPLAIN`.

## Supported Type

- `semver`: stores a semantic version as three 32-bit integer fields: `major`, `minor`, and `patch`.

Valid input format:

```text
major.minor.patch
```

Example:

```sql
SELECT '1.2.3'::semver;
```

## Supported Casts

- `text AS semver`
- `semver AS text`

Examples:

```sql
SELECT '2.3.4'::text::semver;
SELECT '2.3.4'::semver::text;
```

## Supported Functions

- `major(semver) RETURNS int4`
- `minor(semver) RETURNS int4`
- `patch(semver) RETURNS int4`
- `bump_minor(semver) RETURNS semver`
- `is_compatible(semver, semver) RETURNS bool`

Examples:

```sql
SELECT major('1.2.3'::semver);
SELECT bump_minor('1.2.3'::semver);
SELECT is_compatible('1.5.0'::semver, '1.4.2'::semver);
```

## Supported Operators

The extension supports all six comparison operators:

- `<`
- `<=`
- `=`
- `<>`
- `>=`
- `>`

Example:

```sql
SELECT '1.10.0'::semver > '1.9.0'::semver;
```

This returns `true`, proving comparison is numeric field-by-field rather than string-based.

## Supported Aggregates

- `MAX(semver)`
- `MIN(semver)`

Example:

```sql
SELECT MAX(version), MIN(version)
FROM packages;
```

## Indexing Bonus

The extension registers a default B-tree operator class:

```sql
CREATE OPERATOR CLASS semver_ops
DEFAULT FOR TYPE semver USING btree
```

This allows:

```sql
CREATE INDEX packages_version_idx
ON packages USING btree (version);
```

The demo includes an `EXPLAIN` query to show index usage.

## Known Limitations
- The parser expects exactly three numeric components.
- The current parser uses signed integers and does not explicitly reject negative components.
- The extension stores only `major`, `minor`, and `patch`; it does not preserve original input formatting.
