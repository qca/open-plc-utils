.. _hardware:

Hardware
########

.. _hardware-intro:

Introduction
============

Atheros Communications, Ocala FL USA designs and manufactures chipsets that permit network communications over powerline. They do no not manufacture powerline communication products for market. Instead, they provide chipsets, reference designs and expertise needed to build powerline communications products. Atheros does manufacture some sample products, in various form factors, for evaluation purposes only.

.. _hardware-form-factors:

Device Form Factors
===================

Wall Adapters
    A small unit that plugs into any power outlet and has one RJ45 Ethernet jack. The RJ45 jack can be used to connect the unit to another Ethernet device, such as a hub, switch or computer network interface card using CAT5 Ethernet cable. Two or more such units can be used to connect Ethernet devices over the powerline. For example, a computer on one room can be connected to a network printer in another room. See Atheros Product Brief 27003417 - RD6300-ETH HomePlug AV Wall Adapter Reference Design for more information.

Desktop Adapters
    A compact unit having a 6ft power cord, one threaded coax cable F connector and one RJ45 Ethernet jack. Two such units can be used to connect Ethernet devices over either powerline (as above) or coax cable. Use of coax cable improves performance over longer distances and permits network segments to be isolated from each other. See Atheros Product Brief 27002824 - RD6000-ETH HomePlug AV Ethernet Hybrid Adapter Reference Design for more information.

PCI Cards
    A standard PCI compliant computer card having one DIN powerline connector and one threaded coax cable F connector. The card can be inserted into a PCI bus slot on a computer, or other device. Atheros provides device drivers that make the card behave like an ordinary Ethernet card. Like the desktop unit (above), computers and embedded systems can be connected either over powerline or coax cable. See Atheros Product Brief 27003239 - EK6000-PCI HomePlug AV Card Evaluation Kit for more information.

    PCI cards are no longer available from Atheros but reference designs may be obtained from selected Atheros customers that specialize in PCI designs.

Mini-PLC
    A Mini-PCI form factor card that integrates most components needed to embed HomePlug AV into your product design. Although the card will insert into a Mini-PCI slot, it is not electrically compatible with the Mini-PCI standard. Atheros uses this card in both the RD6000-ETH Desktop Adaptor and the EK6000-PCI Card described above. See Atheros Product Brief *27002835 - RD6000-PLC HomePlug AV Mini-PLC Module Reference Design* for more information.

    Unlike Wall and Desktop Adapters, the Mini-PLC Cards require a device driver written for the particular operating system used. Atheros provides a device drivers for the Windows XP Operating System and for the Linux 2.4 and Linux 2.6 kernel. These drivers make the PCI card look like an ordinary Ethernet interface card.

    Mini-PLC cards are no longer available from Atheros but cards may be obtained from selected Atheros customers that specialize in PCI designs.

Embedded Systems
    Atheros offers an expanding family of reference designs for powerline-enabled switches and routers. Most include onboard CPU and enablement software based on Linux, OpenWRT and other suitable operating systems. Atheros will assist customers in adapting the basic hardware and software to suite particular markets.

Chipsets
    Atheros offers an expanding family of Powerline enabled chipsets. Several SoC chipsets are also planned to support a variety of communications applications.

.. _hardware-device-communications:

Device Communications
=====================

Atheros powerline communication chipsets serve as a transparent bridges between an Ethernet network and an active powerline or passive coax cable, effectively extending the Ethernet network. HomePlug AV devices on the powerline, or at either end of a coax cable, will automatically detect each other and establish communications. Normal Ethernet frames that are detected by one HomePlug AV device are passed over powerline or coax to other HomePlug AV devices which then pass the frames on to any Etherenet devices that may be connected to them.

There are three levels of communication.

Powerline Communications
    HomePlug AV devices use a proprietary protocol defined by the HomePlug Powerline Alliance. In most cases, HomePlug AV communications do not leave the powerline or coax media used to connect devices. Connected devices use this protocol to detect each other, establish connection, encapsulate Ethernet frames and route them between devices. This level of communications is proprietary and hidden. See the HomePlug Powerline Alliance *HomePlug AV Specification* for more information.

Atheros Device Communications
    Atheros devices use a subset of the HomePlug AV protocol, mentioned above, to communicate with a local host processor. The subset is known as
    ``vendor-specific messages``
    . Atheros vendor-specific messages are intercepted and processed by Atheros devices. In some cases, they are forwared over powerline or coax to other Atheros devices. Atheros vendor-specific messages are used to interrogate, synchronize, configure and control Atheros devices without affecting HomePlug AV devices from other manufacturers. See the Atheros *HomePlug AV Firmware Technical Reference Manual* for more information.

