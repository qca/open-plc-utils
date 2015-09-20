.. _support-functions:

Support Function Reference
##########################

.. _support-intro:

Introduction
============

The :program:`Open Powerline Toolkit` includes many general-purpose or multi-purpose functions that may appear in other Qualcomm Atheros applications or in third party packages. Consequently, we describe them separately and store them in separate folders withing the toolkit.

.. _support-command:

Command Line Functions
======================

Command line functions are used by all toolkit programs. They are generic functions but we include them in a special section because they collectively co-operate to convert command line options and arguments into appropriate binary representations and provide concise but meaningful feedback when that is not possible.

.. _support-assist:

assist
------

.. c:function:: void assist(char const *name, char const *type, const struct _code_ list[], size_t size, FILE *fp)

Show why a symbolic ``name`` was rejected by function :ref:`lookup <support-lookup>` or similar functions. Argument ``type`` contains a title for the class of names stored in ``list``. This function prints an error message on file stream ``fp`` showing the program name,  the ``type`` string,  the symbolic ``name`` and all names stored in the ``list``. The ``_code_`` structure is declared in `types.h <types.h.html>`_. The function is declared in `symbol.h <symbol.h.html>`_ and defined in `assist.c <assist.c.html>`_. Function :ref:`expect <support-expect>` is used to print list names.

.. _support-basespec:

basespec
--------

.. c:function:: uint64_t basespec(char const *string, unsigned base, unsigned size)

Return the unsigned integer equivalent of a numeric ``string``. Print an error message on ``stderr`` and exit the program with status ``1`` if a syntax error occurs or the result exceeds the capacity of the requested integer ``size`` in bytes. If ``base`` is ``0``,  numeric values may be expressed in decimal, hexadecimal or binary notation where hexadecimal values start with ``"0x"`` and binary values start with ``"0b"``. When ``base`` is non-zero, the notation in ``string`` must conform to the corresponding number base rules. Applications should cast the return value to the appropriate data type prevent loss-of-data compiler warnings. This function is typically used to convert and length-check integers entered as command line arguments. The function is declared in `number.h <number.h.html>`_ and defined in `basespec.c <basespec.c.html>`_.

Like function :ref:`uintspec <support-uintspec>`,  this function both converts and range checks numeric string values,  but the minimum and maximum value are implicit in the ``size`` of the integer. The minimum value is always ``0`` and the maximum value can be computed by  ``((1 << size << 3) - 1)``.

.. _support-bytespec:

bytespec
--------

.. c:function:: void bytespec(char const *string, void *memory, size_t extent)

Encode a ``memory`` region with the binary equivalent of a fixed-length hexadecimal string. Print an error message on stderr and exit the program with status ``1`` if a syntax error occurs or the number of octets does not equal  ``extent``. Hexadecimal octets may be separated by colons for readability but colons are not required. Empty octets are illegal. The function is declared in `memory.h <memory.h.html>`_ and defined in `bytespec.c <bytespec.c.html>`_.

This function is typically used to enter fixed-length data, like hardware addresses and encryption keys, on the command line.

.. _support-checkfilename:

checkfilename
-------------

.. c:function:: bool checkfilename(char const *filename)

Return logical ``true`` if the ``filename`` argument contains only letters, digits, slashes, periods, underscores and hyphens. This function can be used to detect cases where a user accidentally entered an Ethernet address in place of a filename on the command line. Ethernet address strings are, as it happens, also valid filenames. The function is declared in `files.h <files.h.html>`_ and defined in `checkfilename.c <checkfilename.c.html>`_.

.. _support-dataspec:

dataspec
--------

.. c:function:: void dataspec(char const *string, void *memory, size_t extent)

Encode a ``memory`` region with the binary equivalent of a variable-length hexadecimal string. Print an error message on stderr and exit the program with the status ``1`` if a syntax error occurs or the number of octets exceeds ``extent``. The number of octets may, however, be less than ``extent``. Unlike function :ref:`bytespec <support-bytespec>`, hexadecimal octets may not be separated by colons. This function is typically used to enter variable-length data from the command line. The function is declared in `memory.h <memory.h.html>`_ and defined in `dataspec.c <dataspec.c.html>`_.

.. _support-error:

error
-----

.. c:function:: void error(int exitcode, errno_t number, char const *format, ...)

This function works like printf except that printed messages appear on ``stderr`` and are prefixed with the program name and error information. If argument ``errno`` is non-zero then messages are prefixed with the system error description. If argument ``exitcode`` is non-zero then function ``error`` exits the program with value ``exitcode`` after printing the message and does not return to the caller. The function is declared in `error.h <error.h.html>`_ and defined in `error.c <error.c.html>`_.

This function is used to print informative error messages on the console and prevent program execution from proceeding when input is invalid or some error condition exists.

.. _support-expect:

expect
------

.. c:function:: void expect(const struct _code_ list[], size_t size, FILE *fp)

Display all names in argument ``list`` on file stream ``fp``. This function is called by runction :ref:`assist <support-assist>` to print the list of symbolic names but other function may have use for it. The ``_code_`` structure is declared in `types.h <types.h.html>`_. The function is declared in `symbol.h <symbol.h.html>`_ and defined in `expect.c <expect.c.html>`_.

.. _support-getoptv:

getoptv
-------

.. c:function:: int getoptv(int argc, char const *argv[], char const *optv[])

A custom version of the POSIX function ``getopt``. It supports standard global variables ``optind``, ``opterr``, ``optopt`` and ``optarg`` and the non-standard variable ``optmin``. It extracts the program name from ``argv[0]`` and sets global string pointer ``program_name`` for use by functions ``alert``, ``error`` and others. Options ``-?`` and ``--help`` both display program information on ``stdout``. Options ``-!`` and ``--version`` both display program version information on ``stdout``. String vector ``optv`` includes both the standard argument ``optstring`` and usage text found in many programs. The function is declared in `getoptv.h <getoptv.h.html>`_ and defined in `getoptv.c <getoptv.c.html>`_.

.. _support-ipv4spec:

ipv4spec
--------

.. c:function:: size_t ipv4spec(char const *string, void *memory)

Encode a 4-byte ``memory`` region with an IPv4 dotted-decimal ``string`` and return the number of bytes encoded. Terminate the program with an error message and exitcode of ``1`` on conversion error. The value returned by this function is always ``4`` and memory is always encoded in network byte order. This function is typically used to convert IPv4 strings entered as command line arguments. The function is declared in `memory.h <memory.h.html>`_ and defined in `ipv4spec.c <ipv4spec.c.html>`_.

Dotted-decimal format consists of decimal values in the range 0 through 255. Each value represents one octet or 8-bit value. IPv4 addresses require 4 such values separated by one decimal point. This function permits empty octets and leading zeros within octets. For example,  "..." is equivalent to "0.0.0.0" and "127.0.000.001" is equivalent to "127.0.0.1". The second example will encode memory as follows ``{ 0x7F, 0x00, 0x00, 0x01 }`` which is in network byte order, or big endian.

.. _support-ipv6spec:

ipv6spec
--------

.. c:function:: size_t ipv6spec(char const *string, void *memory)

Encode a 16-byte ``memory`` region with an IPv6 colon-separated hexadecimal quartet ``string`` and return the number of bytes encoded. Terminate the program with an error message and exitcode of ``1`` on conversion error. The value returned by this function is always ``16`` and memory is always encoded in network byte order. This function is typically used to convert IPv6 strings entered as command line arguments. The function is declared in `memory.h <memory.h.html>`_ and defined in `ipv6spec.c <ipv6spec.c.html>`_.

Colon-separated hexadecimal quartet notation consists of hexadecimal values in the range 0 through FFFF. Each value represents a quartet or a 32-bit value. IPv6 addresses require 8 quartets separated by one colon. By convention, an empty quartet expands with enough zeros to right-justify the remainder of the address. This function permits multiple empty quartets and leading zeros within quartets. When multiple empty quartets appear, only the right-most occurance expands to zeros. For example,  "AA12::BB34::CC56::DD78" is equivalent to "AA12:0000:BB34:0000:CC56:0000:0000:DD78" because only the right-most empty field expands. This will encode memory as follows ``{ 0xAA, 0x12, 0x00, 0x00, 0xBB, 0x34, 0x00, 0x00, 0xCC, 0x56, 0x00, 0x00, 0x00, 0x00, 0xDD, 0x78 }`` which is in network byte order, or big-endian.

.. _support-lookup:

lookup
------

.. c:function:: signed lookup(char const *name, const struct _code_ list[], size_t size)

Lookup a symbolic ``name`` in a ``list`` and return an associated integer or ``-1`` if the ``name`` does not appear in the ``list``. A typical use is the translation of symbolic command line arguments to integer codes. For example, some Toolkit programs assign symbolic names to field codes so that users can enter names instead of numbers. This approach becomes more useful as the number of permitted codes increases. The ``_code_`` structure is declared in `types.h <types.h.html>`_. The function is declared in `symbol.h <symbol.h.html>`_ and defined in `lookup.c <lookup.c.html>`_.

This function offers two benefits: 1) multiple symbols can translate to one code and 2) names can be changed or new names added without affecting program logic. This function is similar to but different from function :ref:`synonym <support-synonym>` which returns a character string instead of an integer.

.. _support-putoptv:

putoptv
-------

.. c:function:: void putoptv(char const *optv[])

Print program information on ``stdout``. Program information is stored in string vector ``optv``. String indexes are defined in file :file:`putoptv.h`. String ``optv[0]`` is the POSIX standard argument ``optstring``. This function is called by function ``getoptv`` whenever option ``-?`` or ``--help`` is detected on the command line. There is virtually no need to call this function directly. The function is declared in `putoptv.h <putoptv.h.html>`_ and defined in `putoptv.c <putoptv.c.html>`_.

.. _support-synonym:

synonym
-------

.. c:function:: char const *synonym(char const *string, const struct _term_ list[], size_t size)

Lookup a symbolic ``name`` in a ``list`` and return an associated string or the original ``string`` if the ``name`` does not appear in the ``list``. A typical use is the translation of symbolic command line arguments to their equivalent numeric strings before encoding them. For example, many Toolkit programs convert the command line argument "local" to "00:B0:52:00:00:01" before encoding the device MAC address. The ``_term_`` structure is declared in `types.h <types.h.html>`_. The function is declared in `symbol.h <symbol.h.html>`_ and defined in `synonym.c <synonym.c.html>`_.

This function is similar to but different from function :ref:`lookup <support-lookup>` which returns an integer instead of a character string.

.. _support-uintspec:

uintspec
--------

.. c:function:: uint64_t uintspec(char const *string, uint64_t minimum, uint64_t maximum)

Return the unsigned integer equivalent of a numeric ``string``. Print an error message on ``stderr`` and exit the program with the value ``1`` when a syntax error occurs or the result exceeds the specified ``minimum`` or ``maximum`` value. Numeric values may be expressed in decimal, hexadecimal or binary notation where hexadecimal values start with ``"0x"`` and binary values start with ``"0b"``. Applications should cast the return value to the appropriate data type to avoid loss-of-data warnings on some compilers. This function is typically used to convert and range-check integer values entered as command-line arguments. The function is declared in `number.h <number.h.html>`_ and defined in `uintspec.c <uintspec.c.html>`_.

.. _support-version:

version
-------

.. c:function:: void version(void)

Print package and program version information on ``stdout``. This function is called by function ``getoptv`` whenever option ``-!`` or ``--version`` is detected on the command line. There is no need to call this function directly. The function is declared in `version.h <version.h.html>`_ and defined in `version.c <version.c.html>`_. Constants ``PACKAGE`` and ``VERSION`` define the information that is displayed. They are defined in file `version.h <version.h.html>`_ and must be maintained by developers.

.. _support-ethernet:

Ethernet Functions
==================

