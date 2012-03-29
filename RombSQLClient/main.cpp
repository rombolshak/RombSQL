#include <iostream>
#include <cstdlib>
int main(int argc, char **argv) {
    system("tar cf name.db `ls`");
    return 0;
}