Network Traffic
    This is the normal network traffic that passes transparently from local Ethernet, over powerline or coax, to remote Ethernet through HomePlug AV devices.

.. _device-configurations:

Device Configurations
=====================

There are several test configurations that can be used to experiment with Atheros powerline communication devices. Configurations vary based on the powerline communications devices you have available to work with. All configurations described here require at least one computer with an Ethernet card and the Open Powerline Toolkit installed. Most configurations require two Atheros powerline devices.

Open Powerline Toolkit programs let the user specify which Ethernet interface card to use when sending and receiving Ethernet frames. This means that a computer with two interface cards installed can emulate two computers provided there are not internal routing conflicts. To avoid routing conflicts, Atheros recommends that you start with two computers until your are ready for more sophisticated experimentation.

Open Powerline Toolkit programs default to ``eth0``	. This allows the computer to be connected to the normal network on ``eth0`` and connected to the powerline network on ``eth1``. To over-ride the default powerline interfaces, set environment variable ``PLC`` to the desired interface name. All configurations assume that the Ethernet card is installed, the Ethernet driver for that card is loaded and the correct interface is enabled.

Atheros powerline communication devices radiate across powerline or coax at radio frequencies. If two devices are connected, in any way, without intermmediate filters or isolation, they will attempt to commicate. Additionally, they will attempt to circumvent certain types of powerline noise or competing frequencies which can cause reduce data rates. Atheros recomends that devices under test (DUT) take their power from a shared but isolated power source like an isolation power strip or an uninterruptable power supply.

( ... explain about powerline isolation ... ) The power strip should have no filtering, surge protectors or electronic cirtuits inside.

::

    ---> [ATTENUATOR] ---> [POWER_STRIP] ---> [POWERLINE_DEVICE] --->

Typical configurations can be found in the sections below.

.. _configuration-1:

Local Host to Local Device
--------------------------

This is the simplest configuration. It establishes an Ethernet connection between the host and one powerline device. It can be used to test or program a single powerline device.

It requires

- One host computer with an Ethernet interface card

- One CAT-5 Ethernet cable with an RJ-45 connector at either end.

- One Atheros powerline device with RJ-45 connector.

- An isolated power source

::

    [LOCAL_HOST] ---/ ethernet /-----> [POWERLINE_DEVICE] ---/ powerline /----->

Connect the local host to the powerline device with an ordinary CAT-5 Ethernet cable. Apply power to the powerline device. The local host *cannot ping the powerline device* because it functions at the data link layer. The local host can interrogate and control the powerline device using
`int6k <int6k.7.html>`_
or
`int6k2 <int6k2.7.html>`_
programs.

- Type "int6k -r" and note the hardware and firmware revision.

- Type "int6k -I" and note the device MAC, DAK and NMK.
  .. COMMENT: It must have a unique MAC address and must share the same NMK as the other devices on it's logical network.

- Type "int6k -m" and confirm that the device detects no other devices indicating proper powerline isolation.

.. _configuration-2:

Local Host to Remote Device
---------------------------

This configuration is the simplest *powerline* network configuration. It expands the previous configuration by creating a simple powerline network having two powerline devices. One device, the "local device", is connected to the host via Ethernet. A second device, the "remote device", is connected to the first via powerline.

It requires

- One host computer with an Ethernet interface card

- One CAT-5 Ethernet cable with an RJ-45 connector at either end.

- Two Atheros powerline devices, one with RJ-45 connector.

- An isolated power source.

::

    [LOCAL_HOST] ---/ ethernet /-----> [POWERLINE_DEVICE] ---/ powerline /----->
                                       [POWERLINE_DEVICE] ---/ powerline /----->

Configure the previous network then plug a second powerline device into the same power source as the first powerline device. The local host still cannot ping any Ethernet network devices because there are no remote Ethernet devices to ping but it can interrogate and control both powerline devices.

.. _configuration-3:

Local Host to Remote Host
-------------------------

This configuration is the simplest *Ethernet* network configuration. It expands the previous network by connecting the second powerline device to an existing Ethernet network through an Ethernet switch.

It requires

- Two host computers, each with an Ethernet interface card

- Two CAT-5 Ethernet cables with RJ-45 connectors at either end.

- Two Atheros powerline devices, each with RJ-45 connector.

- An isolated power source.

::

    [LOCAL_HOST]  ---/ ethernet /-----> [POWERLINE_DEVICE] ---/ powerline /----->
    [REMOTE_HOST] ---/ ethernet /-----> [POWERLINE_DEVICE] ---/ powerline /----->

