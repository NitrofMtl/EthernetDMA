/*   SAM DMA, Spec page 358:
   Programming Examples
   Single-buffer Transfer (Row 1)
   1. Read the Channel Handler Status Register DMAC_CHSR.ENAx Field to choose a free
      (disabled) channel.
   2. Clear any pending interrupts on the channel from the previous DMAC transfer by reading
      the interrupt status register, DMAC_EBCISR.
   3. Program the following channel registers:
      a. Write the starting source address in the DMAC_SADDRx register for channel x.
      b. Write the starting destination address in the DMAC_DADDRx register for
         channel x.
      c. Write the next descriptor address in the DMAC_DSCRx register for channel x with
         0x0..
      d. Program DMAC_CTRLAx, DMAC_CTRLBx and DMAC_CFGx according to Row 1
         as shown in Table 24-4 on page 357:
         ---------------------------------------------------------------------
         |Transfer Type     |SRC_DSCR|DST_DSCR|BTSIZE|DSCR|SADDR|DADDR|Others|
         |------------------|--------|--------|------|----|-----|-----|------|
         |Single Buffer     |   -    |   -    | USR  | 0  | USR | USR | USR  |
         |or last of multi  |        |        |      |    |     |     |      |
         |------------------|--------|--------|------|----|-----|-----|------|
         |Multi contig DADDR|   0    |   1    | LLI  |USR | LLI |CONT | LLI  |
         |------------------|--------|--------|------|----|-----|-----|------|
         |Multi contig SADDR|   1    |   0    | LLI  |USR |CONT | LLI | LLI  |
         |------------------|--------|--------|------|----|-----|-----|------|
         |Multi with LLI    |   0    |   0    | LLI  |USR | LLI | LLI | LLI  |
         ---------------------------------------------------------------------
      e. Write the control information for the DMAC transfer in the DMAC_CTRLAx and
         DMAC_CTRLBx registers for channel x. For example, in the register, you can program
         the following:
         - i. Set up the transfer type (memory or non-memory peripheral for source and
              destination) and flow control device by programming the FC of the DMAC_CTRLBx
              register.
         - ii. Set up the transfer characteristics, such as:
               - Transfer width for the source in the SRC_WIDTH field.
               - Transfer width for the destination in the DST_WIDTH field.
               - Incrementing/decrementing or fixed address for source in SRC_INC field.
               - Incrementing/decrementing or fixed address for destination in DST_INC field.
      f. Write the channel configuration information into the DMAC_CFGx register for channel
         x.
         - i. Designate the handshaking interface type (hardware or software) for the source
              and destination peripherals. This is not required for memory. This step requires
              programming the SRC_H2SEL/DST_H2SEL bits, respectively. Writing a '1' activates
              the hardware handshaking interface to handle source/destination requests. Writing a
              '0' activates the software handshaking interface to handle source/destination
              requests.
         - ii. If the hardware handshaking interface is activated for the source or destination
              peripheral, assign a handshaking interface to the source and destination peripheral.
              This requires programming the SRC_PER and DST_PER bits, respectively.
   4. After the DMAC selected channel has been programmed, enable the channel by writing
      a '1' to the DMAC_CHER.ENAx bit, where x is the channel number. Make sure that bit
      0 of DMAC_EN.ENABLE register is enabled.
   5. Source and destination request single and chunk DMAC transactions to transfer the
      buffer of data (assuming non-memory peripherals). The DMAC acknowledges at the
      completion of every transaction (chunk and single) in the buffer and carries out the buffer
      transfer.
   6. Once the transfer completes, the hardware sets the interrupts and disables the channel.
      At this time, you can either respond to the Buffer Transfer Completed Interrupt or
      Chained Buffer Transfer Completed Interrupt, or poll for the Channel Handler Status
      Register (DMAC_CHSR.ENAx) bit until it is cleared by hardware, to detect when the
      transfer is complete.
*/

#ifndef	SPI_DMA_H_INCLUDED
#define	SPI_DMA_H_INCLUDED

#include <Arduino.h>

#include <SPI.h>

