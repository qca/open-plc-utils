.. _chapter-scripting:

Scripting
#########

.. _scripting-intro:

Introduction
============

The Open Powerline Toolkit comes with a variety of example scripts in the :file:`scripts` folder. They have proven useful to Atheros engineers but may not satisfy your specific needs. This chapter explains how some of these scripts work so that you can modify them to do what you need to do. Most scripts covered in this chapter are intended for a :ref:`Powerline Workstation <hardware-workstation>`.

Atheros provides GNU :program:`bash` scripts as examples and does not guarantee that they are suitable in any or all situations. Furthermore,  Atheros may not always update them to reflect changes to toolkit programs. Toolkit command line syntax may change from one release to the next and so Atheros cannot guarantee that older scripts will work with newer programs. The scrips are small,  simple and well organized. Do not run them until you have read them and understand what they do and how they work.

Sample scripts are available in the :file:`scripts` folder. Some extremely useful ones are `flash.sh <flash.sh.html>`_, `upgrade.sh <upgrade.sh.html>`_ and `traffic.sh <traffic.sh.html>`_. They all reference files `hardware.sh <hardware.sh.html>`_ and `firmware.sh <firmware.sh.html>`_.

Although Toolkit programs run under Windows their power is realized through scripts. Scripting requires environmental support for sophisticated symbol substitution and expansion. Most Linux shells are suitable but Microsoft DOS is not. On Windows, consider using installing Cygwin and using their bash shell. Scripting languages :program:`Tcl` and :program:`Python` are suitable alternatives. Another alternative is to write small Windows programs that format and execute the DOS shell commands you want.

.. _scripting-basics:

Linux Script Basics
===================

We summarize some scripting rules here for those who may not be familiar with Linux scripting. If you do not understand the rules or the syntax or have questions then you should consult Linux documentation available on the internet.

On Linux, all commands typed in a console window are read and interpreted by a command interpreter called a :program:`shell` program. The are several shell programs available for Linux but we use :program:`bash` exclusively. If you elect to use a different shell program,  like :program:`csh`,  then you will probably need to modify our scripts.

If you do not set execute permissions you must submit it to the shell for execution. If you set execute permissions on a script file then you can execute it directly by typing the filename. You can set execute permission on Linux with the :program:`chmod` utility as shown in the example below.

.. code-block:: console

    $ bash script.sh
    ...
    $ chmod 0755 script.sh
    $ script.sh
    ...

The example above show how to run a script file using :program:`bash` when it does not have execute permissions set. It then shows how to set the execute permissions and run it directly without using :program:`bash`.

It is good practice to insert a bang-path on the first line of a script to ensure is is executed by the correct shell program, regardless of the shell used to open the script. It is possible to open a script with one shell and have it execute under another shell.

.. code-block:: bash

    #!/bin/bash

The next example shows an example bang-path. The first two characters inform the shell that this file contains scripting commands. The remainder of the bang-path line includes a program path with any options and arguments needed by the program. The shell will invoke that program and pass the rest of the script file to the program as standard input.

Symbols are defined with the symbol name then an equals sign then the definition text. By convention, symbol names use uppercase letters. The definition consists of all characters from the equals sign to the line end. If a definition contains white space, enclose it in quotes so that it is treated as one string when expanded. Symbol scope ends with the defining file unless it is exported.

.. code-block:: bash

    NIC=eth0
    MAC=00:B0:52:00:12:34
    MFG="Intergalactic Software Pirates, Inc."
    ...
    echo NIC is ${NIC} MAC is ${MAC} MFG is ${MFG}

To reference a symbol, precede the variable name with a dollar sign. Atheros scripts go one step further by enclosing the symbol name is braces. This improves readability and simplifies global search-and-replace operations.

.. _scripting-independence:

Host Independence
=================

Different hosts may use interfaces for different purposes. For example, one host might use ``eth0`` for local network communications and ``eth1`` for powerline communications. Another host might do the opposite. A portability problem is created when scripts use the literal interface names on the command line,  as illustrated below:

.. code-block:: bash

    #!/bin/bash

    int6k -i eth1 -r
    int6k -i eth2 -r