Configure the previous network then plug the second powerline device into an Ethernet switch connected to an exiting Ethernet network. The local host can now ping other Ethernet network devices on the.

.. _hardware-workstation:

Powerline Workstations
======================

The Open Powerline Toolkit is a collection of independent programs. Individually, they perform basic but useful operations on powerline communication devices and associated support files such as PIB and NVM files. Collectively, they can perform many types of engineering experiments, functional tests and production tasks. Their simplicity and high degree of flexibility lets customers adapt an off-the-shelf linux host to meet a wide range of production requirements. We call this configuration a "powerline workstation".

This section explains how to configure a powerline workstation and setup the Open Powerline Toolkit on that workstation. It covers some necessary aspects of Linux and the Toolkit but it is not a Linux tutorial or a Open Powerline Toolkit tutorial. Linux essentials are covered on the Internet and Open Powerline Toolkit essentials are covered in other sections of this documentation and on-line man pages. Although some typical configurations are illustrated, many variations are possible and are left to the customer to develop based on our examples. There is no single correct way to do anything.

.. _hardware-host-hardware:

Host Hardware
-------------

A powerline workstation host has no special hardware requirements. Any host capable of running Linux and supporting multiple Ethernet cards will do. For example, a 450mhz CPU having 128mb of memory, one 3gb disk and three 10/100 Ethernet cards is adequate.

Production tasks such as device initialization or firmware upgrade require one Ethernet card. Experimentation and functional testing typically require two Ethernet cards. Atheros recommends three Ethernet cards so that the host can communicate with other hosts over a local area network while talking to powerline devices. Atheros also recommends that all Ethernet cards installed support at least 100mbps and be of the same type to simplify network configuration.

.. _hardware-host-software:

Host Software
-------------

Atheros recommends installiing a Debian-based or Ubuntu-based Linux distribution due to the simplicity of network configuration. Redhat-based or SuSE-based distributions are also acceptable. A complete GNU toolchain is required to compile and install the Open Powerline Toolkit. Atheros uses GNU :program:`make` 3.8.0, GNU :program:`gcc` 3.3.5 and GNU :program:`ld` 2.15. If these components are not installed then you must install them. Linux system installation and configuration is beyond the scope of this documentation but there is a wealth of information available on the Internet.

Of course, the Open Powerline Toolkit needs to be installed and successful installation proves that all required Linux components are installed correctly. See :ref:`Installation on Linux <install-linux>` for more information on how to install the Open Powerline Toolkit.

.. _hardware-network:

Network Configuration
---------------------

Linux will assign interface names like ``eth0``, ``eth1`` and ``eth2`` to each installed network card. Atheros recommends that ``eth0`` be connected to your local network so that you can communicate with other hosts on that network. The other two interface cards can then be connected to Atheros devices that are plugged into an isolated power-strip. Of course, one CAT-5 Ethenet cable will be needed for each Ethernet card installed.

Interfaces ``eth1`` and ``eth2`` should be assigned IP addresses on a separate sub-net so that you can :program:`ping` one card from the other over the powerline without sending traffic over the local network. Remember that powerline devices have MAC addresses but not IP addresses. Also,  Linux :program:`ping` uses the routing table to route messages and so you may need to use the ``-I`` option when pinging over the powerline. Otherwise, ping packets may go out over the local network by default.

.. code-block:: console

    $ ifconfig
    eth0      Link encap:Ethernet  HWaddr 00:50:04:A5:D9:5A
              inet addr:192.168.99.12  Bcast:192.168.99.255  Mask:255.255.255.0
              UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1

    eth1      Link encap:Ethernet  HWaddr 00:01:03:2B:03:67
              inet addr:192.168.101.10  Bcast:192.168.101.255  Mask:255.255.255.0
              UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1

    eth2      Link encap:Ethernet  HWaddr 00:01:03:2B:03:73
              inet addr:192.168.101.11  Bcast:192.168.101.255  Mask:255.255.255.0
              UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1

    lo        Link encap:Local Loopback
              inet addr:127.0.0.1  Mask:255.0.0.0
              UP LOOPBACK RUNNING  MTU:16436  Metric:1

The abbreviated :program:`ifconfig` console display,  shown above,  illustrates a typical Ethernet configuration using three cards,  as recommended by Atheros. Interface ``eth0`` is on the ``192.168.99.0`` subnet which serves as the local network, in this case. Interfaces ``eth1`` and ``eth2`` are both on the ``192.168.101.0`` subnet which serves as the powerline network, in this case.

Although not required, installing both :program:`wireshark` and :program:`tshark` is a great idea because they can be used to monitor and log network traffic on any or all of the Ethernet interfaces during various operations.

