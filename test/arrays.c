// This file is part of SymCC.
//
// SymCC is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// SymCC is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// SymCC. If not, see <https://www.gnu.org/licenses/>.

// RUN: %symcc -O2 %s -o %t
// RUN: echo -ne "\x01\x02\x03\x04\x00" | %t 2>&1 | %filecheck %s
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    uint8_t input[4];
    if (read(STDIN_FILENO, input, sizeof(input)) != sizeof(input)) {
        printf("Failed to read the input\n");
        return -1;
    }

    uint8_t offset;
    if (read(STDIN_FILENO, &offset, sizeof(offset)) != sizeof(offset)) {
        printf("Failed to read the offset\n");
        return -1;
    }

    // This is just to make the base pointer symbolic.
    uint8_t *p = input + offset;

    printf("%s\n", (p[0] == 1) ? "yes" : "no");
    // SIMPLE: Trying to solve
    // QSYM-COUNT-2: SMT
    // ANY: yes

    // If our GetElementPointer computations are incorrect, this will create
    // path constraints that conflict with those generated by the previous array
    // access.
    printf("%s\n", (p[2] == 3) ? "yes" : "no");
    // SIMPLE: Trying to solve
    // QSYM-COUNT-2: SMT
    // ANY: yes

    // Use the pointer in a condition to see if contradicting constraints have
    // been created. The QSYM backend will log an error in this case (see
    // below), the simple backend just aborts.
    printf("%s\n", (p == input) ? "yes" : "no");
    // SIMPLE: Trying to solve
    // QSYM-NOT: Incorrect constraints are inserted
    // QSYM-COUNT-2: SMT
    // ANY: yes

    return 0;
}