The commands shown above will work on a host where ``eth1`` and ``eth2`` are used for powerline communications but will not work on another host where ``eth1`` or ``eth2`` are configured differently. Editing scripts can become a chore when they contain many interface references. One solution is the consistent use of symbols. For example, the following example provides some degree of portability.

.. code-block:: bash

    #!/bin/bash

    NIC1=eth1
    NIC2=eth2

    int6k -i ${NIC1} -r
    int6k -i ${NIC2} -r

The commands shown above are an improvment because symbols ``NIC1`` and ``NIC2`` can be edited once;  however, if you frequently move many scripts from one host to another then each script must be changed. That can also become a chore. A better solution is to define the symbols ``NIC1`` and ``NIC2`` once in a single file and then include the definitions in scripts that need them. For example,  if we created file :file:`hardware.sh` like so ...

.. code-block:: bash

    #!/bin/bash
    # file: scripts/hardware.sh

    NIC1=eth1
    NIC2=eth2

... then we could include it in one or more other scripts,  like so ...

.. code-block:: bash

    . ../scripts/hardware.sh

    int6k -i ${NIC1} -r
    int6k -i ${NIC2} -r

On Linux, the ``.`` command causes the named file to be included in-line as though it were part of the including file. This elminates the need to ``export`` symbol definitions. A full discussion of Linux environment variable scope can be found on the internet. The point is that each host should have it's own definitions files stored in a common folder so that other scripts can include them and reference them in a consistent manner.

Atheros example scripts include two definitions files: :file:`hardware.sh` and :file:`firmware.sh`. File :file:`hardware.sh` defines hardware related symbols as shown below and file :file:`firmware.sh` defines firmware and configuration filenames. They reside in a :file:`scripts` folder and relative path is used to access them. This has proven to work well in most situations.

.. _scripting-hardware-definitions:

hardware.sh
-----------

You should create a :file:`hardware.sh` file in a common folder on each host where you want to execute toolkit scripts. In this way, a script created on one host can be executed on another host without modification.

.. code-block:: bash

    #!/bin/bash
    # file: scripts/hardware.sh

    NIC1=eth1
    NIC2=eth2
    MAC1=00:50:04:A5:D9:5A
    MAC2=00:01:03:2B:03:67

    DUT=eth1

File :file:`hardware.sh` assigns specific values to symbols that are used in many of the scripts found in the :file:`scripts` folder. Some Atheros scripts uses all these symbols and some do not. By convention,  ``NIC1`` and ``NIC2`` name the Ethernet interfaces connected to a Golden Node and Device Under Test. ``MAC1`` and ``MAC2`` are the hardware addresses of ``NIC1`` and ``NIC2``,  respectively. These symbols can be referenced in scripts with references like ``${NIC1}`` or ``${MAC1}``. Of course, you could define other symbols here, as well. See the script under :ref:`Device Upgrade <procedure-upgrade-device>` as one example of how file :file:`hardware.sh` can be included in another script.

Some scripts, such as `flash.sh <flash.sh.html>`_ and `upgrade.sh <upgrade.sh.html>`_, only operate on one device and do not need to define both ``NIC1`` and ``NIC2``. By convention, these scripts reference interface ``DUT`` only.

.. _scripting-firmware-definitions:

firmware.sh
-----------

You should create a :file:`firmware.sh` file in a common folder on each host where you want to execute toolkit scripts. In this way, a script created on one host can be executed on another host without modification.

.. code-block:: bash

    #!/bin/bash
    # file: scripts/firmware.sh

    CFG=sdram16mb.cfg
    CFG=sdram64mb.cfg

    PIB=v3.3.0.pib
    NVM=v3.3.0-0-5-A-FINAL.nvm
    NVM=v3.3.0-0-5-B-FINAL.nvm

File :file:`firmware.sh` assigns specific filenames to symbols that are used in some of the scripts found in the :file:`scripts` folder. Some Atheros scripts use all of these symbols and some do not. By convention,  ``CFG`` defines the SDRAM configuration file used to initialize an *INT6000* or *INT6300* device, ``PIB`` defines the Parameter Information Block file to be used and ``NVM`` defines the firmware image file to be used.