.. _hardware-powerstrip:

Isolated Power-strip
--------------------

Atheros devices have a way of finding each other over powerline and sometimes across nearby powerlines. Power-strip isolation prevents cross-talk with other powerline devices that may be plugged into nearby. Proper isolation is not critical when getting started but can be critical in technical evaluation and production environments.

There are many ways to isolate powerline devices. One way is to plug the powerline workstation and the power-strip into an Uninterruptable Power Supply (UPS). Atheros also provides several reference designs for both expensive and inexpensive hardware that can be used to isolate devices and workstations.

Atheros powerline devices tend to work best when there is some signal attenuation over powerline or coax connections. Engineering evaluation configurations should insert some type of variable attenuation between powerline devices to measure the performance of their own powerline device designs. Consult with your Atheros Field Application Engineer on this matter.

.. _hardware-send-to-self:

Send-to-self Patch
==================

One advantage of *Linux* powerline workstations is the ability to control the low-level networking environment. ISO Layer 2 traffic can be easily directed from one Ethernet interface to another on the same host but Layer 3 traffic is a different matter because routing software merely routes this type of traffic internally.

A *Linux* kernel `patch <http://www.ssi.bg/~ja/#loop>`_ is available that will allow ISO Layer 3 traffic to be routed from one Ethernet interface to another on the same host. With this patch, multiple instances of a traffic generator,  like :program:`ttcp` or :program:`iperf`, can be effectively deployed on the same host without modification.

This patch is useful for testing on a closed network but it could pose a security risk to the local host when connected to a public network. Kernels having this patch installed should have a special designation such as "linux-2.6.28-send-to-self" so that users are aware that the patch is installed.

"send-to-self" Patch Description
--------------------------------

The following is the full, original patch description.

::

		Send-To-Self interface flag
		Julian Anastasov <ja@ssi.bg>, July 2003

		Patches for different kernels:

		send-to-self-2.4.21-1.diff
		send-to-self-2.5.73-1.diff

		The  presented patch implements routing of traffic between local
	IP addresses externally via ethernet interfaces. This patch is basically
	the Ben Greear's send-to-self work but reimplemented entirely on routing
	level.   The idea is  to return output route  via external interfaces if
	path between two local IP addresses is requested and they are configured
	on different interfaces with /proc/sys/net/ipv4/conf/DEVNAME/loop set to
	1.    As  result,  arp_filter  (if  enabled  -  the  recommended  value)
	automatically  accepts  the ARP  requests  on the  right  interface. The
	rp_filter  check is modified to accept traffic from such interfaces with
	local  IP as sender, so using loop=1 for interfaces attached to insecure
	mediums is not recommended.

	Pros:
	- it can be used from all existing applications without change
	- it is not limited to 2 interfaces
	- you can use it with many IP addresses
	- does not depend on the rp_filter and arp_filter states, they
	can be set to 1
	- the packets are not altered in any way, useful for QoS testings
	- the routing result is cached, the routing checks are not per packet

	Cons:
	- not possible to use it for interfaces attached to insecure
	mediums (the rp_filter protection allows saddr to be local IP).
	By design. Use at your own risk.

		The usage is simple:

	# Connect two or more interfaces to same hub or via crossover cable

	# Enable loopback mode for eth0 and eth1. This even can be
	# default mode without breaking any other talks. By this way
	# we allow external routing only between local IPs configured
	# on the specified interfaces.

	echo 1 > /proc/sys/net/ipv4/conf/eth0/loop
	echo 1 > /proc/sys/net/ipv4/conf/eth1/loop

	# Add some IP addresses for testing, eg. client and server IP

	ip address add 192.168.1.1 dev eth0
	ip address add 192.168.2.1 dev eth1

	# Testing with applications that are aware of this binding.
	# The main thing the apps need to know is what src and dst IP
	# addresses to use. The client app needs to bind to the src IP
	# and by this way to request output route to the dst IP. There
	# is no specific configuration for the server app listening on
	# 192.168.2.1

	ping -I 192.168.1.1 192.168.2.1

	# Note that specifying the output device (SO_BINDTODEVICE is
	# not recommended)


	# Testing with applications that are not aware of this feature:
	# for 192.168.1.1 client (the same for the server is not needed).
	# Note that by default, in local routes the kernel uses the local
	# IPs as preferred source. This is the safe default mode (if loop=1)
	# for applications that do not care what src IP will be used
	# for their talks with local IPs. We try to change that and to
	# use IPs from different interfaces.

	ip route replace local 192.168.2.1 dev eth1 scope host src 192.168.1.1 proto kernel

	# but for any case, here it is and for the "server":

	ip route replace local 192.168.1.1 dev eth0 scope host src 192.168.2.1 proto kernel

	# Testing it:

	ping 192.168.2.1
	ping -I 192.168.1.1 192.168.2.1
	telnet 192.168.2.1

	# Note that by replacing the local route's preferred source IP address
	# we help the IP address autoselection to select proper IP to the
	# target, in our case, route via eth


