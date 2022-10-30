Used Technology
-----------------------------------

`LLVM <https://llvm.org/>`_ is a free, open-source compiler infrastructure under the `Apache License 2.0 <https://www.apache.org/licenses/LICENSE-2.0>`_. It is designed as a collection of tools including Front Ends parsers, Middle Ends optimizers, and Back Ends to produce machine code out of those programs.

`Clang <https://clang.llvm.org/>`_ is a front-end that uses a LLVM license. Clang works by taking the source language (e.g. C++) and translating it into an intermediate representation that is then received by the compiler back end (i.e., the LLVM backend). Its library-based architecture makes it relatively
easy to adapt Clang and build new tools based on it.  Cling inherits a number of features from LLVM and Clang, such as: fast compiling and low memory use,
efficient C++ parsing, extremely clear and concise diagnostics, Just-In-Time compilation, pluggable optimizers, and support for `GCC <https://gcc.gnu.org/>`_ extensions.


Interpreters allow for exploration of software development at the rate of human thought. Nevertheless, interpreter code can be slower than compiled code due to the fact that translating code at run time adds to the overhead and therefore causes the execution speed to be slower. This issue is overcome by exploiting the *Just-In-Time* (`JIT <https://en.wikipedia.org/wiki/Just-in-time_compilation>`_) compilation method. Cling relies on 

With the JIT approach, the developer types the code in Cling's command prompt. The input code is then lowered to Clang, where is compiled and eventually transformed in order to attach specific behavior. Clang compiles then the input into an AST representation, that is then lowered to LLVM IR, an `intermediate language <https://en.wikipedia.org/wiki/Common_Intermediate_Language>`_ that is not understood by the computer. LLVM’s just-in-time compilation infrastructure translates then the intermediate code into machine language (eg. Intel x86 or NVPTX) when required for use.  In fact, a JIT compiler is able to evaluate whether a certain part of the source code is executed often, and then compile this part, therefore reducing the overall execution time.

A JIT compiler translates bytecode into machine code only once and for next iterations machine just understands the bytecode. Normal interpreter in each iteration repeatedly ranslates bytecode into machine code therebytaking more time to complete a loop that runs millions of times.

JIT executes bytecode and mantains a count of how many time a function is executed. If this count exceeds a predefined limit JIT compiles the code into machine language which can directly be executed by the processor. Next time same function is calculated same compiled code is executed again unlike normal interpretation in which code is interpreted again line by line. This makes execution faster. 

LLVM provides Just In Time compilation APIs that are used by a number of open
source projects (e.g. Julia, Cling, Postgres, LLDB). LLVM’s JIT compilation APIs
are based on the idea of “JIT Linking”, parsing and linking a relocatable object
file in one process and making them executable in another target process. These
processes may be the same process, different processes on the same machine, or
even different processes on different machines. To facilitate this LLVM’s JIT APIs
provide the ExecutorProcessControl interface to abstract calls to the target
process (implemented via IPC/RPC in the cross-process case), and the
JITLinkMemoryManager interface to handle paired memory allocation in the
linker and the target processes. Depending on the JIT configuration that the
client chooses, efficient memory management schemes may include direct
allocation (when running in the same process), shared memory (when running in
different processes on one machine), or RPC calls (when running in different
processes across machine boundaries).

https://llvm.org/docs/ORCv2.html