#define USE_SPI_DMA

//TODO add with transfer buffer
//#ifndef SPI_HAS_TRANSFER_BUF
//#define SPI_HAS_TRANSFER_BUF
//#endif

//TO DO add macro in ethernet.h to replace ethernet setting macro or add SPI inherited class... ?
#define SPI_RATE 6       // divider


/* replace by SPI_DMA class //Nitrof
inline void digitalWriteDirectARM(int pin, bool val) // Быстрый вывод в порт SAM3
  {
  if(val) g_APinDescription[pin].pPort -> PIO_SODR = g_APinDescription[pin].ulPin;
  else    g_APinDescription[pin].pPort -> PIO_CODR = g_APinDescription[pin].ulPin;
  }
 //extern void spiBegin();    // Начало
 extern void spiInit(uint8_t spiRate);
 extern uint8_t spiRec();   // SPI receive a byte
 extern uint8_t spiRec(uint8_t* buf, size_t len);// SPI receive multiple bytes
 extern void spiSend(uint8_t b);// SPI send a byte
 extern void spiSend(const uint8_t* buf, size_t len);// SPI send multiple bytes
*/
class SPI_DMA_class {
public:
  void begin();
  inline void  digitalWriteDirectARM(int pin, bool val) {// Быстрый вывод в порт SAM3
    if(val) g_APinDescription[pin].pPort -> PIO_SODR = g_APinDescription[pin].ulPin;
    else    g_APinDescription[pin].pPort -> PIO_CODR = g_APinDescription[pin].ulPin;
  }

  void beginTransaction(SPISettings settings) {};
  void beginTransaction(uint8_t pin, SPISettings settings) {}; //maybe to have to check configs //Nitrof
  void endTransaction() {}; //think ok emty  //Nitrof

  


  uint8_t spiRec();   // SPI receive a byte
  uint8_t spiRec(uint8_t* buf, size_t len);// SPI receive multiple bytes
  void spiSend(uint8_t b);// SPI send a byte
  void spiSend(const uint8_t* buf, size_t len);// SPI send multiple bytes
//private: //probablely private //Nitrof
  void dmac_disable(); //Disable DMA Controller
  void dmac_enable(); //enable DMA Controller
  void dmac_channel_disable(uint32_t ul_num); //disable DMA Channel
  void dmac_channel_enable(uint32_t ul_num); //enable DMA Channel
  bool dmac_channel_transfer_done(uint32_t ul_num); //Poll for transfer complete
  void spiDmaRX(uint8_t* dst, uint16_t count); //start RX DMA
  void spiDmaTX(const uint8_t* src, uint16_t count); //start TX DMA
  void spiInit(uint8_t spiRate); //initialize SPI controller
  inline byte spiTransfer(byte b);
  void _switch(uint8_t spiRate);

  byte transfer(byte _pin, byte _data, SPITransferMode _mode = SPI_LAST); 
  uint16_t transfer16(byte _pin, uint16_t _data, SPITransferMode _mode = SPI_LAST){};  //TO DO
  void transfer(byte _pin, uint8_t *_buf, size_t _count, SPITransferMode _mode = SPI_LAST) { spiSend(_buf, _count); };

  // Transfer functions on default pin BOARD_SPI_DEFAULT_SS

  byte transfer(byte _data, SPITransferMode _mode = SPI_LAST) { return transfer(BOARD_SPI_DEFAULT_SS, _data, _mode); }
  uint16_t transfer16(uint16_t _data, SPITransferMode _mode = SPI_LAST) { return transfer16(BOARD_SPI_DEFAULT_SS, _data, _mode); }
  void transfer(uint8_t *_buf, size_t _count, SPITransferMode _mode = SPI_LAST) { transfer(BOARD_SPI_DEFAULT_SS, _buf, _count, _mode); }

  //void transfer(const uint8_t *buf, uint8_t *bufRead, uint16_t len); //to do, add with transfer buffer
}; 

extern SPI_DMA_class SPI_DMA;
#endif   //  SPI_DMA_H_INCLUDED