"send-to-self" Patch Application
--------------------------------

The following example illustrates how to use :program:`iperf` to perform TCP and UDP traffic measurements once this patch is installed. We illustrate the use of :program:`iperf` but do not necessarily endorse it for traffic measurement. We also illustrate the use of two interfaces but the "send-to-self" patch will support additional interfaces. We also illustrate the use of environment variables so that procedures can execute on different hosts without modification but these environment variables are not required.

First, we define environment variables, :envvar:`IF1` and :envvar:`IF2`, for each Ethernet interface and, :envvar:`IP1` and :envvar:`IP2`, for their IP addresses. Each interface must be on a separate IP subnet. We export definitions here so that they are accessible to this process and any subprocesses, such as shell scripts. Do whatever is appropriate for your environment.

.. code-block:: console

    $ export IF1=eth1
    $ export IF2=eth2
    $ export IP1=192.168.1.1
    $ export IP2=192.168.2.2

Next, we assign the IP addresses to the interfaces using program :program:`ifconfig`. There are other ways to do this. Observe that we reference our environment variables on the command line.

.. code-block:: console

    $ ifconfig ${IF1} ${IP1}
    $ ifconfig ${IF2} ${IP2}

Next, we suppress internal routing between local interfaces. The ``loop`` propery only exists on kernels that have the "send-to-self" patch installed and have the :file:`/proc` filesystem mounted. Some systems may not mount this file system.

.. code-block:: console

    $ echo 1 > /proc/sys/net/ipv4/conf/${IF1}/loop
    $ echo 1 > /proc/sys/net/ipv4/conf/${IF2}/loop

Alternately, you could edit file :file:`/etc/sysctl.conf`, as follows, to set the ``loop`` property for each interface during system startup. Again, the ``loop`` propery only exists on kernels that have the "send-to-self" patch installed and so errors will occur if you boot another kernel that does not have it installed.

::

    net.ipv4.conf.eth1.loop = 1
    net.ipv4.conf.eth2.loop = 1

Open a console window and start :program:`iperf` as a server. Option ``-s`` identifies this instance of :program:`iperf` as the server. Option ``-B`` binds this instance to one host  interface by IP address, in this case ``IP1`` defined earlier.

.. code-block:: console

    $ iperf -B ${IP1} -s
    ------------------------------------------------------------
    Server listening on TCP port 5001
    Binding to local address 192.168.1.1
    TCP window size: 85.3 KByte (default)
    ------------------------------------------------------------

Open a second console window and start :program:`iperf` as a client. Option ``-c`` identifies this instance of :program:`iperf` as a client. Option ``-B`` binds this instance to the one interface by IP address, in this case ``IP2`` defined earlier. The server address must also be specified, in this case ``IP1`` bound to the server in the last step.

.. code-block:: console

    $ iperf -B ${IP2} -c ${IP1}
    ------------------------------------------------------------
    Client connecting to 192.168.1.1, TCP port 5001
    Binding to local address 192.168.2.1
    TCP window size: 16.0 KByte (default)
    ------------------------------------------------------------
    [  3] local 192.168.2.1 port 5001 connected with 192.168.1.1 port 5001
    [ ID] Interval       Transfer     Bandwidth
    [  3]  0.0-10.0 sec  31.1 MBytes  26.0 Mbits/sec

"send-to-self" Patch Installation
---------------------------------

The "send-to-self" patch exists for several recent *Linux* kernel versions but not all versions. Assuming you have obtained the correct kernel archive and the correct patch version, the following script illustrates the steps needed to apply the patch on *Ubuntu 9.04* and recompile the kernel. Observe that, in this case, the patch version does not match the kernel version because a patch has not been published for that kernel version.

The following script can be used on a Ubuntu Linux distribution to download kernel source, the send-to-self patch, apply the patch then compile and install the resulting kernal image. When the :program:`menuconfig` screen appears:

#. Select ``General Setup`` on the "Linux Kernel Configuration" screen.

#. Select ``Local version - append to kernel release`` on the "General Setup" screen.

#. Enter the version suffix "-send-to-self".

#. Select ``ok`` to return to the "General Setup" screen.

#. Select ``Automatically append version information to the version string`` on the "General Setup" screen.