This file is especially useful when working with a specific version of firmware. If there are multiple definitions for a symbol, the last definition is the one that takes effect. At Atheros, this file often contains dozens of definitions and we merely move or copy the ones we want to the end of the file. Our custom scripts then operate on the same configuration, parameter and firmware files until we reorder the definitions in :file:`firmware.sh`.

.. _scripting-A:

Checking Device Connection
==========================

You may want to confirm that a device is actually connected to an Etherenet interface before attempting to run a script. Program `int6kwait <int6kwait.7.html>`_ can be used for this purpose. We often print a brief message to alert the operator that there is no connection or the device has no power applied.

.. code-block:: bash

	int6kwait -xqsi ${NIC1}
	if [ ${?} -ne 0 ]; then
		echo "Device is not connected"
		exit 1
	fi

In the above example,  we invoke `int6kwait <int6kwait.7.html>`_ to poll the device connected to a specific Ethernet interface (``-i``) until the firmware starts (``-s``). The program will return a non-zero return value (``-x``) if the device does not start within a given period of time. By default, the timeout period is 60 seconds. On return,  we check the return code then print an error message and exit the script on timeout. Symbol NIC1 must be defined earlier in the script,  possibly in :ref:`hardware.sh <scripting-hardware-definitions>`.

.. _scripting-random-identity:

Random Device Identity
======================

Some Atheros scripts need random MAC, DAK or NMK strings as arguments to Toolkit programs. Program `rkey <rkey.7.html>`_ can be used for this purpose. We demonstrate one way to define symbols here and demonstrate how to use symbols later on.

.. code-block:: bash

    DAK=$(rkey secret.key -D)
    NMK=$(rkey secret.key -M)

The first statement,  above,  uses program `rkey <rkey.7.html>`_ to read file :file:`secret.key` and compute a random DAK (``-D``). The ouput string is assigned to symbol DAK. The second statement uses program `rkey <rkey.7.html>`_ to read the file :file:`secret.key` and compute a random NMK (``-M``). The output is assigned to symbol NMK.

.. _scripting-actual-identity:

Actual Device Identity
======================

Some scripts need actual MAC, DAK or NMK strings as arguments to Toolkit programs. Program `int6kid <int6kid.7.html>`_ can be used for this purpose. We demonstrate one way to define symbols here and demonstrate how to use them later on.

.. code-block:: bash

    DAK=$(int6kid -Di ${NIC1})
    NMK=$(int6kid -Mi ${NIC1})

The first statement,  above,  uses program `int6kid <int6kid.7.html>`_ to echo the DAK (``-D``) from the device connected to Ethernet interface ``NIC1`` and assign the output to symbol DAK. The second statement uses program `int6kid <int6kid.7.html>`_ to echo the NMK (``-M``) of the device connected to Ethernet interface ``NIC1`` and assign the output to symbol NMK.

.. _scripting-edit-PIB:

Editing a PIB
=============

Many scripts place a device in a known state by editing a PIB file and writing it to the device before starting an operation. Program `modpib <modpib.7.html>`_ can be used for this purpose. It is safe because only certain values can be changed and, where necessary, certain cross-parameter computations are performed automatically.

.. code-block:: bash

	modpib -C 0 -M next -D ${DAK} -N ${NMK} ${PIB}
	if [ ${?} -ne 0 ]; then
		exit 1
	fi

The example above uses `modpib <modpib.7.html>`_ to set the CCo state (``-C``), the MAC address (``-M``), the DAK (``-D``) and the NMK (``-N``) in the defined ``PIB`` file. Argument ``0`` means "CCo Auto" and  argument ``next`` increments the MAC address. The DAK and NMK were defined earlier in the script. Symbol PIB must be defined earlier in the script,  possibly in :ref:`firmware.sh <scripting-firmware-definitions>`.

Program `modpib <modpib.7.html>`_ will print an error message and return a non-zero value on error. We test the return value and exit the script on error to avoid subsequent errors. One could, of course, suppress normal output (``-q``) and print your own error message using the Linux ``echo`` utility.

