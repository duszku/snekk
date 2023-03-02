# Snekk

this is basically just tui snake idek what more to say.

## stuff you need to compile it

- a C compiler
- curses
- my [libfuncc](https://github.com/duszku/libfuncc) library
- a unix-like os with posix threads library

even though Makefile uses gcc by default, changing it to any other compiler
should still work, at least for the release target

then you just go

```shell
make
```

and thats basically it

## controls

**wasd** for movement, **x** to end the game

## PROCEED WITH CAUTION

<pre>
                                ..^*+++++++^^^==;;**************
                          .,;^**++++*+===++*;^+=,;**************
                      .;*+======$=========+++^;^,;**************
                    ;+===$$$$$$$$$=++=======++^;.;**************
               .,.,+==$$$$$$$$$$$$$=^*+++$&&&+^;.;**************
            .;*+^*+==$$$$$$$$$$$$$$&&;^***++++^;.;**************
          ,*=++++====$$$$$$$=====&&&=*++^^****^;.;**************
       .;+===++$$$$===$$$$=+++++++=*;++*^=$+***^.;**************
     ,*$$$$$==$$$$$$+*&$&&=&++++++++*;;=&&&$$+*^.;**************
  .^=$$$$$$$$$$$$$$++^+=+$=++++++++++*^$$=+*^,^^,,**************
;+$$&&&&&$$$$$$&&$=*+^+$=,*+++++++++*^**       ,.,**************
$&&&&&&&$=++$$$@$++***&=;;++++++====*;;..*,,;,,,.;**************
=$$&$$$=++^+$&&&$*++*+**+**++=====$$^**,=$^=@,.,.;*^************
...,;^^**^;+$$$$$+++^$&&@&=+*+==$$$$^==+$$+,;;^*^;^^^^^*********
,,,,#@$=++;+$$$$$=++^$&$=+*+++***+=+*$$$$@@$;^+$;,^^^^^^^^^*****
.. *@@@$$=,*$$$$$$+*;;, ..   .;*$=++=&#@&@@#@+;^,,^^^^^^^^^^^***
   ;;;;*$=,^+**+$$=+, .++,,....;$@&$&#####@@&@&$,,^^^^^^^^^^^^^*
   ;,,,,,..;*@#+*++^. ^$*&#^..;;;$#########@@&$$,,^^^^^^^^^^^^^*
   .....=@+,$##&*++++,^$=;;;;;*&*^@#########@@&$,,^^^^^^^^^^^^^^
       .^@@+$$&$*+++$=;=#@+;*$&=^*@##########@@&,,^^^^^^^^^^^^^^
.........*=^;^*^^*+++=$=@@@@=^^+&######$*+$@#@@&,;^^^^^^^^^^^^^^
........    +,*$^;*==++=+=$@&@&&@######&^+=*$@@&,;^^^^^^^^^^^^^^
           ^=..**;;*++===++++=$&&@#@@@@@@$=$$@@&.,^^^^^^^^^^^^^^
          ,^^..,;^;,;;;;^^^*+$$$@@@@@@@@@@@@@@&$,,^^^^^^^^^^^^^^
..........,;^. .,;;;^**,,=$$&&&@@@@@@@@@@@@@@$*;.,;;;^^^^^^^^^^^
..........  .    ,^*+++*;,;*+==$&&&&&&&$=+*^;;,.;+++^,^^^^^^^^^^
...............   .,;*++++*^;...,,,,,,....,;,.,$@&&&$^;^^^^^^^^^
..................     .,;^*+*;,,;;,;;,..,^*,;&@@@$=+,;^^^^^^^^^
.....;+=$$$$=+*^,...        .^+,..  ..+@=^;+,$@@@@=;,;;;;;;;;;^^
....;$$$+*^^+====*,           ,      .&##$*,,&@@@$;,;;;;;;;;;;;;
.....,,.     .;*+++^.                .&##@&*.$@&+,,;;;;;;;;;;;;;
......          ,^+++,                +##@&=.*&$..;^;;;;;;;;;;;;
</pre>
