# allgair-bachelor

## Build the software
1. Navigate into the project directory
2. Create a `build` directory
```bash
mkdir build
```
3. Navigate into the `build` directory
```bash
cd build
```
4. Compile
```bash
cmake ..
make
```
5. Navigate back into the root project directory
```
cd ..
```

There should now be a file with a name similar to `raytracing.*.so`. This file should be placed inside the directory in which you wish to include it into your Python scripts.
