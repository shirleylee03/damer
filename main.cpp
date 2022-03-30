#include <iostream>
#include"pdnet_parseoption.h"

int main(int argc,char **argv) {

    cmdlinet cmd; // try open cmd
    cmd.parse(argc,argv);
    cmd.doit();

    return 0;
}
