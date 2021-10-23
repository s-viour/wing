wing is an experimental c/c++ build system designed to bring together many of the existing and established tools for c/c++ development. heads up: it's pretty barebones right now.

# usage
a wing project is a directory containing a few required items:
* a `wing.toml` file with this format:
```
name = "project name"
dependencies = ["dep1", "dep2"]
```
* a `src` directory (supported extensions: `.cpp`, `.cxx`, `.cc`)
* an `include` directory


wing currently has 4 commands:
* `new` - creates a new wing project with the specified name
* `build` - attempts to build the current folder as a wing project
* `install` - installs all dependencies specified in the `wing.toml` file
* `vcpkg-init` - initializes [vcpkg](https://vcpkg.io/en/index.html) in the project folder. **this command must be run before dependencies can be added!**

# building
to build wing, you'll need a copy of... wing! the [releases page](https://github.com/s-viour/wing/releases) will have the latest binary you can use. simply enter the project directory and run:
1. `wing init-vcpkg`
2. `wing install`
3. `wing build`

steps 1 and 2 only have to be done once to initialize vcpkg and install wing's dependencies

# contributing
suggestions, forks, and pull requests are all welcome! suggestions, questions, and the like may be filed to the [issues](https://github.com/s-viour/wing/issues) page.