.. _scripting-initialise-device:

Initialize a Device
===================

Initializing a device involves downloading memory configuration parameters, runtime firmware and runtime parameters into SDRAM and then starting the runtime firmware to make the device fully functional. Program `int6kf <int6kf.7.html>`_ can be used for this purpose when the INT6000 :program:`Softloader` or INT6300 :program:`Bootloader` is running. Additionally, program `int6kf <int6kf.7.html>`_ can be used to flash blank or corrupted NVRAM once runtime firmware has started.

Device initialization is only necessary when a device that has no NVRAM or has corrupted NVRAM or has a :program:`Softloader` stored in NVRAM. It is only possible when either the INT6000 :program:`Softloader` or INT6300 :program:`Bootloader` is running. See :ref:`The Boot Process <firmware-bootload>` for detailed information.

.. code-block:: bash

	int6kf -i ${NIC1} -C ${CFG} -P ${PIB} -N ${NVM}
	if [ ${?} -ne 0 ]; then
		exit 1
	fi

The example above uses program `int6kf <int6kf.7.html>`_ to download an SDRAM configuration file (``-C``), PIB file (``-P``) and NVM file (``-N``) then start firmware execution. Symbols ``NIC1``, CFG, PIB and NVM must be defined earlier in the script, perhaps in files :ref:`hardware.sh <scripting-hardware-definitions>` and :ref:`firmware.sh <scripting-firmware-definitions>`.

Program `int6kf <int6kf.7.html>`_ returns a non-zero value on error. We can check the return code and exit the script on error to avoid subsequent errors. We could, of course, suppress normal output (``-q``) and print our own error message using the Linux ``echo`` utility.

In some cases, we may want to flash a blank or corrupted NVRAM after the runtime firmware has started. We could use program `int6k <int6k.7.html>`_ for this purpose but program `int6kf <int6kf.7.html>`_ can be used, as well. Essentially, it initializes the device (as above) then downloads the PIB and NVM files again and flashes them into NVRAM.

.. code-block:: bash

	int6kf -i ${NIC1} -C ${CFG} -P ${PIB} -N ${NVM} -FF
	if [ ${?} -ne 0 ]; then
		exit 1
	fi

The example above initializes a device with an SDRAM configuration file (``-C``),  PIB file (``-P``) and NVM file (``-N``) as before. Once the runtime firmware has started, the PIB and NVM files are downloaded again and flashed (``-FF``) into NVRAM.

The force-flash option (``-FF``) is needed in this case because runtime firmware that has been written directly to SDRAM and started by the local host assumes there is no NVRAM present to flash or there might be :program:`Softloader` firmware stored in NVRAM that must be protected from accidental flashing. Runtime firmware that has been read from NVRAM need not make that assumption.

.. _scripting-update-device:

Update a Device
===============

In some cases we want to replace the runtime firmware or runtime parameters stored in NVRAM. Program `int6k <int6k.7.html>`_ can be used for this purpose when runtime firmware is running.

A device update may be necessary when new runtime firmware or new runtime parameters or both must be programmed into NVRAM. It is only possible when NVRAM is present and the runtime firmware is running.

.. code-block:: bash

	int6k -i ${NIC1} -P ${PIB} -N ${NVM} -F
	if [ ${?} -ne 0 ]; then
		exit 1
	fi

The example above uses `int6k <int6k.7.html>`_ to download a PIB file (``-P``) and NVM file (``-N``) then flash NVRAM (``-F``). Symbol ``NIC1`` must be defined earlier in the script, perhaps in file :ref:`hardware.sh <scripting-hardware-definitions>`. Symbols PIB and NVM must also be defined earlier in the script, perhaps in file :ref:`firmware.sh <scripting-firmware-definitions>`.

Program `int6k <int6k.7.html>`_ returns a non-zero value on error. We can check the return code and exit the script on error to avoid subsequent errors. We could, of course, suppress normal output (``-q``) and print our own error message using the Linux ``echo`` utility.

In some cases, you may want to preserve the current firmware on a device and update only the PIB. Program `int6k <int6k.7.html>`_ can be used for this purpose, as well.

