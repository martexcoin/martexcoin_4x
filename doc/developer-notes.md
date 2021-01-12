Developer Notes
===============

<!-- markdown-toc start -->
**Table of Contents**

- [Developer Notes](#developer-notes)
    - [Coding Style (General)](#coding-style-general)
    - [Coding Style (C++)](#coding-style-c)
    - [Coding Style (Python)](#coding-style-python)
    - [Coding Style (Doxygen-compatible comments)](#coding-style-doxygen-compatible-comments)
      - [Generating Documentation](#generating-documentation)
    - [Development tips and tricks](#development-tips-and-tricks)
        - [Compiling for debugging](#compiling-for-debugging)
        - [Compiling for gprof profiling](#compiling-for-gprof-profiling)
        - [`debug.log`](#debuglog)
        - [Testnet and Regtest modes](#testnet-and-regtest-modes)
        - [DEBUG_LOCKORDER](#debug_lockorder)
        - [Valgrind suppressions file](#valgrind-suppressions-file)
        - [Compiling for test coverage](#compiling-for-test-coverage)
        - [Performance profiling with perf](#performance-profiling-with-perf)
        - [Sanitizers](#sanitizers)
    - [Locking/mutex usage notes](#lockingmutex-usage-notes)
    - [Threads](#threads)
    - [Ignoring IDE/editor files](#ignoring-ideeditor-files)
- [Development guidelines](#development-guidelines)
    - [General MarteXcoin Core](#general-martexcoin-core)
    - [Wallet](#wallet)
    - [General C++](#general-c)
    - [C++ data structures](#c-data-structures)
    - [Strings and formatting](#strings-and-formatting)
    - [Shadowing](#shadowing)
    - [Threads and synchronization](#threads-and-synchronization)
    - [Scripts](#scripts)
        - [Shebang](#shebang)
    - [Source code organization](#source-code-organization)
    - [GUI](#gui)
    - [Subtrees](#subtrees)
    - [Upgrading LevelDB](#upgrading-leveldb)
      - [File Descriptor Counts](#file-descriptor-counts)
      - [Consensus Compatibility](#consensus-compatibility)
    - [Scripted diffs](#scripted-diffs)
        - [Suggestions and examples](#suggestions-and-examples)
    - [Release notes](#release-notes)
    - [RPC interface guidelines](#rpc-interface-guidelines)
    - [Internal interface guidelines](#internal-interface-guidelines)

<!-- markdown-toc end -->

Coding Style (General)
----------------------

Various coding styles have been used during the history of the codebase,
and the result is not very consistent. However, we're now trying to converge to
a single style, which is specified below. When writing patches, favor the new
style over attempting to mimic the surrounding style, except for move-only
commits.

Do not submit patches solely to modify the style of existing code.

Coding Style (C++)
------------------

- **Indentation and whitespace rules** as specified in
[src/.clang-format](/src/.clang-format). You can use the provided
[clang-format-diff script](/contrib/devtools/README.md#clang-format-diffpy)
tool to clean up patches automatically before submission.
  - Braces on new lines for classes, functions, methods.
  - Braces on the same line for everything else.
  - 4 space indentation (no tabs) for every block except namespaces.
  - No indentation for `public`/`protected`/`private` or for `namespace`.
  - No extra spaces inside parenthesis; don't do `( this )`.
  - No space after function names; one space after `if`, `for` and `while`.
  - If an `if` only has a single-statement `then`-clause, it can appear
    on the same line as the `if`, without braces. In every other case,
    braces are required, and the `then` and `else` clauses must appear
    correctly indented on a new line.

- **Symbol naming conventions**. These are preferred in new code, but are not
required when doing so would need changes to significant pieces of existing
code.
  - Variable (including function arguments) and namespace names are all lowercase and may use `_` to
    separate words (snake_case).
    - Class member variables have a `m_` prefix.
    - Global variables have a `g_` prefix.
  - Compile-time constant names are all uppercase, and use `_` to separate words.
  - Class names, function names, and method names are UpperCamelCase
    (PascalCase). Do not prefix class names with `C`.
  - Test suite naming convention: The Boost test suite in file
    `src/test/foo_tests.cpp` should be named `foo_tests`. Test suite names
    must be unique.

- **Miscellaneous**
  - `++i` is preferred over `i++`.
  - `nullptr` is preferred over `NULL` or `(void*)0`.
  - `static_assert` is preferred over `assert` where possible. Generally; compile-time checking is preferred over run-time checking.

Block style example:
```c++
int g_count = 0;

namespace foo {
class Class
{
    std::string m_name;

public:
    bool Function(const std::string& s, int n)
    {
        // Comment summarising what this section of code does
        for (int i = 0; i < n; ++i) {
            int total_sum = 0;
            // When something fails, return early
            if (!Something()) return false;
            ...
            if (SomethingElse(i)) {
                total_sum += ComputeSomething(g_count);
            } else {
                DoSomething(m_name, total_sum);
            }
        }

        // Success return is usually at the end
        return true;
    }
}
} // namespace foo
```

Coding Style (Python)
---------------------

Refer to [/test/functional/README.md#style-guidelines](/test/functional/README.md#style-guidelines).

Coding Style (Doxygen-compatible comments)
------------------------------------------

Bitcoin Core uses [Doxygen](http://www.doxygen.nl/) to generate its official documentation.

Use Doxygen-compatible comment blocks for functions, methods, and fields.

For example, to describe a function use:

```c++
/**
 * ... Description ...
 *
 * @param[in]  arg1 input description...
 * @param[in]  arg2 input description...
 * @param[out] arg3 output description...
 * @return Return cases...
 * @throws Error type and cases...
 * @pre  Pre-condition for function...
 * @post Post-condition for function...
 */
bool function(int arg1, const char *arg2, std::string& arg3)
```

A complete list of `@xxx` commands can be found at http://www.doxygen.nl/manual/commands.html.
As Doxygen recognizes the comments by the delimiters (`/**` and `*/` in this case), you don't
*need* to provide any commands for a comment to be valid; just a description text is fine.

To describe a class, use the same construct above the class definition:
```c++
/**
 * Alerts are for notifying old versions if they become too obsolete and
 * need to upgrade. The message is displayed in the status bar.
 * @see GetWarnings()
 */
class CAlert
```

To describe a member or variable use:
```c++
//! Description before the member
int var;
```

or
```c++
int var; //!< Description after the member
```

Also OK:
```c++
///
/// ... Description ...
///
bool function2(int arg1, const char *arg2)
```

Not picked up by Doxygen:
```c++
//
// ... Description ...
//
```

Also not picked up by Doxygen:
```c++
/*
 * ... Description ...
 */
```

A full list of comment syntaxes picked up by Doxygen can be found at http://www.doxygen.nl/manual/docblocks.html,
but the above styles are favored.

Recommendations:

- Avoiding duplicating type and input/output information in function
  descriptions.

- Use backticks (&#96;&#96;) to refer to `argument` names in function and
  parameter descriptions.

- Backticks aren't required when referring to functions Doxygen already knows
  about; it will build hyperlinks for these automatically. See
  http://www.doxygen.nl/manual/autolink.html for complete info.

- Avoid linking to external documentation; links can break.

- Javadoc and all valid Doxygen comments are stripped from Doxygen source code
  previews (`STRIP_CODE_COMMENTS = YES` in [Doxyfile.in](doc/Doxyfile.in)). If
  you want a comment to be preserved, it must instead use `//` or `/* */`.

### Generating Documentation

The documentation can be generated with `make docs` and cleaned up with `make
clean-docs`. The resulting files are located in `doc/doxygen/html`; open
`index.html` in that directory to view the homepage.

Before running `make docs`, you'll need to install these dependencies:

Linux: `sudo apt install doxygen graphviz`

MacOS: `brew install doxygen graphviz`

Development tips and tricks
---------------------------

### Compiling for debugging

Run configure with `--enable-debug` to add additional compiler flags that
produce better debugging builds.

### Compiling for gprof profiling

Run configure with the `--enable-gprof` option, then make.

### `debug.log`

If the code is behaving strangely, take a look in the `debug.log` file in the data directory;
error and debugging messages are written there.

The `-debug=...` command-line option controls debugging; running with just `-debug` or `-debug=1` will turn
on all categories (and give you a very large `debug.log` file).

The Qt code routes `qDebug()` output to `debug.log` under category "qt": run with `-debug=qt`
to see it.

### Testnet and Regtest modes

Run with the `-testnet` option to run with "play bitcoins" on the test network, if you
are testing multi-machine code that needs to operate across the internet.

If you are testing something that can run on one machine, run with the `-regtest` option.
In regression test mode, blocks can be created on-demand; see [test/functional/](/test/functional) for tests
that run in `-regtest` mode.

### DEBUG_LOCKORDER

MarteXcoin Core is a multi-threaded application, and deadlocks or other
multi-threading bugs can be very difficult to track down. The `--enable-debug`
configure option adds `-DDEBUG_LOCKORDER` to the compiler flags. This inserts
run-time checks to keep track of which locks are held and adds warnings to the
`debug.log` file if inconsistencies are detected.

### Valgrind suppressions file

Valgrind is a programming tool for memory debugging, memory leak detection, and
profiling. The repo contains a Valgrind suppressions file
([`valgrind.supp`](https://github.com/bitcoin/bitcoin/blob/master/contrib/valgrind.supp))
which includes known Valgrind warnings in our dependencies that cannot be fixed
in-tree. Example use:

```shell
$ valgrind --suppressions=contrib/valgrind.supp src/test/test_bitcoin
$ valgrind --suppressions=contrib/valgrind.supp --leak-check=full \
      --show-leak-kinds=all src/test/test_bitcoin --log_level=test_suite
$ valgrind -v --leak-check=full src/bitcoind -printtoconsole
$ ./test/functional/test_runner.py --valgrind
```

### Compiling for test coverage

LCOV can be used to generate a test coverage report based upon `make check`
execution. LCOV must be installed on your system (e.g. the `lcov` package
on Debian/Ubuntu).

To enable LCOV report generation during test runs:

```shell
./configure --enable-lcov
make
make cov

# A coverage report will now be accessible at `./test_bitcoin.coverage/index.html`.
```

### Performance profiling with perf

Profiling is a good way to get a precise idea of where time is being spent in
code. One tool for doing profiling on Linux platforms is called
[`perf`](http://www.brendangregg.com/perf.html), and has been integrated into
the functional test framework. Perf can observe a running process and sample
(at some frequency) where its execution is.

Perf installation is contingent on which kernel version you're running; see
[this thread](https://askubuntu.com/questions/50145/how-to-install-perf-monitoring-tool)
for specific instructions.

Certain kernel parameters may need to be set for perf to be able to inspect the
running process's stack.

```sh
$ sudo sysctl -w kernel.perf_event_paranoid=-1
$ sudo sysctl -w kernel.kptr_restrict=0
```

Make sure you [understand the security
trade-offs](https://lwn.net/Articles/420403/) of setting these kernel
parameters.

To profile a running bitcoind process for 60 seconds, you could use an
invocation of `perf record` like this:

```sh
$ perf record \
    -g --call-graph dwarf --per-thread -F 140 \
    -p `pgrep bitcoind` -- sleep 60
```

You could then analyze the results by running:

```sh
perf report --stdio | c++filt | less
```

or using a graphical tool like [Hotspot](https://github.com/KDAB/hotspot).

See the functional test documentation for how to invoke perf within tests.


### Sanitizers

Bitcoin Core can be compiled with various "sanitizers" enabled, which add
instrumentation for issues regarding things like memory safety, thread race
conditions, or undefined behavior. This is controlled with the
`--with-sanitizers` configure flag, which should be a comma separated list of
sanitizers to enable. The sanitizer list should correspond to supported
`-fsanitize=` options in your compiler. These sanitizers have runtime overhead,
so they are most useful when testing changes or producing debugging builds.

Some examples:

```bash
# Enable both the address sanitizer and the undefined behavior sanitizer
./configure --with-sanitizers=address,undefined

# Enable the thread sanitizer
./configure --with-sanitizers=thread
```

If you are compiling with GCC you will typically need to install corresponding
"san" libraries to actually compile with these flags, e.g. libasan for the
address sanitizer, libtsan for the thread sanitizer, and libubsan for the
undefined sanitizer. If you are missing required libraries, the configure script
will fail with a linker error when testing the sanitizer flags.

The test suite should pass cleanly with the `thread` and `undefined` sanitizers,
but there are a number of known problems when using the `address` sanitizer. The
address sanitizer is known to fail in
[sha256_sse4::Transform](/src/crypto/sha256_sse4.cpp) which makes it unusable
unless you also use `--disable-asm` when running configure. We would like to fix
sanitizer issues, so please send pull requests if you can fix any errors found
by the address sanitizer (or any other sanitizer).

Not all sanitizer options can be enabled at the same time, e.g. trying to build
with `--with-sanitizers=address,thread` will fail in the configure script as
these sanitizers are mutually incompatible. Refer to your compiler manual to
learn more about these options and which sanitizers are supported by your
compiler.

Additional resources:

 * [AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html)
 * [LeakSanitizer](https://clang.llvm.org/docs/LeakSanitizer.html)
 * [MemorySanitizer](https://clang.llvm.org/docs/MemorySanitizer.html)
 * [ThreadSanitizer](https://clang.llvm.org/docs/ThreadSanitizer.html)
 * [UndefinedBehaviorSanitizer](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html)
 * [GCC Instrumentation Options](https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html)
 * [Google Sanitizers Wiki](https://github.com/google/sanitizers/wiki)
 * [Issue #12691: Enable -fsanitize flags in Travis](https://github.com/bitcoin/bitcoin/issues/12691)

Locking/mutex usage notes
-------------------------

The code is multi-threaded and uses mutexes and the
`LOCK` and `TRY_LOCK` macros to protect data structures.

Deadlocks due to inconsistent lock ordering (thread 1 locks `cs_main` and then
`cs_wallet`, while thread 2 locks them in the opposite order: result, deadlock
as each waits for the other to release its lock) are a problem. Compile with
`-DDEBUG_LOCKORDER` (or use `--enable-debug`) to get lock order inconsistencies
reported in the `debug.log` file.

Re-architecting the core code so there are better-defined interfaces
between the various components is a goal, with any necessary locking
done by the components (e.g. see the self-contained `FillableSigningProvider` class
and its `cs_KeyStore` lock for example).

Threads
-------

- ThreadScriptCheck : Verifies block scripts.

- ThreadImport : Loads blocks from blk*.dat files or bootstrap.dat.

- StartNode : Starts other threads.

- ThreadDNSAddressSeed : Loads addresses of peers from the DNS.

- ThreadMapPort : Universal plug-and-play startup/shutdown

- ThreadSocketHandler : Sends/Receives data from peers on port 9999.

- ThreadOpenAddedConnections : Opens network connections to added nodes.

- ThreadOpenConnections : Initiates new connections to peers.

- ThreadMessageHandler : Higher-level message handling (sending and receiving).

- DumpAddresses : Dumps IP addresses of nodes to peers.dat.

- ThreadFlushWalletDB : Close the wallet.dat file if it hasn't been used in 500ms.

- ThreadRPCServer : Remote procedure call handler, listens on port 9998 for connections and services them.

- BitcoinMiner : Generates coins (if wallet is enabled).

- ThreadCheckDarkSendPool : Runs masternode list and sync data update loops

- Shutdown : Does an orderly shutdown of everything.

Ignoring IDE/editor files
--------------------------

In closed-source environments in which everyone uses the same IDE it is common
to add temporary files it produces to the project-wide `.gitignore` file.

However, in open source software such as MarteX Core, where everyone uses
their own editors/IDE/tools, it is less common. Only you know what files your
editor produces and this may change from version to version. The canonical way
to do this is thus to create your local gitignore. Add this to `~/.gitconfig`:

```
[core]
        excludesfile = /home/.../.gitignore_global
```

(alternatively, type the command `git config --global core.excludesfile ~/.gitignore_global`
on a terminal)

Then put your favourite tool's temporary filenames in that file, e.g.
```
# NetBeans
nbproject/
```

Another option is to create a per-repository excludes file `.git/info/exclude`.
These are not committed but apply only to one repository.

If a set of tools is used by the build system or scripts the repository (for
example, lcov) it is perfectly acceptable to add its files to `.gitignore`
and commit them.

Development guidelines
============================

A few non-style-related recommendations for developers, as well as points to
pay attention to for reviewers of MarteX Core code.

General MarteX Core
----------------------

- New features should be exposed on RPC first, then can be made available in the GUI

  - *Rationale*: RPC allows for better automatic testing. The test suite for
    the GUI is very limited

- Make sure pull requests pass Travis CI before merging

  - *Rationale*: Makes sure that they pass thorough testing, and that the tester will keep passing
     on the master branch. Otherwise all new pull requests will start failing the tests, resulting in
     confusion and mayhem

  - *Explanation*: If the test suite is to be updated for a change, this has to
    be done first

Wallet
-------

- Make sure that no crashes happen with run-time option `-disablewallet`.

  - *Rationale*: In RPC code that conditionally uses the wallet (such as
    `validateaddress`) it is easy to forget that global pointer `pwalletMain`
    can be NULL. See `qa/rpc-tests/disablewallet.py` for functional tests
    exercising the API with `-disablewallet`

- Include `db_cxx.h` (BerkeleyDB header) only when `ENABLE_WALLET` is set

  - *Rationale*: Otherwise compilation of the disable-wallet build will fail in environments without BerkeleyDB

General C++
-------------

- Assertions should not have side-effects

  - *Rationale*: Even though the source code is set to to refuse to compile
    with assertions disabled, having side-effects in assertions is unexpected and
    makes the code harder to understand

- If you use the `.h`, you must link the `.cpp`

  - *Rationale*: Include files define the interface for the code in implementation files. Including one but
      not linking the other is confusing. Please avoid that. Moving functions from
      the `.h` to the `.cpp` should not result in build errors

- Use the RAII (Resource Acquisition Is Initialization) paradigm where possible. For example by using
  `unique_ptr` for allocations in a function.

  - *Rationale*: This avoids memory and resource leaks, and ensures exception safety

C++ data structures
--------------------

- Never use the `std::map []` syntax when reading from a map, but instead use `.find()`

  - *Rationale*: `[]` does an insert (of the default element) if the item doesn't
    exist in the map yet. This has resulted in memory leaks in the past, as well as
    race conditions (expecting read-read behavior). Using `[]` is fine for *writing* to a map

- Do not compare an iterator from one data structure with an iterator of
  another data structure (even if of the same type)

  - *Rationale*: Behavior is undefined. In C++ parlor this means "may reformat
    the universe", in practice this has resulted in at least one hard-to-debug crash bug

- Watch out for out-of-bounds vector access. `&vch[vch.size()]` is illegal,
  including `&vch[0]` for an empty vector. Use `vch.data()` and `vch.data() +
  vch.size()` instead.

- Vector bounds checking is only enabled in debug mode. Do not rely on it

- Make sure that constructors initialize all fields. If this is skipped for a
  good reason (i.e., optimization on the critical path), add an explicit
  comment about this

  - *Rationale*: Ensure determinism by avoiding accidental use of uninitialized
    values. Also, static analyzers balk about this.

- Use explicitly signed or unsigned `char`s, or even better `uint8_t` and
  `int8_t`. Do not use bare `char` unless it is to pass to a third-party API.
  This type can be signed or unsigned depending on the architecture, which can
  lead to interoperability problems or dangerous conditions such as
  out-of-bounds array accesses

- Prefer explicit constructions over implicit ones that rely on 'magical' C++ behavior

  - *Rationale*: Easier to understand what is happening, thus easier to spot mistakes, even for those
  that are not language lawyers

Strings and formatting
------------------------

- Be careful of `LogPrint` versus `LogPrintf`. `LogPrint` takes a `category` argument, `LogPrintf` does not.

  - *Rationale*: Confusion of these can result in runtime exceptions due to
    formatting mismatch, and it is easy to get wrong because of subtly similar naming

- Use `std::string`, avoid C string manipulation functions

  - *Rationale*: C++ string handling is marginally safer, less scope for
    buffer overflows and surprises with `\0` characters. Also some C string manipulations
    tend to act differently depending on platform, or even the user locale

- Use `ParseInt32`, `ParseInt64`, `ParseUInt32`, `ParseUInt64`, `ParseDouble` from `utilstrencodings.h` for number parsing

  - *Rationale*: These functions do overflow checking, and avoid pesky locale issues

- For `strprintf`, `LogPrint`, `LogPrintf` formatting characters don't need size specifiers

  - *Rationale*: MarteX Core uses tinyformat, which is type safe. Leave them out to avoid confusion

Variable names
--------------

The shadowing warning (`-Wshadow`) is enabled by default. It prevents issues rising
from using a different variable with the same name.

Please name variables so that their names do not shadow variables defined in the source code.

E.g. in member initializers, prepend `_` to the argument name shadowing the
member name:

```c++
class AddressBookPage
{
    Mode mode;
}

AddressBookPage::AddressBookPage(Mode _mode) :
      mode(_mode)
...
```

When using nested cycles, do not name the inner cycle variable the same as in
upper cycle etc.


Threads and synchronization
----------------------------

- Build and run tests with `-DDEBUG_LOCKORDER` to verify that no potential
  deadlocks are introduced.

- When using `LOCK`/`TRY_LOCK` be aware that the lock exists in the context of
  the current scope, so surround the statement and the code that needs the lock
  with braces

  OK:

```c++
{
    TRY_LOCK(cs_vNodes, lockNodes);
    ...
}
```

  Wrong:

```c++
TRY_LOCK(cs_vNodes, lockNodes);
{
    ...
}
```

Source code organization
--------------------------

- Implementation code should go into the `.cpp` file and not the `.h`, unless necessary due to template usage or
  when performance due to inlining is critical

  - *Rationale*: Shorter and simpler header files are easier to read, and reduce compile time

- Don't import anything into the global namespace (`using namespace ...`). Use
  fully specified types such as `std::string`.

  - *Rationale*: Avoids symbol conflicts

GUI
-----

- Do not display or manipulate dialogs in model code (classes `*Model`)

  - *Rationale*: Model classes pass through events and data from the core, they
    should not interact with the user. That's where View classes come in. The converse also
    holds: try to not directly access core data structures from Views.

Subtrees
----------

Several parts of the repository are subtrees of software maintained elsewhere.

Some of these are maintained by active developers of Bitcoin Core, in which case changes should probably go
directly upstream without being PRed directly against the project.  They will be merged back in the next
subtree merge.

Others are external projects without a tight relationship with our project.  Changes to these should also
be sent upstream but bugfixes may also be prudent to PR against MarteX Core so that they can be integrated
quickly.  Cosmetic changes should be purely taken upstream.

There is a tool in contrib/devtools/git-subtree-check.sh to check a subtree directory for consistency with
its upstream repository.

Current subtrees include:

- src/leveldb
  - Upstream at https://github.com/google/leveldb ; Maintained by Google, but open important PRs to Core to avoid delay

- src/libsecp256k1
  - Upstream at https://github.com/bitcoin-core/secp256k1/ ; actively maintaned by Core contributors.

- src/crypto/ctaes
  - Upstream at https://github.com/bitcoin-core/ctaes ; actively maintained by Core contributors.

- src/univalue
  - Upstream at https://github.com/jgarzik/univalue ; report important PRs to Core to avoid delay.


Git and GitHub tips
---------------------

- For resolving merge/rebase conflicts, it can be useful to enable diff3 style using
  `git config merge.conflictstyle diff3`. Instead of

        <<<
        yours
        ===
        theirs
        >>>

  you will see

        <<<
        yours
        |||
        original
        ===
        theirs
        >>>

  This may make it much clearer what caused the conflict. In this style, you can often just look
  at what changed between *original* and *theirs*, and mechanically apply that to *yours* (or the other way around).

- When reviewing patches which change indentation in C++ files, use `git diff -w` and `git show -w`. This makes
  the diff algorithm ignore whitespace changes. This feature is also available on github.com, by adding `?w=1`
  at the end of any URL which shows a diff.

- When reviewing patches that change symbol names in many places, use `git diff --word-diff`. This will instead
  of showing the patch as deleted/added *lines*, show deleted/added *words*.

- When reviewing patches that move code around, try using
  `git diff --patience commit~:old/file.cpp commit:new/file/name.cpp`, and ignoring everything except the
  moved body of code which should show up as neither `+` or `-` lines. In case it was not a pure move, this may
  even work when combined with the `-w` or `--word-diff` options described above.

- When looking at other's pull requests, it may make sense to add the following section to your `.git/config`
  file:

        [remote "upstream-pull"]
                fetch = +refs/pull/*:refs/remotes/upstream-pull/*
                url = git@github.com:bitcoin/bitcoin.git

  This will add an `upstream-pull` remote to your git repository, which can be fetched using `git fetch --all`
  or `git fetch upstream-pull`. Afterwards, you can use `upstream-pull/NUMBER/head` in arguments to `git show`,
  `git checkout` and anywhere a commit id would be acceptable to see the changes from pull request NUMBER.
