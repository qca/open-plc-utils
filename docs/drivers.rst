.. _driver-spi:

Serial Drivers
##############

.. _driver-spi-intro:

Introduction
============

Most Qualcomm Atheros PLC chipsets are Ethernet-to-Powerline bridges but the QCA7000 is a Serial-to-Powerline bridge, ... with a big difference. The QCA7000 expects the host serial stream to be segmented into Ethernet frames where each frame is encapsulated by a distinct serial header and trailer. This means the host can format and transmit, or receive and decode, standard Ethernet 802.3 frames over an ordinary SPI or UART interface thereby enabling full Ethernet or Internet protocol communications over powerline at low cost and low speed. The enabling component here is an Ethernet-to-Serial driver that supports the SPI or UART interface connected to the QCA7000. This section covers such a driver.

The example driver described here was written for the Freescale iMX28 board support package running a custom Freescale Linux distribution. As such, we believe that this driver is suitable for the iMX28 processor out of the box but it could be adapted to other processors.

.. _driver-spi-principles:

Principles of Operation
=======================

The basic unit of data transfer over MII and powerline is the IEEE 802.3 Ethernet frame. On output, the host must encapsulate each Ethernet frame as shown below before serial transmission to the QCA7000. The QCA7000 strips off the serial header and footer and forwards the frame over powerline.

SPI Transmit Frame
------------------

Allocate a 1528-byte buffer to accommodate the largest outgoing serial packet and pad with ``NUL`` bytes. Write ``SOF``, compute and write ``FL``, copy the outgoing frame then write ``EOF``. Serially transmit ``FL`` + ``12`` buffer bytes to the QCA7000.

+-----------------------+-------------+-------------+------------------------------------------------------------------------+
|        Offset         |   Length    |   Symbol    |                             Description                                |
+=======================+=============+=============+========================================================================+
|        0x0000         |     4       |    SOF      |  Start Of Frame. Must be ``0xAAAAAAAA``.                               |
+-----------------------+-------------+-------------+------------------------------------------------------------------------+
|        0x0004         |     2       |    FL       |  The Ethernet frame length in little endian format. The frame starts   |
|                       |             |             |  at offset ``0x0008`` here and includes all fields up to but excluding |
|                       |             |             |  ``EOF``. The minimum is ``60``. The maximum is ``1518`` if ``VLAN``   |
|                       |             |             |  is omitted and ``1522`` if not.                                       |
|                       |             |             |                                                                        |
+-----------------------+-------------+-------------+------------------------------------------------------------------------+
|        0x0006         |     2       |    RSVD     |  Must be ``0x0000``. Reserved to ensure 4-byte frame alignment.        |
+-----------------------+-------------+-------------+------------------------------------------------------------------------+
|        0x0008         |     6       |    DA       |  Destination Address                                                   |
+-----------------------+-------------+-------------+------------------------------------------------------------------------+
|        0x000E         |     6       |    SA       |  Source Address. This must not be the MAC address of the powerline     |
|                       |             |             |  device. This must be the MAC address of the local host serial         |
|                       |             |             |  interface as assigned by the SPI or UART driver. The PLC device       |
|                       |             |             |  and the associated host interface must have different MAC addresses.  |
|                       |             |             |                                                                        |
+-----------------------+-------------+-------------+------------------------------------------------------------------------+
|        0x0014         |     4       |    VLAN     |  Virtual LAN tag. This field may be omitted.                           |
+-----------------------+-------------+-------------+------------------------------------------------------------------------+
|        0x0018         |     2       |    ET       |  Ethertype. This field starts at offset ``0x0014`` if ``VLAN`` is      |
|                       |             |             |  omitted.                                                              |
+-----------------------+-------------+-------------+------------------------------------------------------------------------+
|        0x001A         | 42 to 1500  |    BODY     |  Frame body. This field starts at offset ``0x0016`` and ranges from    |
|                       |             |             |  ``46`` to ``1500`` bytes if ``VLAN`` is omitted.                      |
+-----------------------+-------------+-------------+------------------------------------------------------------------------+
|   0x004A to 0x005F8   |     4       |    EOF      |  End Of Frame. Must be ``0x5555``. This field starts at offset         |
|                       |             |             |  ``0x0008`` plus ``FL``.                                               |
+-----------------------+-------------+-------------+------------------------------------------------------------------------+

SPI Receive Frame
-----------------

The SPI read frame occurs in response to a read interrupt generated by the QCA7000. The host must acknowledge a read interrupt and service it by reading and acting on QCA7000 SPI register values. For incoming frames, the host reads the 32-bit overall packet length reported by the QCA7000 then read that many bytes, stripping off the serial header and footer and forwarding the Ethernet frame to the host.

Allocate a 1532-byte buffer to accomodate the largest incoming serial packet. Read ``LEN`` to determine the size of the incoming serial packet. Read ``LEN`` bytes into the buffer. Beware that ``LEN`` is a multiple of 4-bytes so there may be a few trailing ``NUL`` bytes in buffer.

