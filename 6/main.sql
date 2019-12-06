-- Run:
-- sqlite3 :memory: < input.txt

-- Basically, the input defines a relation where the first char defines the center
-- and the second char defined the satelliting object

-- "direct and indirect orbits" then just refers to the transitive closure on this relation.
-- The puzzle answer is just the cardinality of this transitive closure.

-- Some SQL dialects have the ability to do transitive closures.
-- We use SQLite for that.

create table orbits (
    center text not null,
    satellite text not null,
    primary key(center, satellite)
);

-- SQLite-specific commands used to import the data
.separator ")"
.import input.txt orbits
.separator "\t"

create view transitive_orbital_closure
as
with recursive
iterations(x) AS (
    select count(*) from orbits
),
transitive_closure(iteration_count, center, satellite) AS (
        select 0, center, satellite from orbits
    union all
        select iteration_count + 1, A.center, B.satellite
        from transitive_closure as A join orbits as B
        on A.satellite = B.center
        where iteration_count < (select (select x from iterations) * (select x from iterations))
        order by 1 asc
)
select center as source, satellite as dest from transitive_closure order by 1, 2;

select count(*) as answer_part_1 from transitive_orbital_closure;
