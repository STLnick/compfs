Storage=Local
(test program names that work)
(test programs that don't work - what happens)

# Code Generation

## Compilers - P4

**Version Control**:
GitHub repo: https://github.com/STLnick/compfs

Cloned from [parser](https://github.com/STLnick/compfs) to get all of that code to begin.

---

**To run the program**:
- Run `make` in the root of the project to build `compfs`
    - The executable `compfs` is built and placed in `/bin`.
- Execute `cd ./bin`
- Run `./compfs` with a file containing the `.fs` extension (example below)

**Run command structure**:

> `[file]` _must have the extension of_ `.fs` or no extension at all.
> - for example `test1.fs`, `text.fs`, `whatever.fs`, `filename`

`./compfs [file]` or `./compfs [file].fs`
    - **NOTE**: The files to be read from _MUST BE IN THE_ `/bin` directory with the executable

`./compfs -h`

- help

TODO: Maybe replace examples??

### Simple example of a valid, parsable program
`p2g1.fs` of provided test files

```
$$ p3g1 $$
data x := 5 ;
main
begin
data y := 67 ;
outter y ;
assign x := y ;
end
```

### Complex example of a valid, parsable program
`p2g6.fs` of provided test files

```
$$ p3g2 $$
data z := 321 ;
data y := 567 ;
main
begin
data x := 5 ;
loop [ y => z ]
  begin
  data c := 3 ;
     begin
     proc c ; 
     end
  end ;
outter x ;
end
```

### Semantics
Essentially following the rules of `C` for semantic rules.


---

**What Works**:

**What Doesn't Work**:
- We'll find out...

---
### Commit Log


**All other commits and associated code preceding are from `statSem` project and can be accessed at that repo linked above**

