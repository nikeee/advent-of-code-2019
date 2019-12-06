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
    transitive_closure(hops, center, satellite) AS (
            select 0, center, satellite from orbits
        union all
            select hops + 1, A.center, B.satellite
            from transitive_closure as A join orbits as B
            on A.satellite = B.center
            where hops < (select (select x from iterations) * (select x from iterations))
            order by 1 asc
    )
    select hops, center as source, satellite as dest from transitive_closure order by 1, 2;

select count(*) as answer_part_1 from transitive_orbital_closure;

-- Finding the shortest path from YOU to SAN is more tricky, since the orbit relation is directed.
-- Making the orbit relation undirected would cause the transitive closure to increase in size dramatically.

-- We use a simple trick:
-- Let's pretend YOU and SAN want to meet. They both are in the "satellite" column. So if they are able to meet (meaning, there is a path connecting the two),
-- then they would have a common target that will be in the "center" column. This would be the node where the two *could* meet.
-- This means there are (at least) to entries in the transitive closure. One for SAN and one for YOU, woth with their own name in "satellite"/"dest" and a common planet in the "center"/"source" column.
-- We can use that information to join on the transitive closure.

-- The hops needed to go from YOU to SAN are same as the hops that YOU and SAN need from their position to the common point.

-- Turns out, there are indeed multiple paths they could take.
-- We just select all of them and take the one with the minimum hop count. This is the answer to part 2.

create view paths_from_you_to_santa
as
    select
    *,
    (my_part.hops + santas_part.hops) as path_length
    from
        (select * from transitive_orbital_closure where dest = 'YOU') as my_part
    join
        (select * from transitive_orbital_closure where dest = 'SAN') as santas_part
    on my_part.source = santas_part.source
    order by my_part.hops + santas_part.hops asc;

select min(path_length) as answer_part_2 from paths_from_you_to_santa;
