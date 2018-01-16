//
// Created by evgenii on 15.10.17.
//
#include "client.h"

int main() {
    ClientRun("/dev/", 'A');
    ClientWait();
    ClientFinalize();
}