.. code-block:: bash

	int6k -i ${NIC1} -P ${PIB} -C 2
	if [ ${?} -ne 0 ]; then
		exit 1
	fi

The example above downloads a PIB file (``-P``) and commits (``-C``) the PIB only to NVRAM. There are a variety of device flash scenarios and each requires some varation on one of the examples shown here.

.. _scripting-traffic:

Generating Powerline Traffic
============================

Once two devices associate, you must send data across the powerline in both direction so that each device can compute a TX and RX PHY rate. One method is to use the efsu program that comes with the Toolkit. The program is designed to send free-form Ethernet frames for test and debug purposes but we can also use it to send lots of junk frames in either direction.

.. code-block:: bash

	FRAME=frame.hex
	COUNT=1000
	efsu -i ${NIC1} -hd ${MAC2} ${FRAME} -l ${COUNT}
	efsu -i ${NIC2} -hd ${MAC1} ${FRAME} -l ${COUNT}
	efsu -i ${NIC1} -hd ${MAC2} ${FRAME} -l ${COUNT}
	efsu -i ${NIC2} -hd ${MAC1} ${FRAME} -l ${COUNT}

The example above uses `efsu <efsu.7.html>`_ to send the generic Ethernet frame stored in file :file:`frame.hex` between the two Ethernet interfaces ``NIC1`` and ``NIC2`` (``-i``). In this example, ``NIC1`` and ``NIC2`` are both installed in the host. We have `efsu <efsu.7.html>`_ insert the host source address in the OSA field (``-h``) and destination address in the ODA field (``-d``) of each frame as it is sent. The entire operation sends the same frame ``1000`` times (``-l``) in each direction, twice.

Other methods of generating bi-directional traffic can be used but this method is simple, needs nothing other than the toolkit programs and permits easy synchronization. Program `efsu <efsu.7.html>`_ is designed to send custom Etherenet frames, not measure or control data rates,  but the toolkit includes open source program `ttcp <ttcp.7.html>`_ which is designed to measure and control data rates.

.. _scripting-phy-rates:

Reading PHY Rates
=================

Atheros powerline devices automatically compute their average PHY rate which can be read and displayed. Average PHY rate is an indication of performance and can be affected by attenuation and ambient powerline noise. Program `int6krate <int6krate.7.html>`_ is designed to report the average PHY rate.

.. code-block:: bash

	int6krate -xni ${NIC2}
	if [ ${?} -ne 0 ]; then
		echo "Network did not converge"
		exit 1
	fi

The example above uses `int6krate <int6krate.7.html>`_ to read and display the average TX and RX PHY rates for the device connected to Ethernet interface ``NIC2``. Option ``-n`` requests PHY rates. Option ``-x`` exits with a status that can be tested in the subsequent "if" statement. Output appears in fix-width columns to facilitate post-processing. Other ways to obtain PHY rates are "``int6k -m``" and "``int6kstat -t``".

.. _scripting-interaction:

User Interaction
================

Scripts should be designed, whenever possible, to perform correctly without user intervention but there are cases where user intervention is appropriate. The following example illustrates one :program:`bash` shell method that only requires user input when default values are wrong.

.. code-block:: bash

	MAC=00:B0:52:00:BA:BE
	echo -n "MAC Address [${MAC}]: "; read
	if [ ! -z ${REPLY} ]; then
		MAC="${REPLY}"
	fi

First, we define symbol MAC with a default value. The Linux :program:`echo` utility prints a prompt on the console that includes the symbol value. The trailing newline is suppressed (``-n``) so that text can be typed immediately after the prompt. The :program:`echo` command is terminated with semicolon (``;``) so that another command can be included on the same line. The shell :program:`read` statement waits for the user to type something and press the ``enter`` key. The shell will assign the input to shell variable ``REPLY``. The value of ``REPLY`` is evaluated and used to redefine the symbol only if the input was a non-zero length string.

::

    MAC Address [00:B0:52:00:BA:BE]:

The user will see something like this. If the value is correct the user can press the ``enter`` key to generate a zero length string. Otherwise, the user can type the correct value before pressing the ``enter`` key.