#. Select ``exit`` to return to the "Linux Kernel Configuration" screen.

#. Select ``exit`` to close the :program:`menuconfig` program.

#. Select ``yes`` if prompted to save your new kernel configuration. This message does not appear each time.

.. code-block:: bash

	#!/bin/bash
	# file: patches/send-to-self-2.6.28.sh

	# ====================================================================
	# environment variables;
	# --------------------------------------------------------------------

	VERSION=2.6.28
	CURRENT=9
	VARIANT=send-to-self
	PACKAGE=linux-source-${VERSION}
	ARCHIVE=${PACKAGE}.tar.bz2
	PATCH=send-to-self-2.6.26-1.diff

	# ====================================================================
	# extend version string;
	# --------------------------------------------------------------------

	if [ ! -z ${CURRENT} ]; then
		VERSION+=.${CURRENT}
	fi
	if [ ! -z ${VARIANT} ]; then
		VERSION+=-${VARIANT}
	fi

	# ====================================================================
	# install required software;
	# --------------------------------------------------------------------

	if [ ! -f ${ARCHIVE} ]; then
		wget http://www.ssi.bg/~ja/${PATCH}
		apt-get install ${PACKAGE}
	#	apt-get install ${PACKAGE} --reinstall
		apt-get install binutils patch gcc g++
		apt-get install ncurses-dev
		mv /usr/src/${ARCHIVE} .
	fi

	# ====================================================================
	# confirm archive file exists;
	# --------------------------------------------------------------------

	if [ ! -f ${ARCHIVE} ]; then
		echo "File ${ARCHIVE} is missing or misplaced"
		exit 1
	fi

	# ====================================================================
	# confirm patch file exists;
	# --------------------------------------------------------------------

	if [ ! -f ${PATCH} ]; then
		echo "File ${PATCH} is missing or misplaced"
		exit 1
	fi

	# ====================================================================
	# remove old kernel source if present;
	# --------------------------------------------------------------------

	if [ -d ${PACKAGE} ]; then
		echo "Removing old source ..."
		rm -fr ${PACKAGE}
	fi

	# ====================================================================
	# extract kernel source;
	# --------------------------------------------------------------------

	tar -vjxf ${ARCHIVE}
	if [ ! -d ${PACKAGE} ]; then
		echo "Folder ${PACKAGE} does not exist"
		exit 1
	fi
	cd ${PACKAGE}

	# ====================================================================
	# patch kernel source;
	# --------------------------------------------------------------------

	patch -p1 < ../${PATCH}

	# ====================================================================
	# compile kernel source;
	# --------------------------------------------------------------------

	make mrproper
	make menuconfig
	make

	# ====================================================================
	# install kernel source;
	# --------------------------------------------------------------------

	make modules_install
	make install

	# ====================================================================
	# install kernel source;
	# --------------------------------------------------------------------

	mkinitramfs -o /boot/initrd.img-${VERSION} ${VERSION}
	ln -fs config-${VERSION} /boot/config
	ln -fs initrd.img-${VERSION} /boot/initrd.img
	ln -fs System.map-${VERSION} /boot/System.map
	ln -fs vmlinuz-${VERSION} /boot/vmlinuz

.. note:: In case you don't know ...
   apt-get --reinstall
   The :program:`apt-get` program is only available on Debian-based distributions. If you do not use a Debian-based system then you must find another way to obtain the necessary packages. Option ``--reinstall`` instructs :program:`apt-get` to download the kernel even though it has been installed before. It is not needed on the first script execution but may be needed on subsequent script executions if you have deleted the kernel archive file.
   mkinitramfs
   This script uses :program:`mkinitramfs` instead of the :program:`mkinitrd`. This may differ on other distributions. The kernel source package used here has *Ubuntu* modifications that result in a minor version being appended to the kernel version. This may not happen with other distributions or with kernels obtained directly from kernel.org.
   cut-and-paste
   This script, or some like it, are included in the :file:`./patches` folder of the toolkit. You can also copy and paste this script but remember to edit the environment variables at the top, remove all carriage returns and set correct file permissions with ``chmod 0755`` before executing it on your *Linux* host. Run the script as ``root`` user.
   grub/menu.lst
   If your system uses :program:`grub` then edit file :file:`/boot/grub/menu.lst` and add a new reference to the new :file:`initrd.img`,  :file:`System.map` and :file:`vmlinuz` files installed in folder :file:`/boot` by this script. We recommend adding these references as the last ones in the file so that the new kernel does not start by default. Once you are confident that everything works, you can then move the references to the first entry. We also recommend setting the ``timeout`` value to ``10`` for now.

"send-to-self" Patch Listing
----------------------------