The Open Powerline Toolkit supports raw Ethernet I/O on several popular operating systems, including *Linux*,  *Mac OS X* and Microsoft *Windows*. Other operating systems will probably be added over time. These functions are found in the :file:`ether` folder.

Each operating system has a different raw Ethernet interface and so some abstraction was needed to support the toolkit for all environments. Our solution was the channel which is implemented like a ``FILE`` pointer but is used like a file descriptor. All toolkit programs, with a few exceptions, perform raw Ethernet I/O by opening a channel, reading and writing to it and then closing it.

.. _support-channel:

channel
-------

The channel structure contains enough information to perform raw Ethernet I/O in several common runtime environments; however, portions of the structure vary depending on the environment. These differences are appled by compile time constants that include required structure members and exclude others. The common structure members are identified and described below. The others elements are not discussed because they may change.

.. code-block:: c

	typedef struct __packed channel
	{
		signed fd;
		signed ifindex;
		char const * ifname;
		uint8_t peer [ETHER_ADDR_LEN];
		uint8_t host [ETHER_ADDR_LEN];
		uint16_t type;

		/* ... <operating system dependent data> ... */

		signed timeout;
		flag_t flags;
	} CHANNEL;

``fd``
    Socket file descriptor.

``ifindex``
    Ethernet device index. The index only applies when the toolkit is compiled for :program:`LibPcap` or :program:`WinPcap`. This value is the same as that returned in the ``ifr_ifindex`` member of the ``ifreq`` structure available on most operating systems.

``ifname``
    The interface name. On Linux, ethernet names are typically "eth0",  "eth1" and so on. On Mac OS X, interface names are "en0",  "en1" and so on. This string is the same as that returned by the ``ifr_ifname`` member of the ``ifreq`` structure available on most operating systems.

``peer``
    The Ethernet hardware address of some remote device. It is used to encode the ODA field of outgoing Ethernet frames and format some console messages. It is initialized to the Atheros Local Management Address, ``00:B0:52:00:00:01`` for HomePlug AV applications. Application programs can,  and often do, replace this value at runtime.

``host``
    The Ethernet hardware address of the host computer. It is used to encode the OSA field of outgoing Ethernet frames and format some console messages. This address is initialized to the hardware address assigned to the interface by the host operating system. The value should not change.

``type``
    The Ethernet type/length field. It is used to encode the MTYPE field of outgoing Ethernet frames. The values is initialized to ``0x88E1`` for HomePlug AV application and ``0x887B`` for HomePlug 1.0 application. The value should not change.

``timeout``
    A time interval. On *Linux* and *Mac OS X*, it is the maximum time that the application will wait for a device to respond when a response is expected. With *LibPcap* and *WinPcap* it the mininum time the application will wait. It is initialized to ``50`` milliseconds which is a reasonable compromise but most toolkit programs allow the user to change this value.

``flags``
    A bitmap where each bit enables a special behavior during channel open or close or packet read or write. Of general interest is the ``CHANNEL_VERBOSE`` bit which prints outgoing and incoming frames on stderr in hexadecimal dump format. The verbose feature is implemented in for all toolkit programs that perform raw Ethernet I/O and is helpful when debugging device behavior.

Since toolkit applications typically communicate with one powerline device at a time, this structure is statically initialized in a stand-alone module that is linked into each application. It is possible to dynamically initialize it, if needed. The structure is declared in `channel.h <channel.h.html>`_ and statically defined in `channel.c <channel.c.html>`_.

.. _support-closechannel:

closechannel
------------

.. c:function:: signed closechannel(struct channel *channel)

Close the Ethernet socket associated with a channel and free associated memory and data structures. Return ``0`` on success. Return ``-1`` on failure. This function is declared in `channel.h <channel.h.html>`_ and defined in `closechannel.c <closechannel.c.html>`_.

.. _support-openchannel:

openchannel
-----------

.. c:function:: signed openchannel(struct channel *channel, uint16_t protocol)

Open an Ethernet socket that supports the specified protocol and associate it with the interface referenced by the ``channel`` structure .name member. Initialize the interface as needed. The ``protocol`` effectively filters incoming frames for the application.

Interface initialization differs significantly from environment to environment. The socket descriptor is stored in the ``channel`` structure .fd member and the interface hardware address is stored in the ``channel`` structure .host member. Return ``0`` on success. Terminate the program with an error message on failure. This function is declared in `channel.h <channel.h.html>`_ and defined in `openchannel.c <openchannel.c.html>`_.

.. _support-readpacket:

readpacket
----------

.. c:function:: signed readpacket(struct channel *channel, void *packet, signed length)

Read one Ethernet frame from the specified channel. The frame is written into memory starting at address ``packet`` and is truncated to the specified ``length``,  if necessary. Return the actual number of bytes read on success. Return ``0`` on timeout. Return ``-1`` on network error. This function behaves like the standard library ``read`` function. The target memory region remains unchanged on timeout or error. This function is declared in `channel.h <channel.h.html>`_ and defined in `readpacket.c <readpacket.c.html>`_.

On systems using Berkeley Packet Filters, such as MacOS X, the ``ODA`` field is automatically replaced on transmission to prevent Ethernet address spoofing. This may not be true on other systems but the practice is becoming more common.

.. _support-sendpacket:

sendpacket
----------

.. c:function:: signed sendpacket(struct channel *channel, void *packet, signed length)

Write one Ethernet frame to the specified channel. The frame is read from memory starting at address ``packet`` and ending at the specified ``length``. Return the actual number of bytes sent on success. Return ``0`` on timeout. Return ``-1`` on network error. The frame should be properly formatted as an ethernet frame and must be at least 60 bytes long or it will not be sent. This function behaves like the standard library ``write`` function. The source memory region is not modified. This function is declared in `channel.h <channel.h.html>`_ and defined in `sendpacket.c <sendpacket.c.html>`_.

.. _support-network:

Network Functions
=================

.. _support-networkbridges:

NetworkBridges
--------------

.. c:function:: size_t NetworkBridges(struct channel *channel, void *memory, size_t extent)

Search a channel for local powerline devices (bridges) and store their Ethernet hardware in consecutive ``memory`` locations. The memory ``extent`` is specified in bytes. The number of devices found is returned as the function value,  provided enough space is available to store all addressed. Unlike function :ref:`NetworkDevices <support-networkdevices>`, the channel ``peer`` address is ignored.

We call local devices "bridges" because each serves as an Ethernet-to-Powerline bridge for the local host.

Enumerating Local Devices
~~~~~~~~~~~~~~~~~~~~~~~~~
.. code-block:: c

	#include <net/if.h>
	#include "../ether/channel.h"

	extern struct channel channel;
	uint8_t address [255][ETHER_ADDR_LEN];
	size_t bridge = 0;

	size_t bridges = NetworkBridges (&channel, address, sizeof (address));
	while (bridge < bridges)
	{
		binout (address [bridge], ETHER_ADDR_LEN, ':', stdout);
		putc ('\n', stdout);
	}

The code segment shown above illustrates how to use function ``NetworkBridges`` to identify all local devices. We do not need to set the :ref:`channel <support-channel>` ``peer`` because ``NetworkBridges`` always uses the Atheros ``localcast`` address. Array ``address`` is declared with two dimensions to simplify access to individual addresses on return.

.. _support-networkdevices:

NetworkDevices
--------------

.. c:function:: size_t NetworkDevices(struct channel *channel, void *memory, size_t extent)

Query a powerline device, specified by :ref:`channel <support-channel>`, for neighboring network devices and store their Ethernet addresses in consecutive ``memory`` locations. The memory ``extent`` is specified in bytes. The number of devices found is returned as the function value, provided sufficient space is available to store all addresses. Unlike function :ref:`NetworkBridges <support-networkbridges>`, the channel ``peer`` address is identifies the device to be queried.

Enumerating Network Devices
~~~~~~~~~~~~~~~~~~~~~~~~~~~
.. code-block:: c

	#include <net/if.h>
	#include "../ether/channel.h"

	extern struct channel channel;
	uint8_t address [255][ETHER_ADDR_LEN];
	size_t device = 0;

	size_t devices = NetworkDevices (&channel, address, sizeof (address));
	while (device < devices)
	{
		binout (address [device], ETHER_ADDR_LEN, ':', stdout);
		putc ('\n';, stdout);
	}

The code segment shown above illustrates how to use function ``NetworkDevices`` to identify all devices on a specific network. You must set the :ref:`channel <support-channel>` ``peer`` address before calling ``NetworkDevices``. Array ``address`` is declared with two dimensions to simplify access to individual addresses on return. If the :ref:`channel <support-channel>` ``peer`` address is not the Ethernet ``broadcast`` or Atheros ``localcast`` address then the first address returned will be the :ref:`channel <support-channel>` ``peer`` address.

.. _support-generic:

Generic Functions
=================

The functions in this chapter are generic and may have application beyond the Open Powerline Toolkit. In some cases, these functions appear in other Atheros or Open Source software packages. In a few cases, the Toolkit may include complementary or supplementary support functions but only use one or two of them. For example, functions ``memincr`` and ``memdecr`` are both included but ``memdecr`` is not used.

.. _support-binout:

binout
------

.. c:function:: void binout(void const *memory, size_t extent, char c, char e, FILE *fp)

Print a ``memory`` region as a series of binary octets separated by character ``c`` and terminated by character ``e``. Normally, character ``c`` will be ``BIN_EXTENDER``, defined in file `number.h <number.h.html>`_, but it could be any character value. Normally, character ``e`` will be a space or newline, but it could be any character value. A typical use might be to print a register in readable format. For example, specifying ``c`` as ``'-'``, ``e`` as ``';'`` and ``extent`` as ``4`` would produce output looking something like ``"10101010-1111111-00000000-11001100;"`` where each octet is expressed as a binary integer. The function is declared in `memory.h <memory.h.html>`_ and defined in `binout.c <binout.c.html>`_.

.. _support-checksum32:

checksum32
----------

.. c:function:: unint32_t checksum32(const uint32_t memory[], size_t extent, uint32_t checksum)

Return the 32 bit checksum of a ``memory`` region. The checksum is the one's complement of the XOR of all 32 bit words in the region. Argument ``extent`` is the region extent in 32 bit words. Argument ``checksum`` is the reference checksum. The function will return the computed checksum when reference ``checksum`` is ``0`` and will return ``0`` if reference ``checksum`` equals the computed checksum. A typical use is to validate PIB and NVM files or compute new checksums when these files are created or modified. The function is declared in `memory.h <memory.h.html>`_ and defined in `checksum32.c <checksum32.c.html>`_.

This function is similar to :ref:`checksum_32 <support-checksum-32>` but is used exclusively by API functions. It may be deprecated at some point in the future.

.. _support-checksum-32:

checksum_32
-----------

.. c:function:: unint32_t checksum_32(void const *memory, size_t extent, uint32_t checksum)

Return the 32 bit checksum of a ``memory`` region. The checksum is the one's complement of the XOR of all 32 bit words in the region. The region ``extent`` is specified in bytes but it will be rounded down to the nearest multiple of 4 bytes. Argument ``checksum`` is the reference checksum. The function will return the computed checksum when reference ``checksum`` is ``0`` and will return ``0`` if reference ``checksum`` equals the computed checksum. A typical use is to validate PIB and NVM files or compute new checksums when these files are created or modified. The function is declared in `memory.h <memory.h.html>`_ and defined in `checksum_32.c <checksum_32.c.html>`_.

This function is similar to function :ref:`checksum32 <support-checksum32>` however there is no need to cast ``memory`` to ``uint32_t`` and there is no need to round ``extent`` down to a multiple of 4 bytes before calling the function because both operations are performed internally. Also, there is no unecessary endian manipulation of the checksum. It is the prefered method of computing a checksum.

.. _support-chrout:

chrout
------

.. c:function:: void chrout(void const *memory, size_t extent, char c, char e, FILE *fp)

