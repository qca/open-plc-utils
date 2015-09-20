.. _procedures:

Procedures
##########

.. _procedure-intro:

Introduction
============

The Open Powerline Toolkit can perform all sorts of operations on Atheros powerline devices. Many operations are simple and can be accomplished using one command. Others are more complex and may require several commands. This section demonstrates common console commands that you might want to use when experimenting or working with Atheros powerline devices. Multi-command procedures can be generalized and automated using any programming or scripting language that can execute console commands.

.. _procedure-select-interface-linux:

Select an Interface on Linux
============================

:program:`Linux` network interfaces are referenced by name. Use Linux utility :program:`ifconfig` to determine the names of interfaces on your host. By default, Toolkit programs communicate with powerline devices on interface ``eth1`` but another interface can be specified on the command line with the ``-i`` option:

Selecting another Interface on Linux
------------------------------------
.. code-block:: console

    $ int6k -i eth3 ...

In the previous example, :command:`int6k` will send and receive frames on interface ``eth3`` instead of the default interface. Having to specify the interface can become become tedious when entering commands manually. An alternative method involves setting environment variable ``PLC`` to your preferred Ethernet interfaces, as follows:

Changing the Default Interface on Linux
---------------------------------------
.. code-block:: console

    $ export PLC="eth2"

In the previous example, environment variable ``PLC`` is defined and exported, making it available to all programs running under the current console session. This command can be included in :file:`~/.profile` or :file:`/etc/profile` or in :file:`/etc/environment` on *Debian* and *Ubuntu* systems.

Finding Interfaces on Linux
---------------------------
.. code-block:: console

	$ ifconfig
	eth0      Link encap:Ethernet  HWaddr 00:60:97:05:97:0C
		  inet addr:192.168.170.179  Bcast:192.168.170.255  Mask:255.255.255.0
		  inet6 addr: fe80::260:97ff:fe05:970c/64 Scope:Link
		  UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
		  RX packets:52130758 errors:0 dropped:0 overruns:0 frame:0
		  TX packets:59629414 errors:0 dropped:0 overruns:0 carrier:0
		  collisions:0 txqueuelen:1000
		  RX bytes:43781526 (41.7 MiB)  TX bytes:4226651392 (3.9 GiB)
		  Interrupt:209 Base address:0xe000

	eth1      Link encap:Ethernet  HWaddr 00:0F:EA:10:D5:1C
		  inet addr:192.168.99.1  Bcast:192.168.99.255  Mask:255.255.255.0
		  inet6 addr: fe80::20f:eaff:fe10:d51c/64 Scope:Link
		  UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
		  RX packets:68711621 errors:0 dropped:0 overruns:0 frame:0
		  TX packets:70059644 errors:0 dropped:0 overruns:0 carrier:0
		  collisions:0 txqueuelen:1000
		  RX bytes:3449044517 (3.2 GiB)  TX bytes:1328494982 (1.2 GiB)
		  Interrupt:217

	lo        Link encap:Local Loopback
		  inet addr:127.0.0.1  Mask:255.0.0.0
		  inet6 addr: ::1/128 Scope:Host
		  UP LOOPBACK RUNNING  MTU:16436  Metric:1
		  RX packets:177789 errors:0 dropped:0 overruns:0 frame:0
		  TX packets:177789 errors:0 dropped:0 overruns:0 carrier:0
		  collisions:0 txqueuelen:0
		  RX bytes:599522176 (571.7 MiB)  TX bytes:599522176 (571.7 MiB)

The previous example demonstrates the use of Linux :program:`ifconfig` utility to identify available network interfaces by name and MAC address.

If your computer has multiple Ethernet interface cards installed, you can connect each card to a different Atheros powerline device and communicate with each device, independently, from one console session (window) by specifying the appropriate interface card with each command. It is also possible to open multiple console sessions (windows), each having a unique enviroment, and assign a different value for ``PLC`` in each enviroment.

.. _procedure-select-interface-windows:

Select an Interface on Windows
==============================

:program:`WinPcap` references host network interfaces by number, not name. This means that you must determine the proper interface number for the network adapter that you plan to use. Use Toolkit `pcapdevs <pcapdevs.7.html>`_ utility will enumerate available host network adapters by number,  MAC address and description. By default, Toolkit programs communicate with powerline devices on pcap interface ``2`` but another interface can be specified on the command line with the ``-i`` option:

Selecting another Interface on Windows
--------------------------------------
.. code-block:: console

    $ int6k -i 3 ...

In the previous example, :command:`int6k` will send and receive frames using host interface ``3`` instead of the default interface. Having to specify the interface can become tedious when entering commands manually. Presently there is no way to specify a default interface on WIndows platforms.

