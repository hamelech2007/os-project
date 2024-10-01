#include "mouse.h"
#include "print.h"
#include "util.h"

void initializePS2Mouse() {
    uint8_t input, ack;

    // Step 1: Enable the auxiliary device (mouse)
    outPortB(0x64, 0xA8);  // Send command 0xA8 to enable the PS/2 mouse

    // Step 2: Request the current command byte from the PS/2 controller
    outPortB(0x64, 0x20);  // Send command 0x20 to read the PS/2 controller command byte
    input = inPortB(0x60);  // Read the command byte from port 0x60

    // Step 3: Modify the command byte to enable IRQ1 (keyboard) and IRQ12 (mouse)
    input |= 0x03;  // Set bits 0 and 1 to enable keyboard (IRQ1) and mouse (IRQ12) interrupts

    // Step 4: Write the modified command byte back to the PS/2 controller
    outPortB(0x64, 0x60);  // Send command 0x60 to prepare for writing the command byte
    outPortB(0x60, input);  // Write the modified command byte back to port 0x60

    // Step 5: Send command to enable mouse data reporting
    // Send the "write to mouse" command (0xD4) to the PS/2 controller
    outPortB(0x64, 0xD4);

    // Send the "enable data reporting" command (0xF4) to the mouse
    outPortB(0x60, 0xF4);

    // Step 6: Check for acknowledgment (0xFA) from the mouse
    ack = inPortB(0x60);  // Read the acknowledgment byte from the mouse
    if (ack != 0xFA) {
        inPortB(0x60);
        // Handle the error case if the mouse does not acknowledge
        // Add error handling code here (e.g., retry, log, etc.)
        // For debugging, you could print or log an error message
        return;  // Exit if acknowledgment was not received
    }

    // If we get here, the mouse is successfully initialized and data reporting is enabled.
}

uint8_t mouseInput[3];
uint8_t packetNum = 0;
bool firstSent = false;
void sendMousePacket(uint8_t packet) {
    if(!firstSent) {
        firstSent = true;
        if(packet != 0xfa) {
            // something has gone wrong
            return;
        }
        print_str("Mouse Interrupts Enabled!\n");
        return;
    }
    mouseInput[packetNum++] = packet;
    if(packetNum == 3) {
        mouseUpdate();
        packetNum = 0;
    }
}

void mouseUpdate() {
    uint8_t status = mouseInput[0];
    int8_t x = mouseInput[1];
    int8_t y = mouseInput[2];

    bool leftClick = status & 0x01;
    bool rightClick = status & 0x02;
    bool middleClick = status & 0x04;
    
    if(status == 0x8) return;
    if(peek_char() != ' ') print_char('\n');
    print_str("Mouse Event! ");
    print_str("Status: 0x");
    print_hex(status);
    print_str(" X: 0x");
    print_hex(x);
    print_str(" Y: 0x");
    print_hex(y);
    print_char('\n');
    // todo, handle mouse click
}