The following "send-to-self" patch is specifically for *Linux* kernel 2.6.30 and is provided for information only. For practical purposes, the patch has not changed much from version to version but the line numbers have changed. Some recent "send-to-self" patches are included in the toolkit :file:`./patches` folder.

.. code-block:: diff

	diff -urp v2.6.30/linux/Documentation/networking/ip-sysctl.txt linux/Documentation/networking/ip-sysctl.txt
	--- v2.6.30/linux/Documentation/networking/ip-sysctl.txt	2009-06-13 10:53:29.000000000 +0300
	+++ linux/Documentation/networking/ip-sysctl.txt	2009-06-13 15:54:15.000000000 +0300
	@@ -637,6 +637,13 @@ accept_redirects - BOOLEAN
	 forwarding - BOOLEAN
		Enable IP forwarding on this interface.
	 
	+loop - BOOLEAN
	+	By default (loop=0) the traffic between local IP addresses
	+	is routed via interface "lo". Setting this flag for two
	+	interfaces allows traffic between their IP addresses to
	+	be looped externally. This is useful for setups where the
	+	interfaces are attached to same broadcast medium.
	+
	 mc_forwarding - BOOLEAN
		Do multicast routing. The kernel needs to be compiled with CONFIG_MROUTE
		and a multicast routing daemon is required.
	diff -urp v2.6.30/linux/include/linux/inetdevice.h linux/include/linux/inetdevice.h
	--- v2.6.30/linux/include/linux/inetdevice.h	2009-06-13 10:53:56.000000000 +0300
	+++ linux/include/linux/inetdevice.h	2009-06-13 15:54:15.000000000 +0300
	@@ -106,6 +106,7 @@ static inline void ipv4_devconf_setall(s
		  IN_DEV_ORCONF((in_dev), ACCEPT_REDIRECTS)))
	 
	 #define IN_DEV_ARPFILTER(in_dev)	IN_DEV_ORCONF((in_dev), ARPFILTER)
	+#define IN_DEV_LOOP(in_dev)		IN_DEV_CONF_GET(in_dev, LOOP)
	 #define IN_DEV_ARP_ANNOUNCE(in_dev)	IN_DEV_MAXCONF((in_dev), ARP_ANNOUNCE)
	 #define IN_DEV_ARP_IGNORE(in_dev)	IN_DEV_MAXCONF((in_dev), ARP_IGNORE)
	 #define IN_DEV_ARP_NOTIFY(in_dev)	IN_DEV_MAXCONF((in_dev), ARP_NOTIFY)
	diff -urp v2.6.30/linux/include/linux/sysctl.h linux/include/linux/sysctl.h
	--- v2.6.30/linux/include/linux/sysctl.h	2009-06-13 10:53:56.000000000 +0300
	+++ linux/include/linux/sysctl.h	2009-06-13 15:54:40.000000000 +0300
	@@ -491,6 +491,7 @@ enum
		NET_IPV4_CONF_PROMOTE_SECONDARIES=20,
		NET_IPV4_CONF_ARP_ACCEPT=21,
		NET_IPV4_CONF_ARP_NOTIFY=22,
	+	NET_IPV4_CONF_LOOP=23,
		__NET_IPV4_CONF_MAX
	 };
	 
	diff -urp v2.6.30/linux/kernel/sysctl_check.c linux/kernel/sysctl_check.c
	--- v2.6.30/linux/kernel/sysctl_check.c	2009-06-13 10:53:57.000000000 +0300
	+++ linux/kernel/sysctl_check.c	2009-06-13 15:55:00.000000000 +0300
	@@ -220,6 +220,7 @@ static const struct trans_ctl_table tran
		{ NET_IPV4_CONF_PROMOTE_SECONDARIES,	"promote_secondaries" },
		{ NET_IPV4_CONF_ARP_ACCEPT,		"arp_accept" },
		{ NET_IPV4_CONF_ARP_NOTIFY,		"arp_notify" },
	+	{ NET_IPV4_CONF_LOOP,			"loop" },
		{}
	 };
	 
	diff -urp v2.6.30/linux/net/ipv4/devinet.c linux/net/ipv4/devinet.c
	--- v2.6.30/linux/net/ipv4/devinet.c	2009-06-13 10:53:58.000000000 +0300
	+++ linux/net/ipv4/devinet.c	2009-06-13 15:55:22.000000000 +0300
	@@ -1449,6 +1449,7 @@ static struct devinet_sysctl_table {
			DEVINET_SYSCTL_RW_ENTRY(ARP_IGNORE, "arp_ignore"),
			DEVINET_SYSCTL_RW_ENTRY(ARP_ACCEPT, "arp_accept"),
			DEVINET_SYSCTL_RW_ENTRY(ARP_NOTIFY, "arp_notify"),
	+		DEVINET_SYSCTL_RW_ENTRY(LOOP, "loop"),
	 
			DEVINET_SYSCTL_FLUSHING_ENTRY(NOXFRM, "disable_xfrm"),
			DEVINET_SYSCTL_FLUSHING_ENTRY(NOPOLICY, "disable_policy"),
	diff -urp v2.6.30/linux/net/ipv4/fib_frontend.c linux/net/ipv4/fib_frontend.c
	--- v2.6.30/linux/net/ipv4/fib_frontend.c	2009-06-13 10:53:58.000000000 +0300
	+++ linux/net/ipv4/fib_frontend.c	2009-06-13 15:54:15.000000000 +0300
	@@ -239,16 +239,17 @@ int fib_validate_source(__be32 src, __be
						.tos = tos } },
				    .iif = oif };
		struct fib_result res;
	-	int no_addr, rpf;
	+	int no_addr, rpf, loop;
		int ret;
		struct net *net;
	 
	-	no_addr = rpf = 0;
	+	no_addr = rpf = loop = 0;
		rcu_read_lock();
		in_dev = __in_dev_get_rcu(dev);
		if (in_dev) {
			no_addr = in_dev->ifa_list == NULL;
			rpf = IN_DEV_RPFILTER(in_dev);
	+		loop = IN_DEV_LOOP(in_dev);
		}
		rcu_read_unlock();
	 
	@@ -258,6 +259,11 @@ int fib_validate_source(__be32 src, __be
		net = dev_net(dev);
		if (fib_lookup(net, &fl, &res))
			goto last_resort;
	+	if (loop && res.type == RTN_LOCAL) {
	+		*spec_dst = FIB_RES_PREFSRC(res);
	+		fib_res_put(&res);
	+		return 0;
	+	}
		if (res.type != RTN_UNICAST)
			goto e_inval_res;
		*spec_dst = FIB_RES_PREFSRC(res);
	diff -urp v2.6.30/linux/net/ipv4/route.c linux/net/ipv4/route.c
	--- v2.6.30/linux/net/ipv4/route.c	2009-06-13 10:53:58.000000000 +0300
	+++ linux/net/ipv4/route.c	2009-06-13 15:54:15.000000000 +0300
	@@ -2521,6 +2521,11 @@ static int ip_route_output_slow(struct n
				dev_put(dev_out);
				goto out;	/* Wrong error code */
			}
	+		err = -ENETDOWN;
	+		if (!(dev_out->flags&IFF_UP)) {
	+			dev_put(dev_out);
	+			goto out;
	+		}
	 
			if (ipv4_is_local_multicast(oldflp->fl4_dst) ||
			    oldflp->fl4_dst == htonl(0xFFFFFFFF)) {
	@@ -2588,10 +2593,41 @@ static int ip_route_output_slow(struct n
		free_res = 1;
	 
		if (res.type == RTN_LOCAL) {
	-		if (!fl.fl4_src)
	-			fl.fl4_src = fl.fl4_dst;
	+		struct in_device *in_dev;
	+		__be32 src;
	+
			if (dev_out)
				dev_put(dev_out);
	+		dev_out = FIB_RES_DEV(res);
	+		in_dev = in_dev_get(dev_out);
	+		src = fl.fl4_src? : FIB_RES_PREFSRC(res);
	+		if (in_dev && IN_DEV_LOOP(in_dev) && src) {
	+			struct net_device *dev_src;
	+
	+			in_dev_put(in_dev);
	+			in_dev = NULL;
	+			dev_src = ip_dev_find(net, src);
	+			if (dev_src && dev_src != dev_out &&
	+			    (in_dev = in_dev_get(dev_src)) &&
	+			    IN_DEV_LOOP(in_dev)) {
	+				in_dev_put(in_dev);
	+				dev_out = dev_src;
	+				fl.fl4_src = src;
	+				fl.oif = dev_out->ifindex;
	+				res.type = RTN_UNICAST;
	+				if (res.fi) {
	+					fib_info_put(res.fi);
	+					res.fi = NULL;
	+				}
	+				goto make_route;
	+			}
	+			if (dev_src)
	+				dev_put(dev_src);
	+		}
	+		if (in_dev)
	+			in_dev_put(in_dev);
	+		if (!fl.fl4_src)
	+			fl.fl4_src = fl.fl4_dst;
			dev_out = net->loopback_dev;
			dev_hold(dev_out);
			fl.oif = dev_out->ifindex;

