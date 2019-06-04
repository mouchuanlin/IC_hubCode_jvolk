/* Smoke Hub Pseudo-Code */
#define RETRY_LISTEN	1


enum State = {
	IDLE_Q_EMPTY,
	IDLE_Q_NOT_EMPTY,
	WAKING_MODEM,
	BUTTON_PUSH_MENU,
	CONNECTING_TO_NETWORK,
	SMS_LISTEN,
	SMS_PROCESSING,
	SENSOR_LEARN,
	SENSOR_DELETE,
	MD_TRANSMIT,
	NETWORK_ERR,
	RF_INTERRUPT
} currentState;


struct COMMAND_WT {
	uint8_t value,
	uint8_t priority
};

struct Q_COMMANDS {
	struct COMMAND_WT STARTUP_CMD = {0x00, 0x00};
	struct COMMAND_WT ALARM_CMD = {0x01, 0x08};
	struct COMMAND_WT TEST_PIN_CMD = {0x02, 0x06};
	struct COMMAND_WT BRD_TAMPER_CMD = {0x03, 0x07};
	struct COMMAND_WT MD_LBATT_CMD = {0x04, 0x05};
	struct COMMAND_WT MBRD_LBATT_CMD = {0x05, 0x04};
	struct COMMAND_WT SUPERVISORY_CMD = {0x06, 0x03};			// AKA "Test Frequency"
	struct COMMAND_WT RX_CMD = {0x07, 0x09};
};

struct Q_COMMANDS q_buf[20];

struct QUEUE
{
	void ADD(uint8_t cmd);
	void CLEAR();
	bool IS_EMPTY();
	uint8_t READ(uint8_t index);
	uint8_t GET_NEXT_INDEX();						// Retrieve next command, high-priority first. 
													// If it returns 0xFF then there're no more commands to retrieve, 
													// q_index is cleared.
};

struct QUEUE queue = {void add(uint8_t cmd), void clear(), bool isEmpty(), uint8_t read(uint8_t index), uint8_t getNextCmdIndex()};

uint8_t q_index = 0;
bool smsDone = false;					// User can text 1111#DONE# to end listen mode if they wish


