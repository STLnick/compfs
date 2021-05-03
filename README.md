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

### Custom test files that were tested and worked:
```
$$ Prints '1' & '2' if x and '*4' are opposite signs (true in its current form) $$
data x := 4 ;
main
begin
data a := 3 ;
if [ x % *4 ] then proc a ; ;
outter 1 ;
void a ;
outter 2 ;
end
```
```
data x := 4 ;
main
begin
loop [ x => 2 ]
    begin
    outter x ;
    assign x := x - 1 ;
    end ;
outter 100 ;
end
```
```
data y := 5 ;
main
begin
data x := 555 ;
loop [ y => 1 ]
  begin
  data c := 4 ;
  assign y := y - 1 ;
  outter y ;
  end ;
outter x ;
end
```
```
data y := 5 ;
data c := 0 ;
main
begin
data x := 555 ;
void c ;
assign y := y - 2 ;
outter y ;
if [ y => 2 ] then proc c ; ;
outter x ;
end
```

### Semantics
Essentially following the rules of `C` for semantic rules.

---

**What Works**:
- Generates code properly
- Uses stack for variables that aren't in global scope
- Respects order of operations
- Any global or temporary variables generated are initialized below STOP instruction

**What Doesn't Work**:
- Not sure if this was something I messed up, however, you cannot define a variable as a negative number
    - Negatives are still handled fine by the VirtMach and generated code, simply no negative definitions (or use * to achieve)

---
### Commit Log


**All other commits and associated code preceding are from `statSem` project and can be accessed at that repo linked above**

