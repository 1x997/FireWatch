//
// Created by Lian chenyu on 2018-12-04.
//

#include <iwatch/entity/iwatch_abnormality.h>

#include <malloc.h>

#include "iwatch/entity/iwatch_abnormality.h"

void iWatch_Release_Abnormality(iWatch_Abnormality *abnormality) {
    delete abnormality;
}

void iWatch_Release_Abnormalities(iWatch_Abnormality **abnormalities,  int abnormality_count) {
    for (int i = 0; i < abnormality_count; ++i) {
        iWatch_Abnormality *abnormality = abnormalities[i];
        if (nullptr != abnormality) {
            delete abnormality;
        }
    }
}