Print a ``memory`` region as a string of printable ASCII characters terminated by character ``e``. Character ``c`` is printed in place of non-printable characters. The string is terminated by character ``e``. Normally, character ``c`` is ``'.'`` but it could be any character value. Normally, character ``e`` is space or newline but it could be any charcter value. A typical use might be to print a memory region that may (or may not) contain an HFID or other printable text. The function is declared in `memory.h <memory.h.html>`_ and defined in `chrout.c <chrout.c.html>`_.

.. _support-decout:

decout
------

.. c:function:: void decout(void const *memory, size_t extent, char c, char e, FILE *fp)

Print a ``memory`` region as a series of decimal octets separated by character ``c`` and terminated by character ``e``. Normally, character ``c`` will be ``DEC_EXTENDER``, defined in file `number.h <number.h.html>`_, but it could be any character value. Normally, character ``e`` will be a space or newline but it could be any character value.  A typical use might be to print an IP address in readable format. For example, specifying ``c`` as ``'.'``, character ``e`` as ``'/'`` and ``extent`` as ``4`` would produce output looking something like ``"192.168.099.001/"`` where each octet is expressed as a decimal integer. The function is declared in `memory.h <memory.h.html>`_ and defined in `decout.c <decout.c.html>`_.

.. _support-endian:

endian
------

.. c:function:: void endian(void *memory, size_t extent)

Reverse the byte order of a ``memory`` region. It is a variable extent version of functions like ``__bswap_16``, ``__bswap_32`` and ``__bswap_64``. The function is declared in `memory.h <memory.h.html>`_ and defined in `endian.c <endian.c.html>`_.

.. _support-fdchecksum32:

fdchecksum32
------------

.. c:function:: uint32_t fdchecksum32(int fd, size_t extent, uint32_t checksum)

Return the 32 bit checksum of a file region starting from the current file position. The checksum is the one's complement of the XOR or of all 32 bit words in the region. Argument ``extent`` must be specified in 32 bit words, not bytes. Argument ``checksum`` is the reference checksum. The function will return the computed checksum when reference ``checksum`` is ``0`` and will return ``0`` if reference ``checksum`` equals the computed checksum. A typical use is to validate NVM files header by header or section by section. The function is declared in `memory.h <memory.h.html>`_ and defined in `fdchecksum32.c <fdchecksum32.c.html>`_.

.. _support-fdchecksum-32:

fdchecksum_32
-------------

.. c:function:: uint32_t fdchecksum_32(int fd, size_t extent, uint32_t checksum)

Return the 32 bit checksum of a file region starting from the current file position. The checksum is the one's complement of the XOR or of all 32 bit words in the region. Argument ``extent`` is specified in bytes but is rounded down to the nearest multiple of 4 bytes. Argument ``checksum`` is the reference checksum. The function will return the computed checksum when reference ``checksum`` is ``0`` and will return ``0`` if reference ``checksum`` equals the computed checksum. A typical use is to validate NVM files header by header or section by section. The function is declared in `memory.h <memory.h.html>`_ and defined in `fdchecksum_32.c <fdchecksum_32.c.html>`_.

This function is similar to function :ref:`fdchecksum32 <support-fdchecksum32>` but there is no need to round ``extent`` down to the nearest multiple of 4 bytes before calling the function because that is done internally. Also, there is no unecessary endian manuipulation of the checksum.

.. _support-filepar:

filepart
--------

.. c:function:: char const *filepart(char const *pathname)

Return the address of the filename portion of a ``pathname`` string. The filename portion is everything after the rightmost path separator. If a path separator is not present then the address of the ``pathname`` string is returned. This function is similar to the POSIX ``basename`` function but it returns an empty string whenever the rightmost character is a path separator. The path separator can be either slash (``'/'``) or backslash (``'\\'``). The function is declared in `files.h <files.h.html>`_ and defined in `filepart.c <filepart.c.html>`_.

.. _support-hexdecode:

hexdecode
---------

.. c:function:: signed hexdecode(void *memory, size_t extent, char const buffer[], size_t extent)

Decode a ``memory`` region as a string of ASCII hexadecimal digits. Convert memory until the buffer or memory exhausts and return the string extent. Allow three (3) string characters for each memory byte to be decoded. The number of bytes decoded will be the lesser of argument ``extent`` divided by ``3`` or argument ``extent``. The function is declared in `memory.h <memory.h.html>`_ and defined in `hexdecode.c <hexdecode.c.html>`_.

.. _support-hexdump:

hexdump
-------

.. c:function:: void hexdump(void const *memory, size_t offset, size_t extent, FILE *fp)

Print a full or partial ``memory`` region in hexadecimal format showing memory offsets, hexadecimal byte values and ASCII character values. Argument ``memory`` contains some memory region. Argument ``extent`` is the region extent. Argument ``offset`` is the starting display location. Locations ``memory`` [``offset``] up to ``memory`` [``extent``] are displayed, allowing a partial dump of the memory region. An ``offset`` of ``0`` will display the entire region. The function is declared in `memory.h <memory.h.html>`_ and defined in `hexdump.c <hexdump.c.html>`_.

This function is similar to but different from function :ref:`hexview <support-hexview>` .

.. _support-hexencode:

hexencode
---------

.. c:function:: signed hexencode(void *memory, size_t extent, char const *string)

Encode a ``memory`` region with the binary equivalent of an ASCII hexadecimal string. Return the number of bytes encoded or ``0`` on error. The value of ``errno`` is set to ``EINVAL`` if the number of bytes encoded is less than ``extent`` or the entire string cannot be converted due to illegal digits or excessive digits. Ignore optional ``HEX_EXTENDER`` characters separating octets in argument ``string``. Constant ``HEX_EXTENDER`` is defined in file `number.h <number.h.html>`_. The function is declared in `memory.h <memory.h.html>`_ and defined in `hexencode.c <hexencode.c.html>`_.

.. _support-hexin:

hexin
-----

.. c:function:: ssize_t hexin(void const *memory, size_t extent, FILE *fp)

This function is similar to :ref:`hexencode <support-hexencode>` but it reads from file, instead of a string and ignores non-hexadecimal text and comments within the input stream. Incoming text is binary encoded and written to the specified ``memory`` region. The actual number of bytes encoded is returned or ``-1`` on error. See the `efsu <efsu.7.html>`_ man page for a thorough explanation of function behavior. The function is declared in `memory.h <memory.h.html>`_ and defined in `hexin.c <hexin.c.html>`_.

.. _support-hexout:

hexout
------

.. c:function:: void hexout(void const *memory, size_t extent, char c, char e, FILE *fp)

Print a ``memory`` region as a series of hexdecimal octets separated by character ``c`` and termianted by character ``e``. Normally, character ``c`` will be ``HEX_EXTENDER``, defined in file `number.h <number.h.html>`_, but it could be any character value. Normally, character ``e`` will be a space or newline but it could be any character value. A typical use might be to print a MAC or Ethernet address in readable format. For example, specifying ``c`` as ``':'``, character ``e`` as ``','`` and ``extent`` as ``6`` would produce output looking something like ``"00:B0:52:DA:DA:01,"`` where each octet is expressed as a hexadecimal integer. The function is declared in `memory.h <memory.h.html>`_ and defined in `hexout.c <hexout.c.html>`_.

.. _support-hexstring:

hexstring
---------

.. c:function:: char *hexstring(char buffer[], size_t length, void const *memory, size_t extent)

Convert a ``memory`` region to a ``NUL`` terminated string and return the string address. This function is identical to function :ref:`hexdecode <support-hexdecode>` but it return the string address instead of the number of characters decoded. The function is declared in `memory.h <memory.h.html>`_ and defined in `hexstring.c <hexstring.c.html>`_.

.. _support-hexview:

hexview
-------

.. c:function:: void hexview(void const *memory, size_t offset, size_t extent, FILE *fp)

Print a partial ``memory`` region in hexadecimal format showing memory offsets, hexadecimal byte values and ASCII character values. Argument ``memory`` contains part of a larger memory region, much like a file window. Argument ``extent`` is the window length. Argument ``offset`` is the relative offset of the window within the region. Locations ``memory`` [``0``] up to but excluding  ``memory`` [``extent``] are displayed as a partial dump, providing a window into the region. The function is declared in `memory.h <memory.h.html>`_ and defined in `hexview.c <hexview.c.html>`_.

This function is similar to but different from function :ref:`hexdump <support-hexdump>`.

.. _support-memdecr:

memdecr
-------

.. c:function:: signed memdecr(void *memory, size_t extent)

Decrement a multi-byte ``memory`` region. Return ``0`` on success or ``-1`` if all bytes have decremented to ``0x00``. For example, ``{ 0xFF, 0xFF, 0xFF }`` decrements to ``{ 0xFF, 0xFF, 0xFE }`` and ``{ 0xFF, 0x00, 0x00 }`` decrements to ``{ 0xFE, 0xFF, 0xFF }``. A typical use is to iterate through a range if IP or MAC address values. The function is declared in `memory.h <memory.h.html>`_ and defined in `memdecr.c <memdecr.c.html>`_.

.. _support-memincr:

memincr
-------

.. c:function:: signed memincr(void *memory, size_t extent)

Increment a multi-byte ``memory`` region. Return ``0`` on success or ``-1`` once all bytes have been incremented to ``0xFF``. For example ``{ 0x00, 0x00, 0x00 }`` increments to ``{ 0x00, 0x00, 0x01 }`` and ``{ 0x00, 0xFF, 0xFF }`` increments to ``{ 0x01, 0x00, 0x00 }``. A typical use is to iterate through a range of IP or MAC address values. The function is declared in `memory.h <memory.h.html>`_ and defined in `memincr.c <memincr.c.html>`_.

.. _support-memout:

memout
------

.. c:function:: void memout(void const *memory, size_t extent, char const *format, unsigned group, signed c, FILE *fp)

Print a ``memory`` region as a series of octet groups wach separated by character ``c``. The ``group`` argument specifies the number of octets per group. The ``format`` argument determines how each octet is displayed. Normally, character ``c`` will be one of ``BIN_EXTENDER``, ``DEC_EXTENDER`` or ``HEX_EXTENDER`` as defined in file `number.h <number.h.html>`_, but it could be any character value. The function is declared in `memory.h <memory.h.html>`_ and defined in `memout.c <memout.c.html>`_.

.. _support-memswap:

memswap
-------

.. c:function:: void memswap(void *buffer1, void *buffer2, size_t length)

Exchange the contents of one buffer with that of another. No provision is made for buffer overlap. No value is returned. A typical use might be to exchange source and destination addresses in an ethernet packet. The function is declared in `memory.h <memory.h.html>`_ and defined in `memswap.c <memswap.c.html>`_.

.. _support-strdecr:

strdecr
-------

.. c:function:: signed strdecr(void *memory, size_t extent, byte min, byte max)

Decrement a multi-byte ``memory`` region using only ASCII character values in the range ``min`` through ``max``. Return ``0`` on success or ``-1`` once all characters have been decremented to the value of argument ``min``. For example,  if argument ``min`` is ``'A'`` and argument ``max`` is ``'Z'`` then ``{ 'A', 'B', 'C' }`` decrements to ``{ 'A', 'B', 'B' }`` and ``{ 'B', 'Z', 'Z' }`` decrements to ``{ 'A', 'A', 'A' }``. A typical use is to generate a sequence of distinct character strings to seed encryption key functions. The function is declared in `memory.h <memory.h.html>`_ and defined in `strdecr.c <strdecr.c.html>`_.

.. _support-strfbits:

strfbits
--------

.. c:function:: size_t strfbits(char const buffer[], size_t length, char const *operands[], char const *operator, unsigned flagword)