void interrupt isr()
{
	
	if (alarm)
	{
		add alarm_cmd to Queue; debounce later and then set alarmStillTriggered = true until !alarm == true for next few WDT intervals
	}
	if (tamper)
		add tamper to Queue, debounce later, clear tamper interrupt
	
	Same for test pin
	
	if (RC2IF)
	{
		handle rx interrupt; do a buf2[pos++]=RC2REG-like thing
		when terminating byte is received, then add rx to the list (process first, then add 
	}
	
	if (TMR0IF)
	{
		TMR0IF = 0;
		checkState();			// Checks currentState value, controls LEDs accordingly. Only interrupts when awake.
	}
	
}

main()
{
	init_pic();
	queue.clear();
	queue.add(STARTUP_CMD);
	start_timer0();		// Timer0 interrupts for state machine/switch when unit is awake
	
	while (1)
	{
		if (!queue.isEmpty())
			run_modem();
		
		else if (queue.isEmpty())
		{
			while (!WDTCONbits.nTO)
			{
				SLEEP();							// if woken from sleep, is caused by alarm/tamper/test/etc external interrupt
													// such that ISR is checked, items added to Queue for modem to transmit
				NOP();
			}
		}
		
	}
}



void run_modem()
{
	while (!start_modem_ok())
		restart_modem();
	
	if((queue.read(q_index) == Q_STARTUP && \
		ee_read (IP1_ADDR) == default_IP1 && ee_read (ACCT_ADDR) == default_ACCT && \
		ee_read (APN_ADDR) == default_APN) || queue.read(q_index) == Q_COMMANDS.SUPERVISORY_CMD.value)
	{
		currentState = SMS_LISTEN;
		listen_for_sms_ok();
	}
	
	if(ee_read (IP1_ADDR) == default_IP1 && ee_read (ACCT_ADDR) == default_ACCT && ee_read (APN_ADDR) == default_APN)
		return;			// If defaults still programmed, just go back to start
	else
		enable_interrupts();
	
	while (!queue.isEmpty())
	{
		uint8_t nextCmd = queue.getNextCmd();
		currentState = IDLE_Q_NOT_EMPTY;
		
		if (nextCmd == 0xFF)
		{
			currentState = IDLE_Q_EMPTY;
			return;
		}
		
		if (!process_cmd_ok(nextCmd))
			currentState = NETWORK_ERR;
	
		
	TIP: don't queue receiver WHILE receiving data; finish receiving data from receiver and THEN queue with the data in a special receive buffer array.
	(do you want to have array for each serial ID? and if any of the devices transmit again before data is processed, then just write over the existing buffer.
}


void restart_modem()
{
	currentState = WAKING_MODEM;
	module_pwr = POWER_OFF;
	wait 5s
	module_pwr = POWER_ON;
}

bool start_modem_ok()
{
	currentState = WAKING_MODEM;
	module_pwr = POWER_ON;
	wait 50 seconds
	test AT
	test AT+IPR=115200
	test ATE1
	test AT+CMEE=2
	test AT#GPIO=7,0,2
	test AT#SLED=4
	test AT#CGMR
	
	check AT+CPIN value
	
	currentState = CONNECTING_TO_NETWORK;
	check AT+CREG? value, second param should be 1. Maybe 1st param should be 1 or 5?
	check AT+CEREG? value
	check AT+CGATT? value, send AT+CGATT=1 if it doesn't return 1
	check AT+CSQ value, first value should be > 6 (could be 1 or 2 digits long)
	
	if everything above checks out (if not at first then after a few tries),
		return true;
	else
		return false;
}


bool listen_for_sms_ok()
{
	if (!init_sms_ok())
		return false;
		
	minutesPerText = ee_read(SMS_WAIT_TIME_ADDR);
	for (uint8_t minute = 0; minute < minutesPerText; minute ++)
	{
		for(uint8_t second = 0; second < 60; second ++)
		{
			if (check_sms())
			{
				minute = 0;		// Re-start countdown timer if a message was received
				second = 0;
			}
			if (smsDone)
			{
				smsDone = false;
				return true;
			}
			currentState = SMS_LISTEN;
			delay5ms(200);
		}
	}
	return true;
}


bool init_sms_ok()
{
	test AT+CMGF=1
	test AT+CPMS="ME","ME","ME"
	test AT+CNMI=1,1
}


bool check_sms()
{
	bool receivedMsg;
	check AT+CMGL="ALL"
	if not empty,
		receivedMsg = true;
		parse message number, phone number, message contents based on comma delimiters.
		if (user says 1111#DONE#)
			smsDone = true;
	else
		receivedMsg = false;
	check CSQ, make sure it's still above 6
	return receivedMsg;
}

bool parse_sms_ok()
{
	currentState = SMS_PROCESSING;
	parse message here
}


uint8_t read(uint8_t queue_index)
{
	uint8_t data = q_buf[queue_index];
	for (uint8_t i = queue_index; i < sizeof(q_buf) - 1; i ++)
		q_buf[i] = q_buf[i + 1];									// Move all data @ higher indices down by one, like "popping" from a stack
	return data;
}

uint8_t getNextCmdIndex()
{
	uint8_t highestPriorityIndex = 0;
	uint8_t highestPriority = 0;
	for (uint8_t i = 0; i < sizeof(q_buf); i ++)		// Check buffer for highest-priority event
	{
		if (q_buf[i].priority > highestPriority)		
		{
			highestPriority = q_buf[i].priority;		
			highestPriorityIndex = i;
		}
	}
	return highestPriorityIndex;						// return index of highest-priority event
}