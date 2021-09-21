# Developing for the fx-CP400
You'll need your SH4 cross compiler to develop applications for the fx-CP400. If you patched your firmware, you'll already have done this. If not, check the instructions in [this](patching.md) document to build and install one.

## 1. Build the SDK
This only needs to happen once to generate the required object files which will be linked with your application when it's compiled. It isn't necessary to rebuild the SDK itself whenever you change your application code.

`cd` into the `sdk/` directory, and run the `make` command. Ensure the file `sdk.o` is generated - this is the boilerplate code required for your applications to work when compiled.

**Note:** If you get an dependency error, you need to set the environments ([this](patching.md)) to successfully build the sdk.

If you'd like a local copy of the SDK documentation, run the `make docs` command. Open `sdk/doc/index.html` to view them.

## 2. Start your project
Copy the contents of the `app_template/` directory to an empty folder. This will become your project's root directory.

Open `main.cpp` in your favourite text editor, and edit the strings in the `APP_NAME`, `APP_DESCRIPTION`, `APP_AUTHOR` and `APP_VERSION` macro calls to match your application. These are the strings that will be displayed in the launcher when your application is selected. If you don't wish to provide one or more of these fields, simply delete the line (they're all optional).

Similarly, open the `Makefile` and edit the first line (`APP_NAME:=app_template` by default), changing `app_template` to the filename you'd like your generated `.hhk` file to have.

Edit `main.cpp` to your hearts content, and add other `.cpp`, `.c`, and `.s` files as you please. The `Makefile` will automatically detect new source files at compile time.

## 3. Get ready to build
You'll need to point the Makefile to the SDK using the `SDK_DIR` environment variable. For example, if you cloned this repository to `~/hollyhock/`, set `SDK_DIR` to `~/hollyhock/sdk/`. You can do this either by exporting the variable before running any `make` commands, or setting the variable in the same command as you run make. It's recommended you make this an absolute path and not a relative one - otherwise you may run into issues if you attempt to call `make` while not in the same directory as the one you defined `SDK_DIR` relative to.

```sh
# Either do this once, at the start of every session (recommended):
export SDK_DIR=~/hollyhock/sdk/

# Or, run make like this:
SDK_DIR=~/hollyhock/sdk/ make
```

## 4. Build your app!
Run `make` (remembering to set `SDK_DIR` inline if you didn't `export` it).

A `.hhk` file with the name you specified in the `Makefile` will be generated, which you can then copy onto the root directory of the calculator's flash.

Open the launcher and select your application to launch it. Have fun!