Encode a ``buffer`` with an enumerated list of the ``operands`` associated with the corresponding bits in ``flagword``. separate enumerated ``operands`` with an ``operator`` string. For example, given ``char const *operands [] = { "loop", "wait",  "busy" }`` and ``unsigned flagword = 0x05`` then ``strfbits (buffer,  length, operands, "|", flagword)`` would encode buffer with ``"loop|busy"``. Observe that each bit set in ``flagword`` appears in ``buffer`` as the corresponding string from ``operands``. A typical application for this function is the enumeration of flagword states. The function is declared in `format.h <format.h.html>`_ and defined in `strfbits.c <strfbits.c.html>`_.

.. _support-strincr:

strincr
-------

.. c:function:: signed strincr(void *memory, size_t extent, byte min, byte max)

Increment a multi-byte ``memory`` region using only ASCII character values in the range ``min`` through ``max``. Return ``0`` on success or ``-1`` once all characters have been incremented to the value of argument ``max``.  For example,  if argument ``min`` is ``'A'`` and argument ``max`` is ``'Z'`` then ``{ 'A', 'B', 'C' }`` increments to ``{ 'A', 'B', 'D' }`` and ``{ 'A', 'Z', 'Z' }`` increments to ``{ 'B', 'A', 'A' }``. A typical use is to generate a sequence of distinct character strings to seed encryption key functions. The function is declared in `memory.h <memory.h.html>`_ and defined in `strincr.c <strincr.c.html>`_.

.. _support-todigit:

todigit
-------

.. c:function:: unsigned todigit(unsigned c)

Return the integer value of character ``c`` interpreted as digit in the base 36 number system. It is called by many encode functions to support number base conversion. If the value of ``c`` is ``'0'`` through ``'9'`` then integer ``0`` through ``9`` is returned.  If the value of ``c`` is ``'A'`` through ``'Z'`` or ``'a'`` through ``'z'`` then integer ``10`` through ``35`` is returned. The function is declared in `number.h <number.h.html>`_ and defined in `todigit.c <todigit.c.html>`_.

.. _support-typename:

typename
--------

.. c:function:: char const *typename(const struct _type_ list[], size_t size, type_t type, char const *name)

Return the name associated with a message ``type`` by searching a list arranged in ascending order by message type. Return argument ``name`` as the function value if the message ``type`` is not present in the ``list``. Data types ``struct _type_`` and ``type_t`` are defined in file `types.h <types.h.html>`_. A typical use might be to return the name of message based on the message type. The function is declared in `symbol.h <symbol.h.html>`_ and defined in `typename.c <typename.c.html>`_.

.. _support-message:

Message Debug Functions
=======================

Message debug functions are not needed for proper toolkit operation but they can be useful during development, debugging and testing phases of product development. They are specific to HomePlug AV messages only.

.. _support-mmecode:

MMECode
-------

.. c:function:: char const *MMECode(uint16_t MMTYPE, uint8_t MSTATUS)

Return text that explains the status code for a given Atheros vendor-specific management message. Argument ``MMTYPE`` is an unsigned integer in the range ``0xA000`` through ``0xAFFF`` and must exactly match the ``MMTYPE`` of the message returning the status code. For example, the ``CNF`` variant of a message is currently the only variant that has an ``MSTATUS`` field and so other variants return a default string. The function is declared in `mme.h <mme.h.html>`_ and defined in `MMECode.c <MMECode.c.html>`_.

Inclusion of this function can increase application size by 3kb because the text associated with all Atheros vendor-specific management messages and errors are included whether needed or not. Conditional compilation statements are provided internally to suppress the full table with a terse substitute.

Although the :program:`Bootloader` and firmware share some common Vendor Specific Management Messages, the two programs may return different codes for a given message and error condition. Where there are conflicts, Bootloader codes take precedence over firmware codes because it is unlikely that users will .issue the same messages to a device running firmware.

.. _support-mmemode:

MMEMode
-------

.. c:function:: char const *MMEMode(uint16_t MMTYPE)

Return the mode of a given management message type as a string. The mode is determined by the two least significant bits of argument  ``MMTYPE``. The mode string will be one of "REQ", "CNF",  "IND" or "RSP". The function is declared in `ihp.h <ihp.h.html>`_ and defined in `MMEMode.c <MMEMode.c.html>`_.

Most management messages have a request and a confirmation variant while a few have an indication and a response variant. Requests originate from the host and are acknolwedged by a confirmation from the device. Indications originate from a device and are acknowledged by a response from the host.

.. _support-mmename:

MMEName
-------

.. c:function:: char const *MMEName(uint16_t MMTYPE)

Return the name associated with a given management message type. The name is the mnemonic used to distinguish one message type from another in the *HomePlug AV Specification* or Atheros *Firwmare Technical Reference Manual*. Argument ``MMTYPE`` is an unsigned integer in the range ``0x0000`` through ``0xAFFF``. The request, confirmation, indication and response variants of a management message will all return the same message name. The function is declared in `mme.h <mme.h.html>`_ and defined in `MMEName.c <MMEName.c.html>`_.

.. _support-mmepeek:

MMEPeek
-------

.. c:function:: void MMEPeek(void const *memory, size_t extent, FILE *fp)

Prints an Atheros vendor-specific management message to stream ``fp`` in human readable format. Argument ``buffer`` is the message address and argument ``extent`` is the message extent in bytes. Output consists of a formatted header showing decoded header. The function is declared in `mme.h <mme.h.html>`_ and defined in `MMEPeek.c <MMEPeek.c.html>`_.

.. _support-key:

Key Generation Functions
========================

Atheros applications use the SHA256 algorithm to compute unique HomePlug AV keys, including the Device Access Key (DAK), Network Membership Key (NMK) and Network Identifier (NID). The SHA256 algorithm is seeded, salted and rehashed differently for each type of HomePlug AV key. A different function is used to compute each type of HomePlug AV key but all of them call the SHA256 functions. The SHA256 algorithm returns a ``256`` bit (``32`` byte) digest but only the upper ``128`` bits (``16`` bytes) are used for HomePlug AV keys.

SHA256 Digest Computation
-------------------------

The following code example illustrates how to generate, rehash and print an SHA256 digest, or key, from a user-defined seed string and salt string. This logic is typical of that used to generate HomePlug AV compliant keys. See the HomePlug AV Specification for the specific rules used to generate each type of key.

.. code-block:: c

	#include <stdio.h>

	#include "../crypt/SHA256.h"

	#include "../crypt/SHA256Reset.c"
	#include "../crypt/SHA256Block.c"
	#include "../crypt/SHA256Write.c"
	#include "../crypt/SHA256Fetch.c"

	struct sha256 sha256;
	char * seed = "ForMeToKnowAndYouToFindOut";
	char * salt = "X$z@p";
	byte digest [SHA256_DIGEST_SIZE];
	int i;

	SHA256Reset (&sha256);
	SHA256Write (&sha256, (byte *)(seed), strlen (seed));
	SHA256Write (&sha256, (byte *)(salt), strlen (salt));
	SHA256Fetch (&sha256, digest);

	for (i = 0; i < 999; i++)
	{
		SHA256Reset (&sha256);
		SHA256Write (&sha256, digest, sizeof (digest));
		SHA256Fetch (&sha256, digest);
	}
	for (i = 0;  i < 16; i++)
	{
		printf ("%02x", digest [i]);
	}
	printf ("\n");

Above, we declare struct ``sha256`` and initialize it using function :ref:`SHA256Reset <support-SHA256Reset>`. We then write a user-defined seed string (or password) followed by an optional salt string to the digest using function :ref:`SHA256Write <support-SHA256Write>`. The resulting digest (or key) is obtained by calling function :ref:`SHA256Fetch <support-SHA256Fetch>`. Constant ``SHA256_DIGEST_SIZE`` is defined in `SHA256.h <SHA256.h.html>`_. Although the digest is probably secure enough at this point, we rehash it ``999`` times for good measure. We then print the first ``16`` bytes of the result because HomePlug AV keys are always ``16`` bytes long.

.. _support-MACPasswords:

MACPasswords
------------

.. c:function:: void MACPasswords(uint32_t vendor, uint32_t device, uint32_t number, unsigned count, unsigned bunch, unsigned space, flag_t flags)

Print a range of device addresses and user passwords on stdout. The calling application must split the starting device address into a three-byte OUI and three-byte device number and pass them as the unsigned 32-bit integers ``vendor`` and ``device``, respectively. Argument ``device`` is the first device number in the range and ``number`` is the final device number. Argument ``count`` is the number of uppercase letters comprising a password. Passwords contain this many letters but the letters are grouped for easy reading. Letter groups are separated by hyphens. Argument ``bunch`` defines how many letters form each group. Argument ``space`` is the character used to separate groups of letters. Argument ``flags`` enables or disables function features such as insertion of a "used" flag for use by the Atheros Production Test System or the omission of the device address on output. The output of this function is similar to that produced by the :program:`DB Builder` utility distributed with the PTS. This function is declared in `keys.h <keys.h.html>`_ and defined in `MACPasswords.c <MACPasswords.c.html>`_.

.. _support-HPAVKeyDAK:

HPAVKeyDAK
----------

.. c:function:: void HPAVKeyDAK(byte DAK[], char const *string)

Encode buffer ``DAK`` with the Device Access Key (DAK) derived from the ``NUL`` terminated ``string``. The ``string`` is salted, hashed and re-hashed using the SHA256 algorithm. The ``DAK`` is always ``HPAVKEY_DAK_LEN`` bytes, defined in `HPAVKey.h <HPAVKey.h.html>`_,  so no length argument is needed. See the HomePlug AV Specification for more information. This function is declared in `HPAVKey.h <HPAVKey.h.html>`_ and defined in `HPAVKeyDAK.c <HPAVKeyDAK.c.html>`_.

.. _support-HPAVKeyNMK:

HPAVKeyNMK
----------

.. c:function:: void HPAVKeyNMK(byte digest[], char const *string)

Encode buffer ``NMK`` with the Network Membership Key (NMK) derived from the ``NUL`` terminated ``string``. The string is salted, hashed and re-hashed using the SHA256 algorithm. The ``DAK`` is always ``HPAVKEY_NMK_LEN`` bytes,  as defined in `HPAVKey.h <HPAVKey.h.html>`_,  so no length argument is needed. See the HomePlug AV Specification for more information. This function is declared in `HPAVKey.h <HPAVKey.h.html>`_ and defined in `HPAVKeyNMK.c <HPAVKeyNMK.c.html>`_.

.. _support-HPAVKeyNID:

HPAVKeyNID
----------

.. c:function:: void HPAVKeyNID(byte NID[], byte const NMK[], signed level)

Encode buffer ``NID`` with the Network Identification Key (NID) derived from the 16-byte Network Membership Key (NMK). The ``NMK`` is hashed and re-hashed using the SHA256 algorithm then encoded with the security ``level``. The ``NID`` is always ``HPAVKEY_NID_LEN`` bytes,  as defined in `HPAVKey.h <HPAVKey.h.html>`_,  so no length argument is needed. See the HomePlug AV Specification for more information. This function is declared in `HPAVKey.h <HPAVKey.h.html>`_ and defined in `HPAVKeyNID.c <HPAVKeyNID.c.html>`_.

.. _support-HPAVKeyOut:

HPAVKeyOut
----------

.. c:function:: void HPAVKeyOut(byte const digest[], size_t length, char const *phrase, flag_t flags)

Decode and print up to ``length`` bytes from buffer ``digest`` on stdout. Print ``NUL`` terminated ``phrase`` on the same line following the digest when bit ``HPAVKEY_VERBOSE``, defined in `HPAVKey.h <HPAVKey.h.html>`_, is set in ``flags``. This procedure prints keys and phrases in a standard column-oriented format that other applications can easily read and use. This function is declared in `HPAVKey.h <HPAVKey.h.html>`_ and defined in `HPAVKeyOut.c <HPAVKeyOut.c.html>`_.

.. _support-SHA256Block:

SHA256Block
-----------