+-----------------------+-------------+-------------+------------------------------------------------------------------------+
|        Offset         |   Length    |   Symbol    |                             Description                                |
+=======================+=============+=============+========================================================================+
|        0x0000         |     4       |    LEN      |  Hardware generated packet length. This field is only generated for    |
|                       |             |             |  SPI packets, not the UART packets.                                    |
+-----------------------+-------------+-------------+------------------------------------------------------------------------+
|        0x0004         |     4       |    SOF      |  Start Of Frame. Must be ``0xAAAAAAAA``.                               |
+-----------------------+-------------+-------------+------------------------------------------------------------------------+
|        0x0008         |     2       |    FL       |  The Ethernet frame length in little endian format. The frame starts   |
|                       |             |             |  at offset ``0x000C`` here and includes all fields up to but excluding |
|                       |             |             |  ``EOF``. The minimum is ``60``. The maximum is ``1518`` if ``VLAN``   |
|                       |             |             |  is omitted and ``1522`` if not.                                       |
|                       |             |             |                                                                        |
+-----------------------+-------------+-------------+------------------------------------------------------------------------+
|        0x000A         |     2       |    RSVD     |  Must be ``0x0000``. Reserved to ensure 4-byte frame alignment.        |
+-----------------------+-------------+-------------+------------------------------------------------------------------------+
|        0x000C         |     6       |    DA       |  Destination Address                                                   |
+-----------------------+-------------+-------------+------------------------------------------------------------------------+
|        0x0012         |     6       |    SA       |  Source Address. This must not be the MAC address of the powerline     |
|                       |             |             |  device. This must be the MAC address of the local host serial         |
|                       |             |             |  interface as assigned by the SPI or UART driver. The PLC device       |
|                       |             |             |  and the associated host interface must have different MAC addresses.  |
|                       |             |             |                                                                        |
+-----------------------+-------------+-------------+------------------------------------------------------------------------+
|        0x0018         |     4       |    VLAN     |  Virtual LAN tag. This field may be omitted.                           |
+-----------------------+-------------+-------------+------------------------------------------------------------------------+
|        0x001C         |     2       |    ET       |  Ethertype. This field starts at offset ``0x0018`` if ``VLAN`` is      |
|                       |             |             |  omitted.                                                              |
+-----------------------+-------------+-------------+------------------------------------------------------------------------+
|        0x001E         | 42 to 1500  |    BODY     |  Frame body. This field starts at offset ``0x001A`` and ranges from    |
|                       |             |             |  ``46`` to ``1500`` bytes if ``VLAN`` is omitted.                      |
+-----------------------+-------------+-------------+------------------------------------------------------------------------+
|   0x004A to 0x005F8   |     4       |    EOF      |  End Of Frame. Must be ``0x5555``. This field starts at offset         |
|                       |             |             |  ``0x000C`` plus ``FL``.                                               |
+-----------------------+-------------+-------------+------------------------------------------------------------------------+

.. _qcaspi1:

SPI Serial Driver
=================

qcaspi_spi_thread
-----------------

.. c:function:: static int qcaspi_spi_thread(char void *data)

Manages synchronization with the exteranl QCA7000.
Handles interrupts fomr the external QCA7000.
Transmits frames for the transmit queue to the QCA7000.

.. _qcaspi_qca7k_sync:

qcaspi_qca7k_sync
-----------------

.. c:function:: void qca_qca7k_sync(char struct qcaspi *qca, int event)

Keeps track of the current synchonization state.

.. _qcaspi2:

Register Functions
==================

qcaspi_read_register
--------------------

.. c:function:: uint16_t qcaspi_read_register(struct qcaspi *qca, uint16_t reg)

Reads a QCA7000 register and returns register content.

qcaspi_write_register
---------------------

.. c:function:: void qcaspi_write_register(struct qcaspi *qca, uint16_t reg, uint16_t value)

Write a value into a QCA7000 register.

qcaspi_tx_cmd
-------------

.. c:function:: int qcaspi_tx_cmd(struct qcaspi *qca, uint16_t cmd)

Transmit a 16-bit command to the QCA7000. This is currently used when performing a legacy DMA read or write.

.. _qcaspi3:

Interrupt Functions
===================

disable_spi_interrupts
----------------------

.. c:function:: uint32_t disable_spi_interrupts(struct qcaspi *qca)

Disables interrupts by writing ``0`` to the QCA7000 ``INTR_ENABLE`` register.

enable_spi_interrupts
---------------------

.. c:function:: uint32_t enable_spi_interrupts(struct qcaspi *qca, uint32_t intr_enable)

Enables interrupts specified by writing to the QCA7000 ``INTR_ENABLE`` register and returns the previous register value.

qcaspi_intr_handler
-------------------

.. c:function:: static irqreturn_t qcaspi_intr_handler(int irq, void *data)

Called to service interrupts on rising edge of the QCA7000 interrupt line.

.. _qcaspi4:

Transmit Functions
==================

qcaspi_transmit
---------------

