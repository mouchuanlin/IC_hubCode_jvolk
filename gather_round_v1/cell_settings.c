
#include "config.h"
#include "modem.h"

bool program_field(uint8_t field_number, uint8_t data_buf[], uint8_t size)
{
    uint8_t start_addr;
    uint8_t page = 0x00;
    uint8_t max_size = 40;
    uint8_t i = 0;
    if (field_number == 1)
        start_addr = IP1_ADDR;
    else if (field_number == 2)
        start_addr = IP2_ADDR;
    else if (field_number == 3)
        start_addr = IP3_ADDR;
    else if (field_number == 4)
        start_addr = IP4_ADDR;
    else if (field_number == 5)
        start_addr = ACCESS_CODE_ADDR;
    else if (field_number == 6)
        start_addr = PROG_ACK_ADDR;
    else if (field_number == 7)
        start_addr = TEST_FREQ_ADDR;
    else if (field_number == 8)
        start_addr = SERVER_ACK_TIME_ADDR;
    else if (field_number == 9)
        start_addr = SMS_WAIT_TIME_ADDR;
    else if (field_number == 10)
        start_addr = UNIT_ACCT_ADDR;
    else if (field_number == 11)
        start_addr = LINE_CARD_ADDR;
    else if (field_number == 12)
        start_addr = HUB_ZONE_ADDR;
    else if (field_number == 14)
        start_addr = TP_PIN_EN_ADDR;
    else if (field_number == 15)
        start_addr = NUM_NET_RETRY_ADDR;
    else if (field_number == 16)
        start_addr = NUM_IP_RETRY_ADDR;
    else if (field_number == 31)
        start_addr = PORT1_ADDR;
    else if (field_number == 32)
        start_addr = PORT2_ADDR;
    else if (field_number == 33)
        start_addr = PORT3_ADDR;
    else if (field_number == 34)
        start_addr = PORT4_ADDR;
    else if (field_number == 35)
        start_addr = APN_ADDR;
    else if (field_number >= 41 && field_number <= 56)
    {
        page = 0x01;
        start_addr = (uint8_t)(ID_START_ADDR + 6 * (field_number - 41));
        max_size = 6;
    }
    if (size <= max_size)
    {
        if (field_number >= 10)
        {
            sms_buf[0] = (uint8_t)(field_number / 0x0A + 0x41);
            sms_buf[1] = (uint8_t)(field_number % 0x0A + 0x30);
        }
        else
            sms_buf[0] = (uint8_t)(field_number + 0x30);
        sms_buf[2] = '#';
        for (i = 0; i < size; i++)
        {
            ee_write(page, start_addr, data_buf[i]);
            sms_buf[(uint8_t)(i + 3)] = data_buf[i];
        }
        sms_buf[(uint8_t)(i + 3)] = '#';
        return true;
    }
    return false;
}
