#pragma once

#include "Containers/String.h"

class CSerialInterface {
public:
    virtual void GetSerial(CString &sSerial) = 0;

    virtual bool ValidateSerial(CString const &sSerial) = 0;
};
