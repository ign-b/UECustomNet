#include "Serial.h"

#include "Serial/DummySerial.h"

CSerialInterface *CSerial::m_pInterface;

CSerial::CSerial(eSerial nMethod) {
    if (nMethod == SERIAL_DUMMY) {
        m_pInterface = new CDummySerial{};
    }
    /*                                */
    /* any else serial mechanism here */
    /*                                */
    else {
        m_pInterface = new CDummySerial{};
    }
}

void CSerial::GetSerial(CString &sSerial) {
    m_pInterface->GetSerial(sSerial);
}

bool CSerial::ValidateSerial(CString const &sSerial) {
    return m_pInterface->ValidateSerial(sSerial);
}