.. c:function:: void SHA256Block(struct sha256 *sha256, void const *memory)

Merge 64 bytes of memory into the current SHA256 digest. This function performs the core computations required by the SHA256 algoithm. It is called by function :ref:`SHA256Fetch <support-SHA256Fetch>` and so there is no reason to call this function directly. The ``sha256`` structure is defined in `SHA256.h <SHA256.h.html>`_. The function is declared in `SHA256.h <SHA256.h.html>`_ and defined in `SHA256Block.c <SHA256Block.c.html>`_.

.. _support-SHA256Fetch:

SHA256Fetch
-----------

.. c:function:: void SHA256Fetch(struct sha256 *sha256, byte digest[])

Conclude SHA256 computation and transfer the result to the named 32-byte ``digest`` then flush the ``sha256`` structure. The ``digest`` is always 32 bytes long regardless of the amount of information that has been written using function :ref:`SHA256Write <support-SHA256Write>`. The ``sha256`` structure is defined in `SHA256.h <SHA256.h.html>`_. This function is declared in `SHA256.h <SHA256.h.html>`_ and defined in `SHA256Fetch.c <SHA256Fetch.c.html>`_.

Once this function is called, the ``sha256`` structure is purged for security reasons. You must call :ref:`SHA256Reset <support-SHA256Reset>` again before starting another hash or subsequent calls to this function or :ref:`SHA256Write <support-SHA256Write>` will have unpredictable results. .

.. _support-SHA256Ident:

SHA256Ident
-----------

.. c:function:: void SHA256Ident(int fd, byte digest[])

Compute the SHA256 digest from the content of a file. The digest serves as the file "fingerprint" and can be used to identify identical content despite filename changes. File descriptor ``fd`` is the subject file which must be positioned to the start befor calling this function. Buffer ``digest`` will be written with the computed 256-digest and must be ``SHA256_DIGEST_SIZE`` bytes long, as defined in `SHA256.h <SHA256.h.html>`_. This function is declared in `SHA256.h <SHA256.h.html>`_ and defined in `SHA256Ident.c <SHA256Ident.c.html>`_.

.. _support-SHA256Match:

SHA256Match
-----------

.. c:function:: void SHA256Match(int fd, byte const digest[])

Search a registry file for a known SHA256 digest. Return ``true`` on success or ``false`` on failure. File descriptor ``fd`` is the registry file which must be positioned to the start before calling this function. Buffer ``digest`` contains the known 256-bit digest and must be ``SHA256_DIGEST_SIZE`` bytes long, as defined in `SHA256.h <SHA256.h.html>`_. A registry file consists of 64-digit hexadecimal strings that represent SHA256 digest values. Optional text may be appear between the digest and newline to document digest significance. This function ignores such text. This function is declared in `SHA256.h <SHA256.h.html>`_ and defined in `SHA256Match.c <SHA256Match.c.html>`_.

.. _support-SHA256Print:

SHA256Print
-----------

.. c:function:: void SHA256Print(byte const digest[], char const *string)

Print an SHA256 ``digest`` and optional ``string`` on ``stdout``. Buffer ``digest`` must be ``SHA256_DIGEST_SIZE`` bytes long, as defined in `SHA256.h <SHA256.h.html>`_, which results in 64 hexadecimal digits on output. Variable length string is ``NUL`` terminated but may be ``NULL``. If ``string`` is not ``NULL`` and is not empty then a space is inserted between the digest and the string on output. This function may be used to print SHA256 digests and optional strings in standard format,  such as a registry file. This function is declared in `SHA256.h <SHA256.h.html>`_ and defined in `SHA256Print.c <SHA256Print.c.html>`_.

.. _support-SHA256Reset:

SHA256Reset
-----------

.. c:function:: void SHA256Reset(struct sha256 *sha256)

Initialize a ``sha256`` structure before computing a new SHA256 digest. This function should be called once before calling :ref:`SHA256Write <support-SHA256Write>` for the first time for a given digest. The ``sha256`` structure is defined in `SHA256.h <SHA256.h.html>`_. This function is declared in `SHA256.h <SHA256.h.html>`_ and defined in `SHA256Reset.c <SHA256Reset.c.html>`_.

.. _support-SHA256Write:

SHA256Write
-----------

.. c:function:: void SHA256Write(struct sha256 *sha256, void const *memory, size_t extent)

Writes a region of memory to the current SHA256 digest contained in an ``sha256`` structure. An application may call this function any number of times to concatinate multiple memory regions before fetching the digest with function ``SHA256Fetch``. The ``sha256`` structure is defined in `SHA256.h <SHA256.h.html>`_.  This function is declared in `SHA256.h <SHA256.h.html>`_ and defined in `SHA256Write.c <SHA256Write.c.html>`_.

.. _support-xml:

XML Editing Functions
=====================

Atheros provides support for remote PIB management using XML instruction files. The concept is based on an XML schema that defines the elements and attributes needed to edit a PIB file. The schema is located in file piboffset.xsd. In practice, this schema could be used to edit any buffer containing data but we are only interested in editing PIB files here.

Users may create XML files that conform to the schema and specify the PIB objects they want to change and how they want each to change. In brief, the files specify an ``offset``, a data ``length`` and a data value. A data value has type ``dataString``, ``dataHex`` or ``dataByte`` and may be a single data item or an array of data items. Each XML file should include a reference to the Atheros schema and be submitted to a validating XML parser,  such as :program:`Internet Explorer` or :program:`Firefox`,  to ensure that file structure and syntax are correct before use. An example XML file can be seen in piboffset.xml.

Editing involves the following steps. An example of these steps can be seen in the `pxe program source code <pxe.c.html>`_ and in function `RdModWrPib2.c <RdModWrPib2.c.html>`_ which is a component of the `int6k2 <int6k2.7.html>`_ program.

#. Obtain a copy of the PIB to be edited. This can be done by opening a PIB file and reading the entire file into memory or by requesting the PIB directly from an active Atheros powerline device. The PIB length can be determined from the PIB header.

#. Open the XML file, generate an XML parse tree and close the file. This can be done by passing the filename to function :ref:`xmlopen <nodes-xmlopen>`. It will perform all these steps and return the parse tree root node.

#. Modify the PIB based to instructions stored in the parse tree. This is done by passing the parse tree, PIB buffer and PIB length to function :ref:`xmledit <nodes-xmledit>`. It will traverse the parse tree and apply changes to the buffer as defined in the parse tree.

#. Discard the parse tree. This can be done by passing the parse tree to function :ref:`xmlfree <nodes-xmlfree>`.

#. Compute a new PIB checksum. This can be done by clearing the PIB checksum then passing the PIB buffer and PIB length to function :ref:`checksum32 <support-checksum32>` with a checksum value of ``0``.

#. Write the PIB to file or transmit it over the network to an Atheros powerline device.

.. _nodes-xmledit:

xmledit
-------

.. c:function:: signed xmledit(struct node *node, void *memory, unsigned extent)

Write data into ``memory`` based on the data offset and length defined in XML parse tree, ``node``. This function will not apply modifications having malformed structure or having an invalid or illegal offset, length or data value. Use function :ref:`xmlschema <nodes-xmlschema>` to print the current schema. The function is declared in `node.h <node.h.html>`_ and defined in `xmledit.c <xmledit.c.html>`_. The ``node`` structure is defined in `node.h <node.h.html>`_.

.. _nodes-xmlfree:

xmlfree
-------

.. c:function:: void xmlfree(struct node *node)

Release memory associated with an XML parse tree. The function is declared in `node.h <node.h.html>`_ and defined in `xmlfree.c <xmlfree.c.html>`_. The ``node`` structure is defined in `node.h <node.h.html>`_.

The ``text`` member if the parse tree root node points to memory allocated by :ref:`xmlscan <nodes-xmlscan>` to store the original XML string. The application must either deallocate that memory before calling this function or store the text address and deallocate that memory after calling this function;  otherwise, memory leaks will result.

.. _nodes-xmlnode:

xmlnode
-------

.. c:function:: struct node *xmlnode(struct node *node)

Create an empty node blow the specified node and return a pointer to it. A typical application can be seen in function `xmlscan.c <xmlscan.c.html>`_. The function is declared in `node.h <node.h.html>`_ and defined in `xmlnode.c <xmlnode.c.html>`_. The ``node`` structure is defined in `node.h <node.h.html>`_.

.. _nodes-xmlopen:

xmlopen
-------

.. c:function:: struct node *xmlopen(char const *filename)

Open the named XML file, parse it and return the XML parse tree. The parse tree contains, among other things, all XML elements,  attributes and values specified in the file. The function is declared in `node.h <node.h.html>`_ and defined in `xmlopen.c <xmlopen.c.html>`_. The ``node`` structure is defined in `node.h <node.h.html>`_.

The returned parse tree is ready to traverse using struct ``node`` members ``above``, ``below``, ``prior`` and ``after``. Each node has a ``line``, ``type`` and ``test`` member. This function calls function :ref:`xmlscan <nodes-xmlscan>` to parse the file and generate the tree. The parse tree can be inspected by passing the root node to function :ref:`xmltree <nodes-xmltree>`.

.. _nodes-xmlscan:

xmlscan
-------

.. c:function:: signed xmlscan(struct node *node)

Scan the XML string attached to the specified ``node`` and create a parse tree below it. Return ``0`` if parsing is successrul or ``-1`` if not. The function is declared in `node.h <node.h.html>`_ and defined in `xmlscan.c <xmlscan.c.html>`_. The ``node`` structure is defined in `node.h <node.h.html>`_.

The application must point the ``text`` member to a  copy of the XML string before calling this function because parsing is destructive and requires write access to the string. The application can search and manipulate the tree on return. The application should deallocate string memory before calling :ref:`xmlfree <nodes-xmlfree>`.

.. _nodes-example-1:

XML Source File
~~~~~~~~~~~~~~~

The following is sample input to function ``xmlscan``. The element and attribute names shown are for illustration and were selected to that they can be easily identified in the output. The parse tree from this file can be seen in the example for function :ref:`xmltree <nodes-example-2>`.

.. code-block:: xml

	<?xml version="1.0" encoding="UTF-8" ?>
	<xmlfile
		xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
		xsi:noNamespaceSchemaLocation="myschema.xsd">
		<element name="Element1">
			<attribute name="Attribute1">
				1234567890
				</attribute>
			<attribute name="Attribute2">
				The quick brown fox jumped over the lazy dog.
				</attribute>
			</element>
		<element name="Element2">
			<attribute name="Attribute1">
				</attribute>
			<attribute name="Attribute2">
				The quick brown
				fox jumped over
				the lazy dog.
				</attribute>
			</element>
	<!-- comments can go anywhere -->
		<element name="Element2">
			<attribute name="Attribute1"/>
			<attribute name="Attribute2">
				value2
				</attribute>
			</element>
		</xmlfile>

.. _nodes-xmlschema:

xmlschema
---------

.. c:function:: void xmlschema(void)

Print an XML schema that supports function :ref:`xmledit <nodes-xmledit>` on stdout. This function has no argument. The schema produced by this function is guaranteed to support the parse logic implemented by function :ref:`xmledit <nodes-xmledit>`. Scheme element names are defined as constants in `node.h <node.h.html>`_ so that they can be referenced by applications that create their own XML edit instructions. The function is declared in `node.h <node.h.html>`_ and defined in `xmlschema.c <xmlschema.c.html>`_.

The ``text`` member in the parse tree root node points to memory allocated by :ref:`xmlscan <nodes-xmlscan>` to store the original XML string. The application must either deallocate that memory before calling this function or store the text address and deallocate that memory after calling this function;  otherwise, memory leaks will result.