Finding Interfaces on Windows
-----------------------------
.. code-block:: console

	$ pcapdevs
	 1 00:00:00:00:00:00 \Device\NPF_GenericDialupAdapter	(Adapter for generic dialup
	 2 00:0E:2E:03:5F:B8 \Device\NPF_{4CFAABD5-C929-4942-914B-BDFE72B13611} (Realtek R
	 3 00:C0:49:D5:CB:14 \Device\NPF_{73E0D2EF-6069-4831-B379-507025F2BDCD} (Realtek R
	 4 00:0C:76:87:3E:3D \Device\NPF_{86B96905-3AA7-46F1-969A-6C23E3BBBA8B} (Broadcom

The previous example illustrates the use of Toolkit `pcapdevs <pcapdevs.7.html>`_ utility to identify available network interfaces by interface number, MAC address and description. The descriptions have been truncated in this example because they are too long. Devices having MAC address ``00:00:00:00:00:00`` are unavailable.

.. _procedure-identify-device:

Identify a Device
=================

Before you start working with Atheros powerline devices, you should determine the chipset and firmware revision because that will determine the types of operations that can be performed and the order in which some operations should be performed. The following example illustrates how to determine this information.

.. _script-example-device-identification:

Device Identification
---------------------
.. code-block:: console

    $ int6k -r
    eth1 00:B0:52:00:00:01 Fetch Version Information
    eth1 00:B0:52:DA:DA:23 INT6300A0 INT6000-MAC-3-0-3001-1081-LA-A NOT_UPGRADEABLE
    eth1 00:B0:52:FE:87:2D INT6000A1 INT6000-SL-0-1-0110-200-2006-0415-BETA NOT_UPGRADEABLE

This example demonstrates several things.

First, the `int6k <int6k.7.html>`_ ``-r`` option requests hardware and software revision information from a device by sending an Atheros ``VS_SW_VER`` vendor-specific managerment message and waiting for a response. Atheros vendor-specific management messages are described in the *Atheros HomePlug AV Firmware Technical Reference Manual*. It you do not have access to this document then many toolkit features may be difficult to understand.

Since we did not specify an Ethernet interface on the command line, the default interface was used. In this case, the default interface was ``eth1`` and so we observe that ``eth1`` was used to both send requests and receive responses.

Since we did not specify a device address on the command line, the default address was used. Device addresses are also known as MAC, "Ethernet" or "hardware" addresses. They uniquely identify a device on a network. Unlike Ethernet cards,  powlerlines device do not have IP addresses.

The default device address for toolkit programs is ``00:B0:52:00:00:01``. This address is the Local Management Address (LMA). All Atheros devices connected to the local host will respond to this address and include their actual address in the response. In the above example, two devices responded. This means that both were connected directly to ``eth1``, perhaps via a network hub. Observer that device ``00:B0:52:DA:DA:23`` is an INT6300 and device ``00:B0:52:FE:87:2D`` is an INT6000. The first device is running Atheros firmware and the second device is running the Atheros Softloader.

.. _script-example-device-configuration:

Inspecting Device Configuration
-------------------------------

To find out more about device ``00:B0:52:DA:DA:23`` we might issue the following command where option ``-I`` requests identity information stored in the device PIB. In this case, we specified the device by MAC address to prevent other local devices from responding. There is no harm in having both devices respond so we could have specified both hardware addresses.

.. code-block:: console

    $ int6k -I 00:B0:52:DA:DA:23
    eth1 00:B0:52:DA:DA:23 Device Identity
    eth1 00:B0:52:DA:DA:23 -------
	    PIB 3-2 7864 bytes
	    MAC 00:B0:52:DA:DA:23
	    DAK 24:03:D7:84:09:A0:9A:78:8D:F6:82:75:AF:0D:0F:D8
	    NMK 50:D3:E4:93:3F:85:5B:70:40:78:4D:F8:15:AA:8D:B7 (HomePlugAV)
	    NID B0:F2:E6:95:66:6B:03
	    NET Jupiter-23
	    MFG Intergalactic Death Rays Inc.
	    USR Rattin Tordo
	    CCo Auto

Pay special attention to the MAC address after flashing NVRAM. Hardware address ``00:B0:52:00:00:03`` indicates the Default PIB used when no other PIB is available. The Default PIB disables communication over powerline.

Pay special attention to the NMK when devices cannot communicate. Devices having different NMK values will not form a network. Conversely, all devices having the same NMK will form a network. This becomes important when programming large numbers of devices. You must have an out-of-the-box networking strategy that balances user convenience and network security.

The string next to the NMK indicates that password "HomePlugAV" was used to generate that key. This is one of two default passwords and keys used by Atheros. In some cases, it may be another clue that the device is using the default PIB.

.. _procedure-inspect-network:

Browse the Network
==================

A powerline network consists of one or more powerline devices that share a common NMK. One device on the network is in charge of the network and known as the Central Coordinator (CCo). All other devices on that network are known as stations (STA). Each device maintains internal information about the network to which it belongs. You can view that information at any time.

Display Network Information
---------------------------
.. code-block:: console

	$ int6k  -mieth3
	eth3 00:B0:52:00:00:01 Fetch Network Information
	eth3 00:B0:52:00:00:BB Found 1 Network(s)

		network->NID = 77:23:0D:91:06:6E
		network->SNID = 6
		network->TEI = 1
		network->ROLE = 0x02 (CCO)
		network->CCO_DA = 00:B0:52:00:00:BB
		network->CCO_TEI = 1
		network->STATIONS = 1

			station->DA = 00:B0:52:00:00:AA
			station->TEI = 2
			station->BDA = FF:FF:FF:FF:FF:FF
			station->AvgPHYDR_TX = 0 mbps
			station->AvgPHYDR_RX = 0 mbps

The example above prompts the local device on interface ``eth3`` for network membership (``-m``) information. Device ``00:B0:52:00:00:BB`` responded and reported that it belongs to one powerline network. The first block of information identifies the network CCo. The second block of information identifies a network STA. This second block will repeat when multiple stations are present. Each device on the network has a unique Terminal Equipment Identifier (TEI) within the network. The average PHY rates show the data rates between the reporting device and each STA. In this cases there has been no traffic.

.. _procedure-change-network:

Forming a Network
=================

A powerline network is formed when two or more devices share a common Network Membership Key (NMK). An NMK is always 16 bytes (32 hexadecimal digits) in length and can have any value,  including all zeros. Devices programmed with the same NMK value can communicate while those programmed with different NMK values cannot. Program `int6k <int6k.7.html>`_ can be used to program a local or remote device with a specific NMK value.

Setting the Network Membership Key
----------------------------------
.. code-block:: console

    $ int6k -M
    eth1 00:B0:52:00:00:01 Set Local Network Membership Key
    eth1 00:B0:52:BA:BA:56 Setting ...

This example programs the local device with the default NMK. Option ``-M`` sets the network membership key with the default key value. The default key is used because we omitted option ``-K`` and the local device is programmed because we omitted the device MAC address.

One can quickly and easily create a default network by connecting each device,  in turn, to the local host and executing this command shown above. The devices will then form a network when connected via the powerline. Although fast and simple, this method is not very secure because the default NMK is used by many Atheros tools and is well known.

There are cases where one wants to create separate, independent networks on the powerline. This is accomplished by programming some devices with one NMK and other devices with another NMK. In such cases we need to specify an NMK value.

Forming a Custom Network
------------------------
.. code-block:: console

    $ int6k -M -K 00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF
    eth1 00:B0:52:00:00:01 Set Local Network Membership Key
    eth1 00:B0:52:BA:BA:56 Setting ...

This example programs the local device NMK with the value ``00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF``. Option ``-K`` sets the default key value used by option ``-M``. The default device is programmed because we omitted a target MAC address from the command line but we could have specified the local device using the actual device MAC address or the Local Management Address (LMA) of ``00:B0:52:00:00:01`` or the keyword ``local``,  like so ...

.. code-block:: console

    $ int6k -M -K 00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF
    $ int6k -M -K 00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF 00:B0:52:BA:BE:56
    $ int6k -M -K 00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF local

Remote Network Configuration
----------------------------

Once a network has been established, one can change the NMK of remote devices over the powerline but once the NMK on the remote device has changed, it is no longer part of original network and can no longer be returned to the network in the same way. Additionally, one cannot change the NMK on a remote device without specifying the DAK of the device to be changed.

.. code-block:: console

	$ int6k -M -K 00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF \
		-J 00:B0:52:BA:BE:57 \
		-D A7:8E:11:FB:6A:90:2C:CB:A7:8E:11:FB:6A:90:2C:CB
	eth1 00:B0:52:00:00:01 Set Remote Network Membership Key
	eth1 00:B0:52:BA:BA:56 Setting ...

This example sets the NMK on remote device via the local device. Option ``-J`` specified the MAC address of the remote device. Option ``-D`` specified the DAK of the remote device. The request is sent to the local device, because we omitted the target MAC address, and the local device forwards our request to the remote device. The request will fail if the remote device is not accessible or the remote device DAK is incorrect.

This command is one of the more complicated `int6k <int6k.7.html>`_ commands,  requires lots of typing and is consequently prone to typing errors. The order of command line options is not important.

.. _procedure-configure-device:

Configuring a Device
====================

Device identity, capability and behavior are controlled by runtime parameters stored in the device PIB. To change device identity,  capability or behavior you must alter a template PIB file, download it to the device and flash it into NVRAM. A template PIB can come from several sources but an excellent souce is the device, itself. Configuration changes then become a simple read, modify and write back operation.

Read a PIB File
---------------
.. code-block:: console

    $ int6k -i eth2 -p old.pib local
    eth2 00:B0:52:00:00:01 Read Parameters from Device
    eth2 00:B0:52:00:00:BE Read.

This example reads the PIB from the local device and writes it to a file. The toolkit has a growing number of programs designed to inspect and change pibfiles in various ways. Program `modpib <modpib.7.html>`_ is one program that can be used for this purpose.

Modify a PIB File
-----------------
.. code-block:: console

	$ modpib old.pib -M 00:B0:52:00:BA:BE \
			 -N 00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF \
			 -D FF:EE:DD:CC:BB:AA:99:88:77:66:55:44:33:22:11:00 \
			 -S "Intergalactic Death Rays, Inc." \
			 -T "Global Spy Network" \
			 -U "Bedroom"

The example above edits identity parameters in PIB file,  :file:`old.pib`, recomputes the internal checksum and re-writes the file. The network identity parameters are MAC (``-M``), NMK (``-N``) and DAK (``-D``). The user identity parameters are MFG_HFID (``-S``),  NET_HFID (``-T``) and USR_HFID (``-U``). The PIB file is ready for download and flash.

Program `modpib <modpib.7.html>`_ is a safe way to edit a PIB file because it only change selected parameters. Use program `setpib <setpib.7.html>`_ to edit other parameters if required.

Save a PIB File
---------------
.. code-block:: console

    $ int6k -i eth2 -P old.pib -C pib
    eth2 00:B0:52:00:00:01 Write Parameters to Device
    eth2 00:B0:52:00:00:BE Written.

This example writes the modified PIB file, :file:`old.pib`, to the local device and commits it to NVRAM. Option ``-P`` writes the PIB file to the device instead of reading it from the device. Option ``-C`` with argument ``pib`` commits only the PIB, and not firmware, to NVRAM.

.. _procedure-upgrade-device:

Upgrading Firmware and PIB
==========================

Eventually,  you may want to upgrade (or downgrade) the runtime firmware stored in NVRAM. To do this, you must locate a copy of the new :file:`.nvm` file on the Atheros FTP site and download it to your host. You must then download the file to the device and ask the runtime firmware on the device to write the file to NVRAM. Writing firmware to NVRAM is also called "flashing the device". Program `int6k <int6k.7.html>`_ can be used for this purpose when runtime firmware is executing on the device.

Program :program:`int6k` instructs runtime firmware to write to NVRAM but the firmware will behave differently depending on how it was loaded and started. The first time NVRAM is written by runtime firmware, the Factory PIB is created in NVRAM. The second time, the User PIB is created. Thereafter, the User PIB will be re-written each time. This preserves original factory settings and permits their restoration regardless of intervening parameter changes. This means that you should make an effort to get things right the first time.

Upgrade Firmware and PIB
------------------------
.. code-block:: console

    $ int6k -i eth2 -P ../firmware/new.pib -N ../firmware/new.nvm -F
    eth2 00:B0:52:00:00:01 Write Firmware to Device
    eth2 00:B0:52:00:00:BE Wrote ../firmware/new.nvm
    eth2 00:B0:52:00:00:01 Write Parameters to Device
    eth2 00:B0:52:00:00:BE Wrote ../firmware/new.pib
    eth2 00:B0:52:00:00:01 Flash NVRAM
    eth2 00:B0:52:00:00:BE Flashing ...
    eth2 00:B0:52:00:00:BE Started INT6000-MAC-3-3-3325-00-2443-20080414-FINAL-B

This example downloads a PIB file (``-P``) and an NVM file (``-N``) to the device then instructs the runtime firmware to write (or flash) (``-F``) both of them to NVRAM. Option ``-F`` always writes both to NVRAM but option ``-C`` can be used to write only one or the other or both.

Observe that options ``-P``, ``-N`` and ``-F`` are all in uppercase. As a rule, uppercase options modify the device and lowercase options do not. One notable exception to this rule is ``-I`` which merely displays device identity.

.. _procedure-upgrade-pib:

Upgrading PIB Only
==================

When flashing a PIB to the local device, the PIB DAK must match that of the local device or you will get a "DAK Not Zero" error message. This message is incorrect and should probably read "DAK Mismatch". The following two toolkit commands should prevent this error from occuring on Linux. The first command reads the actual MAC (``-A``) and DAK (``-D``) from the device using program `int6kid <int6kid.7.html>`_ and inserts them on the command lise as `modpib <modpib.7.html>`_ arguments using options ``-M`` and ``-D``. We need not include the station MAC address on the `int6k <int6k.7.html>`_ command line because the device is local, but we could include it.

Upgrade local device PIB
------------------------
.. code-block:: console

    $ modpib abc.pib -ieth1 -M $(int6kid -ieth1 -A) -D $(int6kid -ieth1 -D)
    $ int6k -P abc.pib -C pib

When flashing a PIB to a remote device, the PIB DAK must be all zeros or you will get a "DAK Not Zero" error message. This message occurs because the *HomePlug* AV Specification does not permit a DAK to be transmitted over powerline in any form. The following two toolkit commands should prevent this error on Linux. We must include the device MAC on the command line when invoking each program because the device is remote. Notice that symbol ``none`` can be used as shorthand ``00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00`` when specifying the DAK.

Upgrade remote PIB
------------------
.. code-block:: console

    $ modpib abc.pib -ieth1 -M $(int6kid -ieth1 -A 00B052BEEF03) -D none
    $ int6k -P abc.pib -C2 00B052BEEF03

The MAC address stored in the PIB can have any value but it is good practice to preserve the original value. This avoids the common mistake of flashing multiple network stations with the same MAC address because the PIB was not updated.

.. _procedure-initialize-device:

Starting a Device
=================

Some device configurations have no flash memory to store runtime parameters and firmware so the local host must store them until needed then write them into device SDRAM and start execution. This is also needed when the flash memory is blank or corrupted because the runtime firmware is the only way to write flash memory. Programs `int6kf <int6kf.7.html>`_,  `int6kboot <int6kboot.7.html>`_, `ampboot <ampboot.7.html>`_ and `plcboot <plcboot.7.html>`_ are used to configure a blank device then download runtime parameters and firmware and start execution. They all perform the same basic function but in different ways.

Start Firmware on *INT6000*
---------------------------
.. code-block:: console

    $ int6kf -C abc.cfg -N abc.nvm -P abc.pib

Program `int6kf <int6kf.7.html>`_ can be used to download runtime firmware and parameters and start execution on and *INT6000* chipset. The process involves downloading a :file:`.cfg` file using ``VS_SET_SDRAM``, downloading a :file:`.nvm` file using ``VS_WR_MEM``, downloading a :file:`.pib` file using ``VS_WR_MEM`` then starting firmware execution using ``VS_ST_MAC``. See the `int6kf <int6kf.7.html>`_ man page for an explanation of the command line options and arguments used here.

Start Firmware on *INT6300* and *INT6400*
-----------------------------------------
.. code-block:: console

    $ int6kboot -N abc.nvm -P abc.pib

Program `int6kboot <int6kboot.7.html>`_ can be used to download runtime firmware and parameters and start execution on an *INT6300* or *INT6400* chipset. The process involves downloading a :file:`.nvm` file using ``VS_WR_MEM``, downloading a :file:`.pib` file using ``VS_WR_MEM`` then starting firmware execution using ``VS_ST_MAC``. No :file:`.cfg` file is needed or permitted with this application.

Start Firmware on *AR7400*
--------------------------
.. code-block:: console

    $ ampboot -N abc.nvm -P abc.pib

Program `ampboot <ampboot.7.html>`_ can be used to download runtime firmware and parameters and start execution of runtime firmware on an *INT6300*, *INT6400* or *AR7400* chipset. The process involves downloading a :file:`.pib` file using ``VS_WR_MEM``, downloading a :file:`.nvm` file using ``VS_WR_MEM`` then starting firmware execution using ``VS_ST_MAC``. A general purpose applet, stored in the :file:`.nvm` file, configures SDRAM. No :file:`.cfg` file is needed or permitted with this application.

Start Firmware on *AR7420*
--------------------------
.. code-block:: console

    $ plcboot -N abc.nvm -P abc.pib

Program `plcboot <plcboot.7.html>`_ can be used to download runtime firmware and parameters and start execution an *INT6300*, *INT6400*, *AR7400* or *AR7420* chipset. This program detects the type of chipset and the image file format. The boot process varies depending on the chipset that is detected and the image file format. For the *AR7420* chipset, this process involves downloading a :file:`.pib` and :file:`.nvm` file using only ``VS_WRITE_EXECUTE`` messages. For earlier chipsets, the process is as described above.

.. _procedure-testing-device:

Testing a Device
================

When the :program:`Bootloader` is running, it is possible to perform systematic diagnostic testing by downloading and executing a variety of small programs, called "applets". A collection of applets can be found in the toolkit :file:`Applets` folder. Applet `manpages <toolkit.html#applet-files>`_ describe the purpose of each one. Some applets are more useful than others. Start by reading the `applets <applets.7a.html>`_ manpage.

Program `int6ktest <int6ktest.7.html>`_ can be used to sequentially download and execute a series of applets. Applets have evolved over time and some are more useful or informative than others. Newer applets report progress and errors on the console. Older applets report progress or errors by flashing the onboard LEDs. This means of testing cannot be used after the flash memory has been programmed unless you erase, corrupt or remove the flash.

.. code-block:: console

    $ int6ktest -i eth4 connection.nvm memctl.nvm marchloop.nvm
    eth4 00:B0:52:00:00:01 Write Memory (00000000:32)
    eth4 00:B0:52:00:00:01 Wrote connection.nvm
    eth4 00:B0:52:00:00:01 Start connection.nvm (0)
    eth4 00:B0:52:00:00:01 Write Memory (00001000:664)
    eth4 00:B0:52:00:00:01 Wrote connection.nvm
    eth4 00:B0:52:00:00:01 Start connection.nvm (1)
    eth4 00:B0:52:00:00:01 Write Memory (00000040:6076)
    eth4 00:B0:52:00:00:01 Wrote memctl.nvm
    eth4 00:B0:52:00:00:01 Start memctl.nvm (0)
    eth4 00:B0:52:00:00:01 Write Memory (00001000:2212)
    eth4 00:B0:52:00:00:01 Wrote marchloop.nvm
    eth4 00:B0:52:00:00:01 Start marchloop.nvm (0)
    eth4 00:B0:52:00:00:01 0x00 March2 test - Pass

This example uses program `int6ktest <int6ktest.7.html>`_ to download and execute applets `connection.nvm <connection.7a.html>`_, `memctl.nvm <memctl.7a.html>`_ and `marchloop.nvm <marchloop.7a.html>`_ to the powerline device on host interface "eth4". The first applet checks circuit board connections to memory and reports progress and errors by flashing onboard LEDs. The second configures the SDRAM controller and exits without any indication. The third performs a systematic test of SDRAM and reports progress and errors on the console. In this case the test passed.

See programs `amptest <amptest.7.html>`_ and `plctest <plctest.7.html>`_ for updated versions of this program for use with *AR7400* and *QCA7420* chipsets.

.. _procedure-inialize-nvram:

Programming NVRAM
=================

.. note::
	The *INT6300* and *INT6400* do not require a special SDRAM configuration file and so program `int6kboot <int6kboot.7.html>`_ may be used in place of `int6kf <int6kf.7.html>`_ which is described here. The only real difference between the two programs is the presence or absence of option :command:`-C` on the command line.

The `int6kf <int6kf.7.html>`_ program can program a blank or corrupted NVRAM on the INT6300. The process involves starting the runtime firmware as shown in the previous example then downloading a .nvm file using a ``VS_WR_MOD`` message, downloading a .pib file using another WR_WR_MOD message then writing both to NVRAM using a ``VS_MOD_NVM`` message. An example `int6kf <int6kf.7.html>`_ command line is shown below. It is identical to the one shown above except for the addition of the flash option at the end.

Flash NVRAM with int6kf
-----------------------
.. code-block:: console

    $ int6kf -C abc.cfg -N abc.nvm -P abc.pib -F

The example above starts the firmware then uses it to program the NVRAM. This was done in one step rather than two seperate steps as shown in the examples before it. The only difference is that :command:`int6k` needs to use the 'force flash' option when using downloaded firmware to write NVRAM. The following example accomplishes the same thing as the previous example but uses :command:`int6k` to program NVRAM. See the `int6k <int6k.7.html>`_ man page for an explanation of the command line options and arguments used here.

The `int6kf <int6kf.7.html>`_ program cannot program blank or corrupted NVRAM on the INT6000 unless the :program:`Softloader` is running. Since the :program:`Softloader` resides in NVRAM, the NVRAM must be either pre-programmed before use or removed and externally re-programmed if it corrupts.

Flash NVRAM with int6k
----------------------
.. code-block:: console

    $ int6kf -C abc.cfg -N abc.nvm -P abc.pib
    $ int6k -N abc.nvm -P abc.pib -FF

.. _procedure-uart-pairing:

Pairing UART Devices post PTS
=============================

This procedure describes how to pair two UART-enabled powerline adapters once they have been programmed,  personalized and tested. This example assumes you are using a Windows host and two powerline adapters having MAC addresses ``00:B0:52:00:05:F9`` and ``00:B0:52:00:05:FA``.

Start with the first device, ``00:B0:52)(00:05:F9``. Plug the host serial cable into the RJ45 port on the device and plug the device into an AC power source.

Change to transparent mode using program int6kuart. Option -v displays outgoing command and incoming serial responses. The "+++" is the serial data sent to the device and the "OK" is the device response.
.. code-block:: console

    $ int6kuart -s com3 -wv
    +++
    OK

Read the adapter PIB into a temporary file using program  `int6kuart <int6kuart.7.html>`_. The following command reads the PIB from serial device :file:`com3` and saves it to file :file:`temp.pib` on the host. You will see a series of dots appear on the console as the PIB is read from the device. The filename used is not important.
.. code-block:: console

    $ int6kuart -s com3 -p temp.pib
    \....................

Program the PIB file with the default destination address using program `setpib <setpib.7.html>`_. The following command inserts the MAC address of the second device, ``00B0520005FA``, at fixed offset ``0x2C8A`` in file :file:`temp.pib` downloaded in the previous step.
.. code-block:: console

    $ setpib temp.pib 2C8A data 00B0520005FA

Write the modified PIB file back to the adapter and commit it to flash memory using program `int6kuart <int6kuart.7.html>`_. The following command write file :file:`temp.pib` to serial device :file:`com3` then commits the PIB to flash memory. You will see a series of dots appear on the console as the PIB is written to the device. The device will reboot.
.. code-block:: console

    $ int6kuart -s com3 -P temp.pib -C2
    \....................

Change the device to Transparent Mode using program `int6kuart <int6kuart.7.html>`_ once the device resets.
.. code-block:: console

    $ inc6kuart -s com3 -wv
    +++
    OK

Verify that the destination MAC address is correct using program `int6kuart <int6kuart.7.html>`_.
.. code-block:: console

    $ int6kuart -s com3 -d
    00B0520005FA

Repeat this process on the second adapter, ``00:B0:52:00:05:FA``, using the MAC address of the first device, ``00:B0:52:00:05:F9``, as the destination address.

.. _procedure-troubleshoot-problems:

Trouble-shooting Problems
=========================

The following steps may help determine why two devices cannot communicate via ethernet over the powerline.  They assume that you have successfully installed the Powerline Toolkit but are having trouble forming a powerline network.

.. this list is pretty hacky... I'm not sure why each of these labels creates an entirely new list!!!

.. _trouble-shoot-1:

1. On :program:`Linux`, run program :program:`ifconfig` to determine available interface names as described :ref:`earlier <procedure-select-interface-linux>`. The default interface name is ``eth1`` on Linux. You must use interface names, not numbers, on the Linux command line.

   On :program:`Windows`, run program `pcapdevs <pcapdevs.7.html>`_ to determine available interface numbers as described :ref:`earlier <procedure-select-interface-windows>`. The default interface number is ``2`` on Windows. You must use interface numbers,  not names,  on Windows command lines, even under Cygwin.

   In either case, :program:`Linux` or :program:`Windows`, if a powerline device is connected to any interface other than the default interface then you must specify the name or number on the command line each time you run `int6k <int6k.7.html>`_. In the following examples, we will use device numbers ``3`` and ``4`` to indicate two different interfaces on the same host.

.. _trouble-shoot-2:

2. Make sure both devices respond to ``int6k -ri3`` and ``int6k -ri4`` as described :ref:`earlier <script-example-device-identification>`. They should each display the software revision information. If you get no response then check your connections and network configuration. If you see "Bootloader" then the connection is good but the firmware is not running. If the firmware versions are different then make them the same.

   You must have running firmware to continue and the firmware revisions should be the same.

.. _trouble-shoot-3:

3. Make sure both devices respond to ``int6k -Ii3`` then ``int6k -Ii4`` as described :ref:`earlier <script-example-device-configuration>`. They should display about 10 lines of information showing their MAC, DAK and NMK.

   If the MAC is ``00:B0:52:00:00:01`` then either the default Atheros PIB was stored in NVRAM or you downloaded and flashed the generic Atheros PIB without changing the MAC address beforehand.

   If the MAC is ``00:B0:52:00:00:03`` then either no PIB was stored in NVRAM or you downloaded and attempted to flashed an invalid PIB. The firmware will disable powerline communications in this case.

.. _trouble-shoot-4:

4. If the devices have different NMKs they will not form a network.

   One way to form a network is to manually set the NMK on each device to a known value with ``int6k -Mi3`` then ``int6k -Mi4``. Each device will reset afterwards and may reset again when the other device is changed. Repeat :ref:`Step 3 <trouble-shoot-3>` and confirm that both devices have the same NMK and that "(HomePlugAV)" appears next to the NMK.

   Another way to form a network is to physically press the push-button on each device for 2 to 3 seconds or simulate a push-button press on each device with ``int6k -B1 -i3`` then ``int6k -B1 -i4``. The devices should reset several times and then form a network.

.. _trouble-shoot-5:

5. Make sure each device knows the other one is present on the network with ``int6k -mi3`` then ``int6k -Mi4``. Each device should show that a network exists and identify the other device as a station on that network. It may take up to 120 seconds for one device to find another and up to 5 minutes to recognise that the other device has left the network.

.. _trouble-shoot-6:

6. :ref:`Step 5 <trouble-shoot-5>` should also show the TX and RX PHY rates for each device. :program:`Ping` each host Interface from the other several times to generate traffic. The PHY Rates should then be non-zero. We use `efsu <efsu.7.html>`_ to generate :ref:`traffic <scripting-traffic>` inside our own scripts. You may elect to use another method.

.. _trouble-shoot-7:

7. Power down both devices then power them up. Wait about 20 seconds and confirm that you can :program:`ping` in either direction.

.. _slac-config:

SLAC Configuration
==================

Avitar can be used to configure a PLC device as either a normal PLC station or a SLAC PEV-PLC or SLAC EVSE-PLC station. In doing so, :program:`Avitar` changes several PIB values at the same time. This section illustrates how to configurat a PIB the same way using the toolkit.

The HomePlug Green PHY Specification refers to the ``APCM_SET_CCO.REQ`` and ``APCM_CONF_SLAC`` primatives in the SLAC protocol description. These are virtual operations to be implemented by the user. In both cases, one must read the PIB from the PLC device, edit the PIB, recompute PIB checksums and write the PIB back to the PLC device. This can be done manually, using :program:`Avitar`, or in a script, using the Powerline Toolkit as shown below, or programatically, using custom software.

PEV-PLC PIB Settings
--------------------

+---------+--------+--------------------+--------------------------------------+
|OFFSET   | LENGTH | SYMBOL             | DESCRIPTION                          |
+=========+========+====================+======================================+
| 0000F4  | 1      | CCoSelection       | 1: This sets CCo Selection to Never  |
+---------+--------+--------------------+--------------------------------------+
| 001653  | 1      | SLAC_EnableMode    | 1: This sets SLAC Selection to PEV   |
+---------+--------+--------------------+--------------------------------------+
| 001C98  | 4      | LowLinkSpeed_kbps  | 10240                                |
+---------+--------+--------------------+--------------------------------------+
| 001C9C  | 4      | HighLinkSpeed_kbps | 102400                               |
+---------+--------+--------------------+--------------------------------------+

The following shell script converts a factory PIB to a SLAC PEV by setting the CCO Mode to Never and the SLAC Mode to PEV. The User HFID is set to "PEV" for role identification, optionally. This PIB must be written into PEV-PLC flash memory for proper operation.

.. code-block:: bash

    #!/bin/sh
    # file: pev.sh
    # ========================================================================
    # convert a factory PIB to a SLAC PEV PIB by changing these PIB settings;
    # ------------------------------------------------------------------------
    setpib ${1} 74 hfid "PEV"
    setpib ${1} F4 byte 1
    setpib ${1} 1653 byte 1
    setpib ${1} 1C98 long 10240 long 102400

PEV-EVSE PIB Settings
---------------------

+---------+--------+--------------------+--------------------------------------+
|OFFSET   | LENGTH | SYMBOL             | DESCRIPTION                          |
+=========+========+====================+======================================+
| 0000F4  | 1      | CCoSelection       | 2: This sets CCo Selection to Always |
+---------+--------+--------------------+--------------------------------------+
| 001653  | 1      | SLAC_EnableMode    | 2: This sets SLAC Selection to EVSE  |
+---------+--------+--------------------+--------------------------------------+
| 001C98  | 4      | LowLinkSpeed_kbps  | 10240                                |
+---------+--------+--------------------+--------------------------------------+
| 001C9C  | 4      | HighLinkSpeed_kbps | 102400                               |
+---------+--------+--------------------+--------------------------------------+

The following shell script converts a factory PIB to a SLAC EVSE by setting the CCO Mode to Always and the SLAC Mode to EVSE. The User HFID is set to "EVSE" for role identification, optionally. This PIB must be written into EVSE-PLC flash memory for proper operation.

.. code-block:: bash

    #!/bin/sh
    # file: evse.sh
    # ========================================================================
    # convert a factory PIB to a SLAC EVSE PIB by changing these PIB settings;
    # ------------------------------------------------------------------------
    setpib ${1} 74 hfid "EVSE"
    setpib ${1} F4 byte 2
    setpib ${1} 1653 byte 2
    setpib ${1} 1C98 long 10240 long 102400

.. _slac-apcm-set-cco:

APCM_SET_CCO Primative
----------------------

This primative configures a PLC state by reading PIB from PLC, changing CCoSelection, computing new PIB checksum then writing the PIB back to PLC. It can be performed manually using Avitar, in a script using the Powerline Toolkit or programatically using custom software.

.. _slac-apcm-config-slac:

APCM_CONFIG_SLAC Primative
--------------------------

.. _procedure-slac-demonstration:

SLAC Demonstration
==================

This page explains how to install and use two Qualcomm Atheros Powerline Toolkit programs, `pev <pev.7.html>`_ and `evse <evse.7.html>`_, to demonstrate the HomePlug AV Signal Level Attenuation Characterization (SLAC) protocol.

On Microsoft Windows, the two programs are distributed in a seperate Microsoft installation file :file:`slac-utils-x.x.x.msi`. To install them on Microsoft Windows, double-click on the installation file. The programs will be installed in folder :file:`c:\\Program Files (x86)\\Qualcomm Atheros\\Powerline Toolkit` unless the user overrides the default settings durin installation. New users should add this folder to the ``PATH`` environment variable and define environment variable ``PLC`` to reference the Ethernet interface connected to their PLC device.

On Linux, the two programs are distibuted inside the full toolkit tarball :file:`plc-utils-x.x.x`. To install them on Linux, unpack the tarball and change directory to the package folder. Type ":command:`sudo make install`" to compile and install the toolkit. Type type ":command:`sudo make manuals`" to install man pages. New users should define environment variable ``PLC`` to reference the inteface connected to their PLC device.

.. note::
   In some cases QCA will distribute two Windows :file:`.exe` files and two monolithic :file:`.c` files. On Windows, copy the executable files to folder :file:`c:\\Program Files (x86)\\Qualcomm Atheros\\Powerline Toolkit` and update the system ``PATH`` and ``PLC`` environment variables. On Linux, compile the two source files using ":command:`gcc -o pev pev.c`" and ":command:`gcc -o evse evse.c`" and copy the two binary files to :file:`/usr/local/bin` then update the system ``PLC`` environment variable. You will need root priviledge to copy the binary files to the system folder and you must change the binary file permissions to ``4555`` in order to execute them as a non-root user.

Each program reads an optional configuration file on start-up. Program `pev <pev.7.html>`_ reads :file:`pev.ini` and program `evse <evse.7.html>`_ reads :file:`evse.ini`. Qualcomm Atheros does not provide these files but option ``-c``, on each program, can be used to print a template configuration file on stdout. Once a user settles on a convenient working folder, they should run command ":command:`pev -c > pev.ini`" to create the configuration file for program :program:`pev` and ":command:`evse -c > evse.ini`" to create the configuration file for program :program:`evse`.

A SLAC demonstration can be setup using two Qualcomm Atheros PL16 boards, two personal computers, a power strip and a powerline impairment of some kind. One PL16 board must be configured as a PEV and the other must be configured as an EVSE. Plug both PL16 boards into the powerstrip and connect each board to a different computer via Ethernet. Run program `pev <pev.7.html>`_ on the computer connected to the PEV configured board. Run program `evse <evse.7.html>`_ on the computer connected to the EVSE configured board. Program `evse <evse.7.html>`_ runs continuously waiting to service `pev <pev.7.html>`_ programs running on other computers. Program `pev <pev.7.html>`_ runs until charging occurs then exits.

On the EVSE host, start program `evse <evse.7.html>`_ as shown below. It will run until cancelled.

.. code-block:: console

    $ evse
    evse: UnoccupiedState: Listening ...

Messages will appear on the console as events occur. The first field is the program name, in this case "evse". The second field is the function name, in this case "UnoccupiedState". In this case, program :program:`evse` does nothing until some PEV trys to associate.

On the EVSE host, run program `evse <evse.7.html>`_ as shown below. It will run until it detects and connects to an available EVSE-HLE. Use option :command:`-w` to vary probe timeouts.

.. code-block:: console

    $ pev
    pev: DisconnectedState: Probing ...
    pev: pev_cm_slac_param: --> CM_SLAC_PARAM.REQ
    pev: pev_cm_slac_param: <-- CM_SLAC_PARAM.CNF

Messages will appear on the console as events occur. The first field is the program name, in this case "pev". The second field is the function name, in this case ``DisconnectedState`` and ``pev_cm_slac_param``. The right arrow indicates that function ``pev_cm_slac_param`` is sending a ``CM_SLAC_PARAM.REQ`` message. If things work properly, you will see a another message indicating that function ``pev_cm_slac_param`` is receiving a ``CM_SLAC_PARAM.CNF`` message.

.. image:: IMG_20130227_145749.jpg
   :alt: IMG_20130227_145749.jpg

.. image:: IMG_20130227_151458.jpg
   :alt: IMG_20130227_151458.jpg

