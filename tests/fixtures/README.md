# Test fixtures

`minimal.qsp` is a minimal QSP game loaded by `tst_engine`. It is generated
from `minimal.txt` with the txt2gam tool of the upstream 60f0e9d tree (its
helpers are self-contained; the legacy engine reports QSP_VER 5.7.0 for the
generated file):

    cc -std=gnu11 -I <upstream>/txt2gam/src -o txt2gam \
        <upstream>/txt2gam/src/*.c $(pkg-config --cflags --libs oniguruma)
    ./txt2gam minimal.txt minimal.qsp

The game defines one location `start` whose body runs STRCOMP/STRFIND
checks and prints a description containing `Desc:`. Note: the location body
of this 2013-era output does not survive the legacy parse (the loaded world
has the location but no executable code), so `tst_engine` exercises the
engine through `QSPExecString` instead of relying on location execution.
