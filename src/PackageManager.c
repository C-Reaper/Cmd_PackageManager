#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>

#include "/home/codeleaded/System/Static/Library/PackageManager.h"

int main() {
    PackageManager pm = PackageManager_New(".","./PackageManager.yaml");
    PackageManager_Load(&pm);
    PackageManager_Free(&pm);
    return 0;
}