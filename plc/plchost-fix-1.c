
if (action == 0x02) 

{
	close (plc->PIB.file);
	if (ReadParameters2 (plc)) 
	{
		return (-1);
	}
	if ((plc->PIB.file = open (plc->PIB.name = plc->pib.name, O_BINARY|O_RDONLY)) == -1) 
	{
		error (1, errno, "%s", plc->PIB.name);
	}
	if (ResetDevice (plc)) 
	{
		return (-1);
	}
	continue;
}

if (action == 0x03) 

{
	close (plc->PIB.file);
	if (ReadParameters2 (plc)) 
	{
		return (-1);
	}
	if ((plc->PIB.file = open (plc->PIB.name = plc->pib.name, O_BINARY|O_RDONLY)) == -1) 
	{
		error (1, errno, "%s", plc->PIB.name);
	}
	close (plc->NVM.file);
	if (ReadFirmware2 (plc)) 
	{
		return (-1);
	}
	if ((plc->NVM.file = open (plc->NVM.name = plc->nvm.name, O_BINARY|O_RDONLY)) == -1) 
	{
		error (1, errno, "%s", plc->NVM.name);
	}
	if (ResetDevice (plc)) 
	{
		return (-1);
	}
	continue;
}

