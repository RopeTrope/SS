# 🛠️ Custom Assembly Toolchain with Emulator

This project implements an **GNU based toolchain** for compiling and running a custom assembly language. It includes:

- A **lexical and syntax analyzer** built with **Flex and Bison**
- A **one-pass assembler**
- A **linker**
- An **emulator** for executing the generated machine code
- Customized format based on **ELF (Executable and Linkable Format)**

---

## 🧱 Project Components

### 1. **Lexer and Parser**
- **Tools used:** [Flex](https://github.com/westes/flex), [Bison](https://www.gnu.org/software/bison/)
- **Function:** Recognizes assembly syntax and directives, performs parsing, and builds intermediate representations.
- **Directives supported:**  
  - `.section`, `.word`, `.global`, `.text`, etc.

### 2. **One-Pass Assembler**
- Translates assembly instructions into machine code in **a single pass**.
- Outputs relocatable object files

### 3. **Linker**
- Combines multiple object files into a single **executable binary**.
- Resolves symbols, performs relocation, and organizes sections.

### 4. **Emulator**
- Executes the final linked file.
- Useful for running, debugging, and validating the compiled assembly programs.


