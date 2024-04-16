#include <SPI.h>
#include <SD.h>

File fd;
const uint8_t BUFFER_SIZE = 20;
char fileName[] = "demoFile.txt"; // SD library only supports up to 8.3 names
char buff[BUFFER_SIZE+2] = "";  // Added two to allow a 2 char peek for EOF state
uint8_t index = 0;

const uint8_t chipSelect = 8;

enum states: uint8_t { NORMAL, E, EO };
uint8_t state = NORMAL;

bool alreadyBegan = false;  // SD.begin() misbehaves if not first call

void initializeCard(void);
void eof(void);
void flushBuffer(void);
void readByte(void);

////////////////////////////////////////////////////////////////////////////////
// Standard Arduino setup function
////////////////////////////////////////////////////////////////////////////////
void setup() {
    Serial.begin(57600);
    while (!Serial);  // Wait for serial port to connect (ATmega32U4 type PCBAs)

    initializeCard();
}

////////////////////////////////////////////////////////////////////////////////
// Arduino calls this function over and over again when running
////////////////////////////////////////////////////////////////////////////////
void loop() {
    if (Serial.available() > 0) {
        readByte();
        if (index == BUFFER_SIZE) {
            flushBuffer();  // Write full buffer to µSD card
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Do everything from initializing the SD card to opening the demo file
////////////////////////////////////////////////////////////////////////////////
void initializeCard(void) {
    Serial.print(F("Initializing SD card..."));

    if (!SD.begin(chipSelect) && !alreadyBegan) { // begin uses half-speed...
        Serial.println(F("Initialization failed!"));
        while (!SD.begin(chipSelect)); // Retry until successful
        delay(250); // Give some time to stabilize
    }
    alreadyBegan = true;
    Serial.println(F("Initialization done."));

    if (SD.exists(fileName)) {
        Serial.print(fileName);
        Serial.println(F(" exists."));
    } else {
        Serial.print(fileName);
        Serial.println(F(" doesn't exist. Creating."));
    }
    
    Serial.println(F("Enter text to be written to file. 'EOF' will terminate writing."));
}

////////////////////////////////////////////////////////////////////////////////
// This function is called after the EOF command is received. It writes the
// remaining unwritten data to the µSD card, and prints out the full contents
// of the log file.
////////////////////////////////////////////////////////////////////////////////
void eof(void) {
    index -= 3; // Remove EOF from the end
    flushBuffer();
    
    // Re-open the file for reading:
    fd = SD.open(fileName);
    if (fd) {
        Serial.println("");
        Serial.print(fileName);
        Serial.println(":");
        while (fd.available()) {
            Serial.write(fd.read());
        }
    } else {
        Serial.print("Error opening ");
        Serial.println(fileName);
    }
    fd.close();
}

////////////////////////////////////////////////////////////////////////////////
// Write the buffer to the log file. If we are possibly in the EOF state, verify
// that to make sure the command isn't written to the file.
////////////////////////////////////////////////////////////////////////////////
void flushBuffer(void) {
    fd = SD.open(fileName, FILE_WRITE);
    if (fd) {
        fd.write(buff, index);
        fd.flush();
        index = 0;
        fd.close();
    } else {
        Serial.println("Failed to open file for writing");
    }
}

////////////////////////////////////////////////////////////////////////////////
// Reads a byte from the serial connection. This also maintains the state to
// capture the EOF command.
////////////////////////////////////////////////////////////////////////////////
void readByte(void) {
    byte byteRead = Serial.read();
    Serial.write(byteRead); // Echo
    buff[index++] = byteRead;
  
    // Update state machine to detect 'EOF'
    if (byteRead == 'E' && state == NORMAL) {
        state = E;
    } else if (byteRead == 'O' && state == E) {
        state = EO;
    } else if (byteRead == 'F' && state == EO) {
        eof();
        state = NORMAL;
    } else {
        if (state != NORMAL) {
            state = NORMAL;
        }
    }
}
