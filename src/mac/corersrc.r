#ifdef __UNIX__
  #include <Carbon.r>
#else
  #include <Types.r>
  #if UNIVERSAL_INTERFACES_VERSION > 0x320
    #include <ControlDefinitions.r>
  #endif
#endif

resource 'ldes' ( 128 )
{
	versionZero 
	{
		0 , 
		0 ,
		0 , 
		0 , 
		hasVertScroll , 
		noHorizScroll , 
		0 , 
		noGrowSpace , 
	}
} ;

resource 'ldes' ( 129 )
{
	versionZero 
	{
		0 , 
		0 ,
		0 , 
		0 , 
		hasVertScroll , 
		hasHorizScroll , 
		0 , 
		noGrowSpace , 
	}
} ;

data 'CURS' (10) {
	$"0000 03E0 0630 0808 1004 31C6 2362 2222"            
	$"2362 31C6 1004 0808 0630 03E0 0000 0000"            
	$"0000 03E0 0630 0808 1004 31C6 2362 2222"            
	$"2362 31C6 1004 0808 0630 03E0 0000 0000"            
	$"0007 0008"                                          
};

data 'CURS' (11) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"            
	$"0000 0000 0000 0000 0000 0000 0000 0000"            
	$"0000 0000 0000 0000 0000 0000 0000 0000"            
	$"0000 0000 0000 0000 0000 0000 0000 0000"            
	$"0000 0000"                                          
};

data 'CURS' (12) {
	$"00F0 0088 0108 0190 0270 0220 0440 0440"            
	$"0880 0880 1100 1E00 1C00 1800 1000 0000"            
	$"00F0 00F8 01F8 01F0 03F0 03E0 07C0 07C0"            
	$"0F80 0F80 1F00 1E00 1C00 1800 1000 0000"            
	$"000E 0000"                                          
};

data 'CURS' (13) {
	$"0000 3C00 4200 8100 8100 8100 8100 4300"            
	$"3F80 01C0 00E0 0070 0038 001C 000E 0006"            
	$"0000 3C00 4200 8100 8100 8100 8100 4300"            
	$"3F80 01C0 00E0 0070 0038 001C 000E 0006"            
	$"0004 0003"                                          
};

data 'CURS' (14) {
	$"0000 07E0 1FF0 3838 3C0C 6E0E 6706 6386"            
	$"61C6 60E6 7076 303C 1C1C 0FF8 07E0 0000"            
	$"0540 0FF0 3FF8 383C 7C0E EE0F 6706 E387"            
	$"61C6 E0E7 7076 703E 3C1C 1FFC 0FF0 0540"            
	$"0007 0007"                                          
};

data 'CURS' (15) {
	$"0100 0100 0100 0100 0100 0100 0180 07E0"            
	$"1FF8 1FF8 0000 1FF8 1FF8 1550 1550 1550"            
	$"0100 0100 0100 0100 0100 0100 0180 07E0"            
	$"1FF8 1FF8 0000 1FF8 1FF8 1550 1550 1550"            
	$"0000 0000"                                          
};

data 'CURS' (16) {
	$"00C0 0140 0640 08C0 3180 47FE 8001 8001"            
	$"81FE 8040 01C0 0040 03C0 C080 3F80 0000"            
	$"00C0 01C0 07C0 0FC0 3F80 7FFE FFFF FFFF"            
	$"FFFE FFC0 FFC0 FFC0 FFC0 FF80 3F80 0000"            
	$"0006 000F"                                          
};

data 'CURS' (17) {
	$"0100 0280 0260 0310 018C 7FE3 8000 8000"            
	$"7F80 0200 0380 0200 03C0 0107 01F8 0000"            
	$"0100 0380 03E0 03F0 01FC 7FFF FFFF FFFF"            
	$"FFFF 03FF 03FF 03FF 03FF 01FF 01F8 0000"            
	$"0007 0000"                                          
};

data 'CURS' (18) {
	$"0000 4078 60FC 71CE 7986 7C06 7E0E 7F1C"            
	$"7FB8 7C30 6C30 4600 0630 0330 0300 0000"            
	$"C078 E0FC F1FE FBFF FFCF FF8F FF1F FFBE"            
	$"FFFC FE78 FF78 EFF8 CFF8 87F8 07F8 0300"            
	$"0001 0001"                                          
};


data 'CURS' (19) {
	$"0000 0002 0006 000E 001E 003E 007E 00FE"            
	$"01FE 003E 0036 0062 0060 00C0 00C0 0000"            
	$"0003 0007 000F 001F 003F 007F 00FF 01FF"            
	$"03FF 07FF 007F 00F7 00F3 01E1 01E0 01C0"            
	$"0001 000E"                                          
};

data 'CURS' (20) {
	$"0000 0080 01C0 03E0 0080 0080 0080 1FFC"            
	$"1FFC 0080 0080 0080 03E0 01C0 0080 0000"            
	$"0080 01C0 03E0 07F0 0FF0 0180 3FFE 3FFE"            
	$"3FFE 3FFE 01C0 0FF0 07F0 03E0 01C0 0080"            
	$"0007 0007"                                          
};

data 'CURS' (21) {
	$"0000 0080 01C0 03E0 0080 0888 188C 3FFE"            
	$"188C 0888 0080 03E0 01C0 0080 0000 0000"            
	$"0080 01C0 03E0 07F0 0BE8 1DDC 3FFE 7FFF"            
	$"3FFE 1DDC 0BE8 07F0 03E0 01C0 0080 0000"            
	$"0007 0008"                                          
};

data 'CURS' (22) {
	$"0000 001E 000E 060E 0712 03A0 01C0 00E0"            
	$"0170 1238 1C1C 1C0C 1E00 0000 0000 0000"            
	$"007F 003F 0E1F 0F0F 0F97 07E1 03E0 01F0"            
	$"31F8 3A7C 3C3E 3E1E 3F0E 3F00 0000 0000"            
	$"0007 0008"                                          
};

data 'CURS' (23) {
	$"0000 7800 7000 7060 48E0 05C0 0380 0700"            
	$"0E80 1C48 1838 0038 0078 0000 0000 0000"            
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            
	$"0006 0006"                                          
};

data 'CURS' (24) {
	$"0006 000E 001C 0018 0020 0040 00F8 0004"            
	$"1FF4 200C 2AA8 1FF0 1F80 3800 6000 8000"            
	$"000F 001F 003E 007C 0070 00E0 01FC 3FF6"            
	$"7FF6 7FFE 7FFC 7FF8 3FF0 7FC0 F800 E000"            
	$"000A 0006"                                          
};