XML Edit Schema
~~~~~~~~~~~~~~~
.. code-block:: xml

	<?xml version='1.0' encoding='UTF-8'?>
	<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema' elementFormDefault='qualified'>
		<xs:element name='pib'>
			<xs:complexType>
				<xs:sequence>
					<xs:element maxOccurs='unbounded' ref='object'/>
					</xs:sequence>
				</xs:complexType>
			</xs:element>
		<xs:element name='object'>
			<xs:complexType>
				<xs:attribute name='name' use='required' type='xs:NCName'/>
				<xs:sequence>
					<xs:sequence>
						<xs:element ref='offset'/>
						<xs:element ref='length'/>
						</xs:sequence>
					<xs:choice>
						<xs:element ref='array'/>
						<xs:element ref='dataString'/>
						<xs:element ref='dataByte'/>
						</xs:choice>
					</xs:sequence>
				</xs:complexType>
			</xs:element>
		<xs:element name='length' type='xs:positiveInteger'/>
		<xs:element name='offset' type='xs:hexBinary'/>
		<xs:element name='array'>
			<xs:complexType>
				<xs:choice maxOccurs='unbounded'>
					<xs:element ref='dataByte'/>
					<xs:element ref='dataHex'/>
					</xs:choice>
				</xs:complexType>
			</xs:element>
		<xs:element name='dataString' type='xs:string'/>
		<xs:element name='dataHex' type='xs:hexBinary'/>
		<xs:element name='dataByte' type='xs:unsignedByte'/>
		</xs:schema>

The schema shown above was produced by function :ref:`xmlschema <nodes-xmlschema>`. It is should be used to validate XML edit instructions before submitting them to program `pxe <pxe.7.html>`_.

.. _nodes-xmltree:

xmltree
-------

.. c:function:: void xmltree(struct node *node)

Print the XML parse tree on stdout. The function is declared in `node.h <node.h.html>`_ and defined in `xmltree.c <xmltree.c.html>`_.

.. _nodes-example-2:

XML Parse Tree
~~~~~~~~~~~~~~

The following is sample output from function ``xmltree``. The XML source is shown in the example for function :ref:`xmlscan <nodes-example-1>`. The numbers appearing down the left margin indicate the input line number from the XML source file parsed by function :ref:`xmlscan <nodes-xmlscan>`. The character codes indicate the node type and are defined in `node.h <node.h.html>`_. The ``node`` structure is defined in `node.h <node.h.html>`_.