.. c:function:: int qcaspi_transmit(struct qcaspi *qca)

Transmit as many frames as possible from the transmit queue.

qcaspi_tx_frame
---------------

.. c:function:: int qcaspi_tx_frame(struct qcaspi *qca, struct sk_buff *skb)

Transmit a single socket buffer over the SPI interface.

qcaspi_flush_txq
----------------

.. c:function:: void qcaspi_flush_txq(struct qcaspi *qca)

Flush the transmit queue. Typically called when a synchronization issue is detected between the SPI master (host) and SPI slave (QCA7000).

.. _qcaspi5:

Receive Functions
=================

qcaspi_receive
--------------

.. c:function:: int qcaspi_receive(struct qcaspi *qca)

Reads the QCA7000 read buffer bytes register and reads all available data from the QCA7000. Calls function to parse out the individual Ethernet frames and passes them to the Linux kernel protocol stack.

.. _qcaspi6:

DMA Functions
=============

qcaspi_dma_read_burst
---------------------

.. c:function:: uint32_t qcaspi_dma_read_burst(struct qcaspi *qca, uint8_t *buffer, uint32_t length)

Performs QCA7000 DMA burst read.

qcaspi_dma_read_legacy
----------------------

.. c:function:: uint32_t qcaspi_dma_read_legacy(struct qcaspi *qca, uint8_t *buffer, uint32_t length)

Performs QCA7000 DMA legacy read.

qcaspi_dma_write_burst
----------------------

.. c:function:: uint32_t qcaspi_dma_write_burst(struct qcaspi *qca, uint8_t *buffer, uint32_t length)

Called by ``qcaspi_tx_frame`` to peform a DMA burst write instead of a legacy write.

qcaspi_dma_write_legacy
-----------------------

.. c:function:: uint32_t qcaspi_dma_write_legacy(struct qcaspi *qca, uint8_t *buffer, uint32_t length)

Called by ``qcaspi_tx_frame`` to peform a DMA legacy write instead of a burst write.

.. _qcaspi7:

Support Functions
=================

.. _QcaFrmCreateHeader:

QcaFrmCreateHeader
------------------

.. c:function:: uint32_t QcaFrmCreateHeader(uint8_t *buffer, uint16_t length)

Encode buffer with the required SPI header and overall frame length.

.. _QcaFrmCreateFooter:

QcaFrmCreateFooter
------------------

.. c:function:: uint32_t QcaFrmCreateFooter(uint8_t *buffer)

Encode buffer with the required SPI footer.

.. _QcaFrmFsmInit:

QcaFrmFsmInit
-------------

.. c:function:: void QcaFrmFsmInit(QcaFrmHdl *frmHdl)

Initialize the state machine used to decode the incoming QCA7000 byte stream.

.. _QcaFrmFsmDecode:

QcaFrmFsmDecode
---------------

.. c:function:: uint32_t QcaFrmFsmDecode(QcaFrmHdl *frmHdl, uint8_t *buffer, uint16_t length, uint8_t bytevalue)

Feeds incoming bytes into the state machine and breaks the stream into individual frames. Frames are passed to the Linux kernel.

.. _qcaspi:

Kernel Functions
================

qcaspi_netdev_xmit
------------------

.. c:function:: int qcaspi_netdev_xmit(struct sk_buff *skb, struct net_device *device)

Called by the Linux kernel append outgoing frames to the transmit queue.

qcaspi_netdev_tx_timeout
------------------------

.. c:function:: void qcaspi_netdev_tx_timeout(struct net_device *device)

Called by the Linux kernel after the transmit queue has been stopped for an extended period of time.

qcaspi_netdev_uninit
--------------------

.. c:function:: static void qcaspi_netdev_uninit(struct net_device *device)

Called when function ``unregister_netdev`` is called. For the QCA7000 driver, function ``qcaspi_mod_exit`` calls function ``unregister_netdev``.

qcaspi_netdev_get_stats
-----------------------

.. c:function:: struct net_device_stats *qcaspi_netdev_get_stats(struct net_device *device)

Returns transmit, receive and error statistics associated with the net device. These are the statistics displayed by :program:`ifconfig`.

qcaspi_netdev_change_mtu
------------------------

.. c:function:: int qcaspi_netdev_change_mtu(struct net_device *device, int new_mtu)

Changes the serial interface MTU size.

qcaspi_netdev_set_mac_address
-----------------------------

.. c:function:: static int qcaspi_netdev_set_mac_address(struct net_device *device, void *memory)

Sets the serial interface MAC address. Called by :program:`ifconfig` whenever user types :command:`ifconfig qca0 hw ether xx:xx:xx:xx:xx:xx`.

qcaspi_netdev_close
-------------------

.. c:function:: int qcaspi_netdev_close(struct net_device *device)

Called by :program:`ifconfig` to disable the network interface.

qcaspi_mod_exit
---------------

.. c:function:: static void qcaspi_mod_exit(void)

Called by the kernel to shutdown the driver module.


