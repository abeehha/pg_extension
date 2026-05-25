DROP TABLE IF EXISTS packages;

DROP EXTENSION IF EXISTS semver CASCADE;

CREATE EXTENSION semver;

CREATE TABLE packages(
    name text,
    version semver
);

INSERT INTO packages VALUES
('postgres', '16.0.0'),
('node', '20.5.1'),
('react', '18.2.0'),
('vue', '3.4.0'),
('django', '5.0.1'),
('flask', '3.0.0'),
('numpy', '2.1.0'),
('pandas', '2.2.1'),
('express', '5.0.0'),
('docker', '27.0.1');

SELECT * FROM packages;

SELECT * FROM packages
ORDER BY version;

SELECT MAX(version)
FROM packages;

SELECT MIN(version)
FROM packages;

SELECT name, version
FROM packages
WHERE version >= '5.0.0';

CREATE INDEX packages_version_idx
ON packages USING btree (version);

SET enable_seqscan = off;

EXPLAIN SELECT name, version
FROM packages
WHERE version >= '5.0.0'::semver
ORDER BY version;

RESET enable_seqscan;

SELECT bump_minor('1.2.3'::semver);

SELECT '2.3.4'::text::semver AS cast_to_semver;

SELECT '2.3.4'::semver::text AS cast_to_text;

SELECT is_compatible(
    '1.5.0'::semver,
    '1.4.2'::semver
);