::

    001 [?] [xml]
    001 [ ] 	[version]
    001 [=] 		[1.0]
    001 [ ] 	[encoding]
    001 [=] 		[UTF-8]
    002 [<] [xmlfile]
    002 [ ] 	[xmlns:xsi]
    002 [=] 		[http://www.w3.org/2001/XMLSchema-instance]
    002 [ ] 	[xsi:noNamespaceSchemaLocation]
    002 [=] 		[myschema.xsd]
    003 [<] 	[element]
    003 [ ] 		[name]
    003 [=] 			[Element1]
    004 [<] 		[attribute]
    004 [ ] 			[name]
    004 [=] 				[Attribute1]
    005 [>] 			[1234567890]
    006 [/] 			[attribute]
    007 [<] 		[attribute]
    007 [ ] 			[name]
    007 [=] 				[Attribute2]
    008 [>] 			[The quick brown fox jumped over the lazy dog.]
    009 [/] 			[attribute]
    010 [/] 		[element]
    011 [<] 	[element]
    011 [ ] 		[name]
    011 [=] 			[Element2]
    012 [<] 		[attribute]
    012 [ ] 			[name]
    012 [=] 				[Attribute1]
    013 [/] 			[attribute]
    014 [<] 		[attribute]
    014 [ ] 			[name]
    014 [=] 				[Attribute2]
    015 [>] 			[The quick brown fox jumped over the lazy dog.azy]
    018 [/] 			[attribute]
    019 [/] 		[element]
    020 [!] 	[-- comments can go anywhere --]
    021 [<] 	[element]
    021 [ ] 		[name]
    021 [=] 			[Element2]
    022 [<] 		[attribute]
    022 [ ] 			[name]
    022 [=] 				[Attribute1]
    023 [<] 		[attribute]
    023 [ ] 			[name]
    023 [=] 				[Attribute2]
    024 [>] 			[value2]
    025 [/] 			[attribute]
    026 [/] 		[element]
    027 [/] 	[xmlfile]

.. _support-download:

Download Functions
==================

Download function write applets, parameters and runtime firmware to device SDRAM or NVRAM.

.. _WriteExecuteApplet:

WriteExecuteApplet
------------------

.. c:function:: signed WriteExecuteApplet(struct plc *plc, void const *header)

Write an NVM image directly into SDRAM using ``VS_WRITE_AND_EXECUTE`` messages. This function calls either :ref:`WriteExecuteApplet1 <WriteExecuteApplet1>` or :ref:`WriteExecuteApplet2 <WriteExecuteApplet2>` based on the NVM header version. The function is declared in `plc.h <plc.h.html>`_ and defined in `WriteExecuteApplet.c <WriteExecuteApplet.c.html>`_. The ``plc`` structure is defined in `plc.h <plc.h.html>`_.

.. _WriteExecuteApplet1:

WriteExecuteApplet1
-------------------

.. c:function:: signed WriteExecuteApplet1(struct plc *plc, struct nvm_header1 *header)

Write an NVM image directly into SDRAM using ``VS_WRITE_AND_EXECUTE`` messages. This function is similar to :ref:`WriteExecuteApplet2 <WriteExecuteApplet2>` but expects a structure ``nvm_header1`` pointer instead of a structure ``nvm_header2`` pointer. The function is declared in `plc.h <plc.h.html>`_ and defined in `WriteExecuteApplet.c <WriteExecuteApplet.c.html>`_. Structure ``plc`` is defined in file `plc.h <plc.h.html>`_. Structure ``nvm_header1`` is defined in file `nvm.h <nvm.h.html>`_.

.. _WriteExecuteApplet2:

WriteExecuteApplet2
-------------------

.. c:function:: signed WriteExecuteApplet2(struct plc *plc, struct nvm_header2 *header)

Write an NVM image directly into SDRAM using ``VS_WRITE_AND_EXECUTE`` messages. This function is similar to :ref:`WriteExecuteApplet1 <WriteExecuteApplet1>` but expects a structure ``nvm_header2`` pointer instead of a structure ``nvm_header1`` pointer. The function is declared in `plc.h <plc.h.html>`_ and defined in `WriteExecuteApplet.c <WriteExecuteApplet.c.html>`_. Structure ``plc`` is defined in file `plc.h <plc.h.html>`_. Structure ``nvm_header1`` is defined in file `nvm.h <nvm.h.html>`_.

.. _support-procrastination:

Procrastination Functions
=========================

Procrastinations functions wait for specific events to occur or fail to occur.

.. _WaitForAssoc:

WaitForAssoc
------------

.. c:function:: signed WaitForAssoc(struct plc *plc)

Poll a powerline device using ``VS_NW_INFO`` messages until the device forms or joins a network. The host ethernet interface, destination hardware address and source hardware address are specified in structure ``plc``. It calls either :ref:`WaitForAssoc1 <WaitForAssoc1>` or :ref:`WaitForAssoc2 <WaitForAssoc2>` to procrastinate based on the ``hardwareID`` member of structure ``plc``. The function is declared in `plc.h <plc.h.html>`_ and defined in file `WaitForAssoc.c <WaitForAssoc.c.html>`_. Struct ``plc`` is declared in file `plc.h <plc.h.html>`_ and defined in file `plc.c <plc.c.html>`_.

.. _WaitForAssoc1:

WaitForAssoc1
-------------

.. c:function:: signed WaitForAssoc1(struct plc *plc)

Poll a 200mbs powerline device using ``VS_NW_INFO`` version ``0`` messages until the device joins or forms a network. The host ethernet interface, destination hardware address and source hardware address are specified in structure ``plc``. The function is declared in file `plc.h <plc.h.html>`_ and defined in file `WaitForAssoc1.c <WaitForAssoc1.c.html>`_. Struct ``plc`` is declared in file `plc.h <plc.h.html>`_ and defined in file `plc.c <plc.c.html>`_.

.. _WaitForAssoc2:

WaitForAssoc2
-------------

.. c:function:: signed WaitForAssoc2(struct plc *plc)

Poll a 1000mbs powerline device using ``VS_NW_INFO`` version ``1`` messages until the device joins or forms a network. The host ethernet interface, destination hardware address and source hardware address are specified in structure ``plc``. Structure ``plc`` is declared in file `plc.h <plc.h.html>`_ and defined in file `plc.c <plc.c.html>`_. The function is declared in file `plc.h <plc.h.html>`_ and defined in file `WaitForAssoc2.c <WaitForAssoc2.c.html>`_.

.. _WaitForReset:

WaitForReset
------------

.. c:function:: signed WaitForReset(struct plc *plc)

Poll a powerline device using ``VS_SW_VER`` messages until the device stops responding. The host ethernet interface, destination hardware address and source hardware address are specified in structure ``plc``. Structure ``plc`` is declared in file `plc.h <plc.h.html>`_ and define in file `plc.c <plc.c.html>`_. The function is declared in `plc.h <plc.h.html>`_ and defined in file `WaitForReset.c <WaitForReset.c.html>`_.

.. _WaitForStart:

WaitForStart
------------

.. c:function:: signed WaitForStart(struct plc *plc, char firmware, size_t length)

Poll a powerline device using ``VS_SW_VER`` messages until the device responds. The host ethernet interface, destination hardware address and source hardware address are specified in structure ``plc``. Update the ``hardwareID`` member of the ``plc`` structure. Return the firmware revision string contained in the confirmation message. Structure ``plc`` is declared in file `plc.h <plc.h.html>`_ and defined in file `plc.c <plc.c.html>`_. The function is declared in file `plc.h <plc.h.html>`_ and defined in file `WaitForStart.c <WaitForStart.c.html>`_.

.. _support-traffic:

Traffic Functions
=================

Traffic functions generate powerline traffic in various ways. Some send traffic direct from the host to each device while others use the ``VS_FR_LBK`` message to echo Ethernet frames back to the sender.

.. _support-CrossTraffic1:

CrossTraffic1
-------------

.. c:function:: signed CrossTraffic1(struct plc *plc)

Instruct a powerline device  to send a generic Ethernet frame to another powerline device for a fixed period of time in order to set the ``TX`` rate on the source device and the ``RX`` rate on the destination device. The source and destination address are specified in ``plc->MAC`` and ``plc->RDA``, respectively, and the duration is specified in ``plc->timeperiod``. This function is similar to function :ref:`CrossTraffic2 <support-CrossTraffic2>` but the source and destination addresses are reversed. Structure ``plc`` is declared in file `plc.h <plc.h.html>`_ and defined in file `plc.c <plc.c.html>`_. The function is declared in `plc.h <plc.h.html>`_ and defined in file `CrossTraffic1.c <CrossTraffic1.c.html>`_.

.. _support-CrossTraffic2:

CrossTraffic2
-------------

.. c:function:: signed CrossTraffic2(struct plc *plc)

Instruct a powerline device  to send a generic Ethernet frame to another powerline device for a fixed period of time in order to set the ``TX`` rate on the source device and the ``RX`` rate on the destination device. The source and destination address are specified in ``plc->RDA`` and ``plc->MAC``, respectively, and the duration is specified in ``plc->timeperiod``. This function is similar to function :ref:`CrossTraffic1 <support-CrossTraffic1>` but the source and destination addresses are reversed. Structure ``plc`` is declared in file `plc.h <plc.h.html>`_ and defined in file `plc.c <plc.c.html>`_. The function is declared in `plc.h <plc.h.html>`_ and defined in file `CrossTraffic2.c <CrossTraffic2.c.html>`_.

.. _support-LocalTraffic1:

LocalTraffic1
-------------

.. c:function:: signed LocalTraffic1(struct plc *plc)

Send a fixed number of generic frames from the local host a remote powerline device in order to set the ``TX`` rate on the local powerline device and the ``RX`` rate on the remote powerline device. The local and remote device address are specified in ``plc->MAC`` and ``plc->RDA``, respectively, and the count is specified in ``plc->count``. This function is similar to function :ref:`LocalTraffic2 <support-LocalTraffic2>` which instructs a remote device to echo frames back to the local device. Structure ``plc`` is declared in file `plc.h <plc.h.html>`_ and defined in file `plc.c <plc.c.html>`_. The function is declared in `plc.h <plc.h.html>`_ and defined in file `LocalTraffic1.c <LocalTraffic1.c.html>`_.

.. _support-LocalTraffic2:

LocalTraffic2
-------------

.. c:function:: signed LocalTraffic2(struct plc *plc)

Instruct a remote powerline device to echo generic Ethernet frames back to the local host for a fixed period of time in order to set the ``TX`` rate on the remote powerline device and the ``RX`` rate on the local powerline device. The local and remote device address are specified in ``plc->MAC`` and ``plc->RDA``, respectively, and the count is specified in ``plc->count``. This function is useful when there is no easy way to send traffic from the remote host. This function complements function :ref:`LocalTraffic1 <support-LocalTraffic1>` which send frames from the local device to a remote device. Structure ``plc`` is declared in file `plc.h <plc.h.html>`_ and defined in file `plc.c <plc.c.html>`_. The function is declared in `plc.h <plc.h.html>`_ and defined in file `LocalTraffic2.c <LocalTraffic2.c.html>`_.

.. _support-NetworkTraffic1:

NetworkTraffic1
---------------

.. c:function:: signed NetworkTraffic1(struct plc *plc)

This function is an alternative to function :ref:`NetworkTraffic2 <support-NetworkTraffic2>` which uses Qualcomm Version ``1`` management messages to determine network topology. Structure ``plc`` is declared in file `plc.h <plc.h.html>`_ and defined in file `plc.c <plc.c.html>`_. The function is declared in `plc.h <plc.h.html>`_ and defined in file `NetworkTraffic1.c <NetworkTraffic1.c.html>`_.

This function use ``VS_SW_INFO`` and ``VS_NW_INFO`` messages to discover powerline network topology then calls functions :ref:`LocalTraffic1 <support-LocalTraffic1>` and :ref:`LocalTraffic2 <support-LocalTraffic2>` to generate traffic between the local powerline device and each remote powerline device. Unlike function :ref:`NetworkTraffic1 <support-NetworkTraffic1>`, it does not generate traffic between various remote devices.

.. _support-NetworkTraffic2:

NetworkTraffic2
---------------

.. c:function:: signed NetworkTraffic2(struct plc *plc)

This function is an alternative to function :ref:`NetworkTraffic1 <support-NetworkTraffic1>` which uses Qualcomm Version ``0`` management messages to determine network topology. Structure ``plc`` is declared in file `plc.h <plc.h.html>`_ and defined in file `plc.c <plc.c.html>`_. The function is declared in `plc.h <plc.h.html>`_ and defined in file `NetworkTraffic2.c <NetworkTraffic2.c.html>`_.

.. _support-Traffic1:

Traffic1
--------

.. c:function:: signed Traffic1(struct plc *plc)

This function is an alternative to function :ref:`Traffic2 <support-Traffic2>` which uses Qualcomm Version ``1`` management messages to determine network topology. Structure ``plc`` is declared in file `plc.h <plc.h.html>`_ and defined in file `plc.c <plc.c.html>`_. The function is declared in `plc.h <plc.h.html>`_ and defined in file `Traffic1.c <Traffic1.c.html>`_.

This function use ``VS_SW_INFO`` and ``VS_NW_INFO`` messages to discover powerline network topology then calls functions :ref:`LocalTraffic1 <support-LocalTraffic1>` and :ref:`LocalTraffic2 <support-LocalTraffic2>` to generate traffic between the local powerline device and each remote powerline device. Unlike function :ref:`NetworkTraffic1 <support-NetworkTraffic1>`, it does not generate traffic between remote devices.

.. _support-Traffic2:

Traffic2
--------

.. c:function:: signed Traffic2(struct plc *plc)

This function is an alternative to function :ref:`Traffic1 <support-Traffic1>` which uses Qualcomm Version ``0`` management messages to determine network topology. Structure ``plc`` is declared in file `plc.h <plc.h.html>`_ and defined in file `plc.c <plc.c.html>`_. The function is declared in `plc.h <plc.h.html>`_ and defined in file `Traffic2.c <Traffic2.c.html>`_.

This function use ``VS_SW_INFO`` and ``VS_NW_INFO`` messages to discover powerline network topology then calls functions :ref:`LocalTraffic1 <support-LocalTraffic1>` and :ref:`LocalTraffic2 <support-LocalTraffic2>` to generate traffic between the local powerline device and each remote powerline device. Unlike function :ref:`NetworkTraffic2 <support-NetworkTraffic2>`, it does not generate traffic between remote devices.

.. _slac:

SLAC Functions and Variables
============================

The functions described in this chapter implement SLAC protocol building blocks. Essentially, there is one function for each dataflow shown on the SLAC interface diagram. The implementations are not sophisticated. Customers must adapt them their own run-time environments.

To assist in adapting functions fo various environments, SLAC functions all accept three arguments. The first is a session variable that references information that supports a single PEV-EVSE SLAC interaction. The second is a :ref:`channel <support-ethernet>` variable that references information that supports ISO Layer 2 network communications. The third is a message variable that references an IEEE 802.3 frame buffer used to format, transmit, receive and interpret SLAC messages. Applications may create lists, stacks, queues or heaps of session variables to support multiple PEV-EVSE conversations. That has not been done here but may be done in the future.

PEV-HLE determines the application type and security type and stores them in the session variable before attempting a new SLAC session. PEV-HLE also generates and stores the run identifier in the session variable before attempting a new SLAC session. The run identifier is included in messages so that the EVSE-HLE can distinguish frames from different PEV-HLEs when transmitted in broadcast.

.. _session-variable:

The Session Variable
--------------------

The session variable is a structure defined in `slac.h <None>`_. It stores the information needed to manage a single SLAC protocol session between PEV-HLE and EVSE-HLE. It is necessary because there are cases where multiple PEV-HLE attempt to communicate with one EVSE-HLE or vice verse and each communication involves multiple steps and some steps will fail or be abandoned.

session.RunID
    A session identifier used to support multiple PEV-EVSE sessions.
    The PEV-HLE defines a new value with each associateion attempt and broadcasts it to EVSE-HLE in ``CM_SLAC_PARAM.REQ`` ``RunID``.
    EVSE-HLE receive it and store it in their session variable.
    Once association starts, subsequent messages, in both directions, include this identifier so that PEV-HLE and EVSE-HLE can detect and collate messages from the same conversation.

session.APPLICATION_TYPE
    Indicates the context in which the SLAC process is being carried out.
    Currently, the only context is PEV-EVSE association.
    The PEV-HLE sends this value to the EVSE-HLE in EVSE-HLE in ``CM_SLAC_PARAM.REQ`` message ``APPLICATION_TYPE`` field.
    EVSE-HLE receive it and store it in their session variable.

session.SECURITY_TYPE
    Indicates that the sender prefers Secure SLAC for PEV-EVSE matching.
    Currently, the only value used in SLAC demonstration software is ``0`` for "No Security".
    The PEV-HLE defines this value as ``0`` on startup and broadcasts it to EVSE-HLE in in ``CM_SLAC_PARAM.REQ`` message ``SECURITY_TYPE`` field.
    EVSE-HLE receive it and store it in their session variable for later use when sending SLAC messages.

session.RESP_TYPE
    The EVSE-HLE defines this value and sends it to the PEV-HLE in ``CM_SLAC_PARAM.CNF`` message ``RESP_TYPE`` field.
    THe default is ``0`` but this can be changed using "response type =" in profile :file:`evse.ini`.

session.NUM_SOUNDS
    The number of ``CM_MNBC_SOUND.IND`` messages that the PEV-HLE should send during sounding.
    The EVSE-HLE defines this value and sends it to PEV-HLE in ``CM_SLAC_PARAM.CNF`` message ``NUM_SOUNDS`` field.
    The default is ``10`` but this can be changed using "time to soundi =" in profile :file:`evse.ini`.

session.TIME_OUT
    The total time allowed for all session sounding to complete.
    Any msounds arriving after the timeout will be lost.
    The time is expressed in 100 msec increments.
    The EVSE-HLE defines this value and sends it to PEV_HLE in ``CM_SLAC_PARAM.CNF``.
    The default is ``8`` but this can be changed using "number of sounds =" in profile :file:`evse.ini`.

session.AAG []
    The cumulative average attenuation across ``58`` groups of ``16`` carriers.
    The EVSE-HLE computes these averages from multiple ``CM_ATTEN_PROFILE.IND`` and forwards them to PEV-HLE in ``CM_ATTEN_CHAR.IND``.
    PEV_HLE populates array from ``CM_ATTEN_CHAR.IND`` message ``ACVarField.ATTEN_PROFILE.AAG`` field and evaluates it by averaging all values in the array to determine the overall average attenuation.
    If that values is less than or equal to session.limit then a match may be attempted.

session.NumGroups
    The number of elements in array session.AAG.
    This EVSE-HLE defines this number and sends it to PEV-HLE in ``CM_ATTEN_CHAR.IND`` ``ACVarField.NumGroups``.
    For practical purposes, this value of this variable is constant at ``58``.

session.MSOUND_TARGET
    MAC address of EVSE-HLE that will receive ``CM_MNBC_SOUND.IND`` messages.
    The EVSE-HLE defines this address and broadcasts it to PEV-HLE in ``CM_SLAC_PARAM.CNF`` message ``M-SOUND_TRAGET`` field.
    THe PEV-HLE stores this address in its session variable.
    The default is ``FF:FF:FF:FF:FF:FF`` and remains constant.
    This value is not used at this time.

session.FORWARDING_STA
    MAC address of PEV-HLE to receive ``CM_SLAC_PROFILE.IND`` messages.
    The EVSE-HLE defines this address and sends it to PEV-HLE in ``CM_SLAC_PARAM.CNF`` message ``FORWARDING_STA`` field.
    THe PEV-HLE stores this address in its session variable.
    The default is ``FF:FF:FF:FF:FF:FF`` and remains constant.
    This value is ignored when RESP_TYPE=0.

session.PEV_ID
    The vehicle identification number.
    The PEV-HLE sends this value to the EVSE-HLE in ``CM_SLAC_MATCH.REQ`` ``MatchVarField.PEV_ID`` field.
    The default is ``AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA`` but is can be changed using "vehicle identifier =" in profile :file:`pev.ini`.
    The MatchVarField portion of the message forms the transaction record and may be encrypted.

session.PEV_MAC
    PEV-HLE MAC address.
    The PEV-HLE sends this address to the EVSE-HLE in ``CM_SLAC_MATCH.REQ`` ``MatchVarField.PEV_MAC`` field.
    The MatchVarField portion of the message forms the transaction record and may be encrypted.

session.EVSE_ID
    The station identification number.
    The EVSE-HLE sends this value to PEV-HLE in ``CM_SLAC_MATCH.CNF`` message ``MatchVarField.EVSE_ID`` field.
    The default is ``BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB`` but is can be changed using "station identifier =" in profile :file:`evse.ini`.
    The MatchVarField portion of the message forms the transaction record and may be encrypted.

session.EVSE_MAC
    EVSE-HLE MAC address.
    The EVSE-HLE sends this value to PEV-HLE in ``CM_SLAC_MATCH.CNF`` message ``MatchVarField.EVSE_MAC`` field.
    The MatchVarField portion of the message forms the transaction record and may be encrypted.

session.NMK
    The EVSE-PLC Network Membership Key.
    The EVSE-HLE defines this value when the station powers on.
    The EVSE-HLE sends the NMK to the PEV-HLE in ``CM_SLAC_MATCH.CNF`` message ``MatchField.NMK`` field.
    The PEV-HLE uses this value to form a network with the EVSE-PLC during charging.
    The MatchVarField portion of the message forms the transaction record and may be encrypted.
    The default for PEV-HLE is ``50D3E4933F855B7040784DF815AA8DB7``, which is the key derived from network password "HomePlugAV", but that can be changed using "nwtwork membership key =" in profile :file:`pev.ini`.
    The default for EVSE-HLE is ``B59319D7E8157BA001B018669CCEE30D``, which is the key derived from network password "HomePlugAV0123", but that can be changed using "nwtwork membership key =" in profile :file:`evse.ini`.

session.NID
    The EVSE-PLC Network Identifier.
    The EVSE-HLE defines this value when the station powers.
    The EVSE-HLE sends the NID to the PEV-HLE in ``CM_SLAC_MATCH.CNF`` message ``MatchField.NID`` field.
    The PEV-HLE uses this value to form a network with the EVSE-PLC during charging.
    The MatchVarField portion of the message forms the transaction record and may be encrypted.
    The default for PEV-HLE is ``B0F2E695666B03``, which is the key derived from network password "HomePlugAV", but that can be changed using "nwtwork membership key =" in profile :file:`pev.ini`.
    The default for EVSE-HLE is ``026BCBA5354E08``, which is the key derived from network password "HomePlugAV0123", but that can be changed using "nwtwork membership key =" in profile :file:`evse.ini`.

session.RND
    A random value.
    PEV-HLE computes this value and sends it to EVSE-HLE in ``CM_MNBC_SOUND.IND``.
    This value is not used at this time.

session.original_nmk
    The original PEV-PLC Network Membership Key.
    This is the original NMK of the PEV-HLE.
    The PEV-HLE preserves this value before, and restores it after, the charging cycle.
    The Qualcomm Atheros PEV-HLE and EVSE-HLE demonstration software reads these values from profile :file:`pev.ini` and :file:`evse.ini`, respectively, by "network membership key =".

session.original_nid
    The PLC Network Identifier (NID).
    PEV-PLC and EVSE-PLC will each have a different NID at the start of the SLAC process.
    This value must be preserved before, and restored after, charging.
    Once charging is complete, PEV-HLE and EVSE-HLE restore this value to their respective PLC using ``CM_SET_KEY``.
    In a real world implementation, PEV-HLE and EVSE-HLE would read this value from their repective PLC and save it in session.nmk and session.NMK.
    The Qualcomm Atheros PEV-HLE and EVSE-HLE demonstration software reads these values from profile :file:`pev.ini` and :file:`evse.ini`, respectively, by "network identifier =".

session.state
    The current SLAC protocol session state. Qualcomm Atheros PEV-HLE and EVSE-HLE demonstration programs use this value to direct internal state transitions.
    Values range from ``0`` through ``3``.

session.sounds
    The number of MSounds captured received by EVSE-HLE.
    This value may be less that NUM_SOUNDS the requested by the EVSE_HLE due to network interruptions or EVSE-HLE overloading.
    This is the denominator used when computing the average attenuation in array session.AAG.
    The values is set by PEV_HLE from ``CM_ATTEN_CHAR.IND`` message ``NUM_SOUNDS`` field.

session.limit
    The attenuation threshold.
    If the average attenuation value in session.AAG exceeds this value the PEV-HLE will make no attempt to connect the PEV-PLC to the EVSE-PLC.
    The default value is ``10`` but this can be re-defined in profile :file:`pev.ini` by "attenuation threshold =".

session.pause
    The time in milliseconds between MSounds.
    Sounding too frequently does not allow enough time for the EVSE-PLC to sample and compute values.
    Sounding too infrequently wastes time.
    The maximum sample rate may be limited by the quality of the powerline media and traffic passwing through the EVSE-PLC.
    The default is ``10`` but can be re-defined in profile :file:`pev.ini` by "msound pause =".

session.chargetime
    The time in milliseconds allowed for the charging cycle to complete.
    The default is ``10`` but this can be changed using "charge time =" in profile :file:`pev.ini` and :file:`evse.ini`.
    The charge time should be the same for PEV-HLE and EVSE-HLE when running long tests.

session.settletime
    The time in milliseconds allowed for the charging cycle to complete.
    The default is ``10`` but this can be changed using "settle time =" in profile :file:`pev.ini` and :file:`evse.ini`.
    The settle time should be the same for PEV-HLE and EVSE-HLE when running long tests.

session.counter
    The number of chaging session that have take place.
    This variable is not part of the SLAC protocol.
    The PEV-HLE and EVSE-HLE increment this counter each time they enter the "Charging" state and exchange this value in the unused ``CM_SLAC_PARAM.REW`` and ``CM_SLAC_PARAM.CNF`` message ``CipherSuite`` field.
    Runtime options can be used to instruct either the PEV-HLE or EVSE-HLE to terminate if their counts are out-of-sync.

session.flags
    A bit-mapped flagword used to control program flow based on user-specified command line options.

pev_cm_slac_param
-----------------

.. c:function:: signed pev_cm_slac_param(struct session *session, struct channel *channel, struct message *message)

PEV-HLE broadcasts ``CM_SLAC_PARAM.REQ`` messages and waits for a ``CM_SLAC_PARAM.CNF`` from EVSE-HLEs.
This is the first message sent and it initiates SLAC session one some EVSE-HLE responds.
Function pev_cm_slac_param copies session.APPLICATION_TYPE, session.SECURITY_TYPE and session.RunID into ``CM_SLAC_PARAM.REQ`` and broadcasts it to listening EVSE-HLE.

It is possible that multiple EVSE-HLEs will respond but one response is sufficient to proceed.
If a ``CM_SLAC_PARAM.CNF`` is not received within a reasonable time, PEV-HLE sends another another ``CM_SLAC_PARAM.REQ`` message.
PEV-HLE should compare the run identifier in the ``CM_SLAC_PARAM.CNF`` to that stored in the session variable to ensure that it is not processing ``CM_SLAC_PARAM.CNF`` messages from some other session.
PEV-HLE compares ``RunID``, ``APPLICATION_TYPE`` and ``SECURITY_TYPE`` from ``CM_SLAC_PARAM.CNF`` to that stored in the session variable to ensure they have not changed.
PEV-HLE stores ``MSOUND_TARGET``, ``NUM_SOUNDS`` and ``TIME_OUT`` in the session variable for later use.

evse_cm_slac_param
------------------

.. c:function:: signed evse_cm_slac_param(struct session *session, struct channel *channel, struct message *message)

EVSE-HLE receives a ``CM_SLAC_PARAM.REQ`` and responds with a ``CM_SLAC_PARAM.CNF`` message.
This function waits indefinitly for ``CM_SLAC_PARAM.REQ`` then copies ``RunID``, ``APPLICATION_TYPE`` and ``SECURITY_TYPE`` from ``CM_SLAC_PARAM.REQ`` to the session variable for later use.
It then copies session.MSOUND_TARGET, session.NUM_SOUNDS, session.TIME_OUT, session.RESP_TYPE, session.FORWARDING_STA, session.APPLICATION_TYPE, session.SECURITY_TYPE and session.RunID to ``CM_SLAC_PARAM.CNF`` and sends it unicast to the requesting PEV-HLE.

pev_cm_start_atten_char
-----------------------

.. c:function:: signed evse_cm_start_atten_char(struct session *session, struct channel *channel, struct message *message)

PEV-HLE informs listening EVSE-HLE that sounding is about to start by broadcasting ``CM_START_ATTEN_CHAR.IND``.
Function pev_cm_start_char copies session.NUM_SOUNDS, session.TIME_OUT, session.RESP_TYPE, session.FORWARDING_STA and session.RunID to ``CM_START_ATTEN_CHAR.IND``.

The standard recommends that this message be sent three times. Our demonstration software does not do this.

evse_cm_start_atten_char
------------------------

.. c:function:: signed evse_cm_start_atten_char(struct session *session, struct channel *channel, struct message *message)

EVSE-HLE receives ``CM_START_ATTEN_CHAR`` from PEV-HLE and prepares to receive sounds by initializing msound counters and starting a timer.
This function compares ``RunID`` from ``CM_START_ATTEN_CHAR`` to session.RunID to ensure that this message belongs to a known session.
This function compares ``APPLICATION_TYPE``, ``SECURITY_TYPE``, ``NUM_SOUNDS``, ``TIME_OUT``, ``RESP_TYPE`` and ``FORWARDING_STA`` to the session variable to ensure that values have not changed.

pev_cm_mnbc_sound
-----------------

.. c:function:: signed pev_cm_mnbc_sound(struct session *session, struct channel *channel, struct message *message)

PEV-HLE sends periodic ``CM_MNBC_SOUND.IND`` to EVSE-HLE.

evse_cm_mnbc_sound
------------------

.. c:function:: signed evse_cm_mnbc_sound(struct session *session, struct channel *channel, struct message *message)

EVSE-HLE receives interleaved ``CM_MNBC_SOUND.IND`` from PEV-HLE and ``CM_ATTEN_PROFILE.IND`` messages from EVSE-PLC.
This function maintains a running average of attenuation values across each carrier group in array session.AAG.

evse_cm_atten_char
------------------

.. c:function:: signed evse_cm_atten_char(struct session *session, struct channel *channel, struct message *message)

EVSE-HLE sends ``CM_ATTEN_CHAR.IND`` to PEV-HLE.
This function copies session.AAG to ``CM_ATTEN_CHAR.IND``.

pev_cm_atten_char
-----------------

.. c:function:: signed pev_cm_atten_char(struct session *session, struct channel *channel, struct message *message)

PEV-HLE receives ``CM_ATTEN_CHAR.IND`` from EVSE-HLE and acknwoledges with ``CM_ATTEN_CHAR.RSP``.

pev_cm_slac_match
-----------------

.. c:function:: signed pev_cm_slac_match(struct session *session, struct channel *channel, struct message *message)

PEV-HLE sends ``CM_SLAC_MATCH.REQ`` to selected EVSE-HLE and waits for ``CM_SLAC_MATCH.CNF``.
This function evaluates sounds across all carrier groups and compares result to some threshold and selects one EVSE-HLE.
This function copies session.APPLICAION_TYPE, session.SECURITY_TYPE, session.PEV_ID, session.PEV_MAC and session.RunID to ``CM_SLAC_PARAM.REQ`` and sends it unicast to the selected EVSE-HLE.

evse_cm_slac_match
------------------

.. c:function:: signed evse_cm_slac_match(struct session *session, struct channel *channel, struct message *message)

EVSE-HLE listens for ``CM_SLAC_MATCH.REQ`` from PEV-HLE and abandons the session if no indication is received within a reasonable time.
If an indication is received then this function compares ``RunID`` from ``CM_SLAC_MATCH.REQ`` to the session.RunID to ensure the message is part of the current session.
This function compares ``APPLICATION_TYPE`` and ``SECURITY_TYPE`` to the session.APPLICATION_TYPE and session.SECURITY_TYPE to ensure that they have not changed.
This function copies ``PEV_ID`` and ``PEV_MAC`` from ``CM_SLAC_MATCH.REQ`` to the session variable for later use.
This function updates session.NMK and session.NID with random values.
This function copies session.EVSE_ID, session.EVSE_MAC, session.NMK and session.NID to ``CM_SLAC_MATCH.RSP`` and sends it unicast to the selecing PEV-HLE.

evse_cm_set_key
---------------

.. c:function:: signed evse_cm_mnbc_sound(struct session *session, struct channel *channel, struct message *message)

pev_cm_set_key
--------------

.. c:function:: signed pev_cm_mnbc_sound(struct session *session, struct channel *channel, struct message *message)

