//============================================================================
// Name        : convertMC68376.cpp
// Author      : Obeisance
// Version     :
// Copyright   : Free to use for all
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

string replaceString(string original, string searchTerm, string replaceTerm);
//searches original for searchTerm, if found will replace with replaceTerm
//otherwise, returns original

string extractAddress(string original);
//extracts an address as indicated by a hex preceded by '($' and terminated by ')'

int main() {
	ifstream myfile("assemblyVersion.txt");
	ofstream reWrite("taggedAssemblyVersion.txt", ios::trunc);


	//we will search the entire text file for a set of addresses which will be
	//replaced with a corresponding set of identifiers

	//SIM address map
	char fffa00[7] = {'$','f','f','f','a','0','0'};//SIM module configuration register (SIMCR)
	char fffa02[7] = {'$','f','f','f','a','0','2'};//SIM test register (SIMTR)
	char fffa04[7] = {'$','f','f','f','a','0','4'};//Clock synthesizer control register (SYNCR)
	char fffa07[7] = {'$','f','f','f','a','0','7'};// | Reset status register (RSR)
	char fffa08[7] = {'$','f','f','f','a','0','8'};//SIM test register E (SIMTRE)
	//string fffa0a = "fffa0a";
	//string fffa0c = "fffa0c";
	//string fffa0e = "fffa0e";
	char fffa11[7] = {'$','f','f','f','a','1','1'};// | Port E data (PORTE0)
	char fffa13[7] = {'$','f','f','f','a','1','3'};// | Port E data (PORTE1)
	char fffa15[7] = {'$','f','f','f','a','1','5'};// | Port E data direction (DDRE)
	char fffa17[7] = {'$','f','f','f','a','1','7'};// | Port E pin assignment (PEPAR)
	char fffa19[7] = {'$','f','f','f','a','1','9'};// | Port F data (PORTF0)
	char fffa1b[7] = {'$','f','f','f','a','1','b'};// | Port F data (PORTF1)
	char fffa1d[7] = {'$','f','f','f','a','1','d'};// | Port F data direction (DDRF)
	char fffa1f[7] = {'$','f','f','f','a','1','f'};// | Port F pin assignment (PFPAR)
	char fffa21[7] = {'$','f','f','f','a','2','1'};// | System protection control (SYPCR)
	char fffa22[7] = {'$','f','f','f','a','2','2'};//Periodic interrupt control register (PICR)
	char fffa24[7] = {'$','f','f','f','a','2','4'};//Periodic interrupt timing register (PITR)
	char fffa27[7] = {'$','f','f','f','a','2','7'};// | Software service (SWSR)
	//string fffa28 = "fffa28";
	//string fffa2a = "fffa2a";
	//string fffa2c = "fffa2c";
	//string fffa2e = "fffa2e";
	char fffa30[7] = {'$','f','f','f','a','3','0'};//Test module master shift A (TSTMSRA)
	char fffa32[7] = {'$','f','f','f','a','3','2'};//Test module master shift B (TSTMSRB)
	char fffa34[7] = {'$','f','f','f','a','3','4'};//Test module shift count (TSTSC)
	char fffa36[7] = {'$','f','f','f','a','3','6'};//Test module repetition counter (TSTRC)
	char fffa38[7] = {'$','f','f','f','a','3','8'};//Test module control (CREG)
	char fffa3a[7] = {'$','f','f','f','a','3','a'};//Test module distributed (DREG)
	//string fffa3c = "fffa3c";
	//string fffa3e = "fffa3e";
	char fffa41[7] = {'$','f','f','f','a','4','1'};// | Port C data (PORTC)
	//string fffa42 = "fffa42";
	char fffa44[7] = {'$','f','f','f','a','4','4'};//Chip select pin assignment (CSPAR0)
	char fffa46[7] = {'$','f','f','f','a','4','6'};//Chip select pin assignment (CSPAR1)
	char fffa48[7] = {'$','f','f','f','a','4','8'};//Chip select base boot (CSBARBT)
	char fffa4a[7] = {'$','f','f','f','a','4','a'};//Chip select option boot (CSORBT)
	char fffa4c[7] = {'$','f','f','f','a','4','c'};//Chip select base 0 (CSBAR0)
	char fffa4e[7] = {'$','f','f','f','a','4','e'};//Chip select option 0 (CSOR0)
	char fffa50[7] = {'$','f','f','f','a','5','0'};//Chip select base 1 (CSBAR1)
	char fffa52[7] = {'$','f','f','f','a','5','2'};//Chip select option 1 (CSOR1)
	char fffa54[7] = {'$','f','f','f','a','5','4'};//Chip select base 2 (CSBAR2)
	char fffa56[7] = {'$','f','f','f','a','5','6'};//Chip select option 2 (CSOR2)
	char fffa58[7] = {'$','f','f','f','a','5','8'};//Chip select base 3 (CSBAR3)
	char fffa5a[7] = {'$','f','f','f','a','5','a'};//Chip select option 3 (CSOR3)
	char fffa5c[7] = {'$','f','f','f','a','5','c'};//Chip select base 4 (CSBAR4)
	char fffa5e[7] = {'$','f','f','f','a','5','e'};//Chip select option 4 (CSOR4)
	char fffa60[7] = {'$','f','f','f','a','6','0'};//Chip select base 5 (CSBAR5)
	char fffa62[7] = {'$','f','f','f','a','6','2'};//Chip select option 5 (CSOR5)
	char fffa64[7] = {'$','f','f','f','a','6','4'};//Chip select base 6 (CSBAR6)
	char fffa66[7] = {'$','f','f','f','a','6','6'};//Chip select option 6 (CSOR6)
	char fffa68[7] = {'$','f','f','f','a','6','8'};//Chip select base 7 (CSBAR7)
	char fffa6a[7] = {'$','f','f','f','a','6','a'};//Chip select option 7 (CSOR7)
	char fffa6c[7] = {'$','f','f','f','a','6','c'};//Chip select base 8 (CSBAR8)
	char fffa6e[7] = {'$','f','f','f','a','6','e'};//Chip select option 8 (CSOR8)
	char fffa70[7] = {'$','f','f','f','a','7','0'};//Chip select base 9 (CSBAR9)
	char fffa72[7] = {'$','f','f','f','a','7','2'};//Chip select option 9 (CSOR9)
	char fffa74[7] = {'$','f','f','f','a','7','4'};//Chip select base 10 (CSBAR10)
	char fffa76[7] = {'$','f','f','f','a','7','6'};//Chip select option 10 (CSOR10)
	//string fffa78 = "fffa78";
	//string fffa7a = "fffa7a";
	//string fffa7c = "fffa7c";
	//string fffa7e = "fffa7e";
	//SRAM address map
	char fffb40[7] = {'$','f','f','f','b','4','0'};//RAM module configuration register (RAMMCR)
	char fffb42[7] = {'$','f','f','f','b','4','2'};//RAM test register (RAMTST)
	char fffb44[7] = {'$','f','f','f','b','4','4'};//RAM array base address register high (RAMBAH)
	char fffb46[7] = {'$','f','f','f','b','4','6'};//RAM array base address register low (RAMBAL)
	//MRM address map
	char fff820[7] = {'$','f','f','f','8','2','0'};//Masked ROM module configuration register (MRMCR)
	//string fff822 = "fff822";
	char fff824[7] = {'$','f','f','f','8','2','4'};//ROM array base address high register (ROMBAH)
	char fff826[7] = {'$','f','f','f','8','2','6'};//ROM array base address low register (ROMBAL)
	char fff828[7] = {'$','f','f','f','8','2','8'};//Signature high register (SIGHI)
	char fff82a[7] = {'$','f','f','f','8','2','a'};//Signiture low register (SIGLO)
	//string fff82c = "fff82c";
	//string fff82e = "fff82e";
	char fff830[7] = {'$','f','f','f','8','3','0'};//ROM boostrap word 0 (ROMBS0)
	char fff832[7] = {'$','f','f','f','8','3','2'};//ROM boostrap word 1 (ROMBS1)
	char fff834[7] = {'$','f','f','f','8','3','4'};//ROM boostrap word 2 (ROMBS2)
	char fff836[7] = {'$','f','f','f','8','3','6'};//ROM boostrap word 3 (ROMBS3)
	//string fff838 = "fff838";
	//string fff83a = "fff83a";
	//string fff83c = "fff83c";
	//string fff83e = "fff83e";
	//QADC address map
	char fff200[7] = {'$','f','f','f','2','0','0'};//Module configuration register (QADCMCR)
	char fff202[7] = {'$','f','f','f','2','0','2'};//Test register (QADCTEST)
	char fff204[7] = {'$','f','f','f','2','0','4'};//Interrupt register (QADCINT)
	char fff206[7] = {'$','f','f','f','2','0','6'};//Port A data (PORTQA) | PORT B data (PORTQB)
	char fff207[7] = {'$','f','f','f','2','0','7'};// | PORT B data (PORTQB)
	char fff208[7] = {'$','f','f','f','2','0','8'};//Port data direction register (DDRQA)
	char fff20a[7] = {'$','f','f','f','2','0','a'};//Control register 0 (QACR0)
	char fff20c[7] = {'$','f','f','f','2','0','c'};//Control register 1 (QACR1)
	char fff20e[7] = {'$','f','f','f','2','0','e'};//Control register 2 (QACR2)
	char fff210[7] = {'$','f','f','f','2','1','0'};//Status register (QASR)
	//fff212-fff22e are reserved
	char fff230[7] = {'$','f','f','f','2','3','0'};//Conversion command word (CCW) table
	char fff232[7] = {'$','f','f','f','2','3','2'};//Conversion command word (CCW) table
	char fff234[7] = {'$','f','f','f','2','3','4'};//Conversion command word (CCW) table
	char fff236[7] = {'$','f','f','f','2','3','6'};//Conversion command word (CCW) table
	char fff238[7] = {'$','f','f','f','2','3','8'};//Conversion command word (CCW) table
	char fff23a[7] = {'$','f','f','f','2','3','a'};//Conversion command word (CCW) table
	char fff23c[7] = {'$','f','f','f','2','3','c'};//Conversion command word (CCW) table
	char fff23e[7] = {'$','f','f','f','2','3','e'};//Conversion command word (CCW) table
	char fff240[7] = {'$','f','f','f','2','4','0'};//Conversion command word (CCW) table
	char fff242[7] = {'$','f','f','f','2','4','2'};//Conversion command word (CCW) table
	char fff244[7] = {'$','f','f','f','2','4','4'};//Conversion command word (CCW) table
	char fff246[7] = {'$','f','f','f','2','4','6'};//Conversion command word (CCW) table
	char fff248[7] = {'$','f','f','f','2','4','8'};//Conversion command word (CCW) table
	char fff24a[7] = {'$','f','f','f','2','4','a'};//Conversion command word (CCW) table
	char fff24c[7] = {'$','f','f','f','2','4','c'};//Conversion command word (CCW) table
	char fff24e[7] = {'$','f','f','f','2','4','e'};//Conversion command word (CCW) table
	char fff250[7] = {'$','f','f','f','2','5','0'};//Conversion command word (CCW) table
	char fff252[7] = {'$','f','f','f','2','5','2'};//Conversion command word (CCW) table
	char fff254[7] = {'$','f','f','f','2','5','4'};//Conversion command word (CCW) table
	char fff256[7] = {'$','f','f','f','2','5','6'};//Conversion command word (CCW) table
	char fff258[7] = {'$','f','f','f','2','5','8'};//Conversion command word (CCW) table
	char fff25a[7] = {'$','f','f','f','2','5','a'};//Conversion command word (CCW) table
	char fff25c[7] = {'$','f','f','f','2','5','c'};//Conversion command word (CCW) table
	char fff25e[7] = {'$','f','f','f','2','5','e'};//Conversion command word (CCW) table
	char fff260[7] = {'$','f','f','f','2','6','0'};//Conversion command word (CCW) table
	char fff262[7] = {'$','f','f','f','2','6','2'};//Conversion command word (CCW) table
	char fff264[7] = {'$','f','f','f','2','6','4'};//Conversion command word (CCW) table
	char fff266[7] = {'$','f','f','f','2','6','6'};//Conversion command word (CCW) table
	char fff268[7] = {'$','f','f','f','2','6','8'};//Conversion command word (CCW) table
	char fff26a[7] = {'$','f','f','f','2','6','a'};//Conversion command word (CCW) table
	char fff26c[7] = {'$','f','f','f','2','6','c'};//Conversion command word (CCW) table
	char fff26e[7] = {'$','f','f','f','2','6','e'};//Conversion command word (CCW) table
	char fff270[7] = {'$','f','f','f','2','7','0'};//Conversion command word (CCW) table
	char fff272[7] = {'$','f','f','f','2','7','2'};//Conversion command word (CCW) table
	char fff274[7] = {'$','f','f','f','2','7','4'};//Conversion command word (CCW) table
	char fff276[7] = {'$','f','f','f','2','7','6'};//Conversion command word (CCW) table
	char fff278[7] = {'$','f','f','f','2','7','8'};//Conversion command word (CCW) table
	char fff27a[7] = {'$','f','f','f','2','7','a'};//Conversion command word (CCW) table
	char fff27c[7] = {'$','f','f','f','2','7','c'};//Conversion command word (CCW) table
	char fff27e[7] = {'$','f','f','f','2','7','e'};//Conversion command word (CCW) table
	//fff280-fff2ae are reserved
	char fff2b0[7] = {'$','f','f','f','2','b','0'};//Result word table (RRJURR)
	char fff2b2[7] = {'$','f','f','f','2','b','2'};//Result word table (RRJURR)
	char fff2b4[7] = {'$','f','f','f','2','b','4'};//Result word table (RRJURR)
	char fff2b6[7] = {'$','f','f','f','2','b','6'};//Result word table (RRJURR)
	char fff2b8[7] = {'$','f','f','f','2','b','8'};//Result word table (RRJURR)
	char fff2ba[7] = {'$','f','f','f','2','b','a'};//Result word table (RRJURR)
	char fff2bc[7] = {'$','f','f','f','2','b','c'};//Result word table (RRJURR)
	char fff2be[7] = {'$','f','f','f','2','b','e'};//Result word table (RRJURR)
	char fff2c0[7] = {'$','f','f','f','2','c','0'};//Result word table (RRJURR)
	char fff2c2[7] = {'$','f','f','f','2','c','2'};//Result word table (RRJURR)
	char fff2c4[7] = {'$','f','f','f','2','c','4'};//Result word table (RRJURR)
	char fff2c6[7] = {'$','f','f','f','2','c','6'};//Result word table (RRJURR)
	char fff2c8[7] = {'$','f','f','f','2','c','8'};//Result word table (RRJURR)
	char fff2ca[7] = {'$','f','f','f','2','c','a'};//Result word table (RRJURR)
	char fff2cc[7] = {'$','f','f','f','2','c','c'};//Result word table (RRJURR)
	char fff2ce[7] = {'$','f','f','f','2','c','e'};//Result word table (RRJURR)
	char fff2d0[7] = {'$','f','f','f','2','d','0'};//Result word table (RRJURR)
	char fff2d2[7] = {'$','f','f','f','2','d','2'};//Result word table (RRJURR)
	char fff2d4[7] = {'$','f','f','f','2','d','4'};//Result word table (RRJURR)
	char fff2d6[7] = {'$','f','f','f','2','d','6'};//Result word table (RRJURR)
	char fff2d8[7] = {'$','f','f','f','2','d','8'};//Result word table (RRJURR)
	char fff2da[7] = {'$','f','f','f','2','d','a'};//Result word table (RRJURR)
	char fff2dc[7] = {'$','f','f','f','2','d','c'};//Result word table (RRJURR)
	char fff2de[7] = {'$','f','f','f','2','d','e'};//Result word table (RRJURR)
	char fff2e0[7] = {'$','f','f','f','2','e','0'};//Result word table (RRJURR)
	char fff2e2[7] = {'$','f','f','f','2','e','2'};//Result word table (RRJURR)
	char fff2e4[7] = {'$','f','f','f','2','e','4'};//Result word table (RRJURR)
	char fff2e6[7] = {'$','f','f','f','2','e','6'};//Result word table (RRJURR)
	char fff2e8[7] = {'$','f','f','f','2','e','8'};//Result word table (RRJURR)
	char fff2ea[7] = {'$','f','f','f','2','e','a'};//Result word table (RRJURR)
	char fff2ec[7] = {'$','f','f','f','2','e','c'};//Result word table (RRJURR)
	char fff2ee[7] = {'$','f','f','f','2','e','e'};//Result word table (RRJURR)
	char fff2f0[7] = {'$','f','f','f','2','f','0'};//Result word table (RRJURR)
	char fff2f2[7] = {'$','f','f','f','2','f','2'};//Result word table (RRJURR)
	char fff2f4[7] = {'$','f','f','f','2','f','4'};//Result word table (RRJURR)
	char fff2f6[7] = {'$','f','f','f','2','f','6'};//Result word table (RRJURR)
	char fff2f8[7] = {'$','f','f','f','2','f','8'};//Result word table (RRJURR)
	char fff2fa[7] = {'$','f','f','f','2','f','a'};//Result word table (RRJURR)
	char fff2fc[7] = {'$','f','f','f','2','f','c'};//Result word table (RRJURR)
	char fff2fe[7] = {'$','f','f','f','2','f','e'};//Result word table (RRJURR)
	//fff300-fff32e are reserved
	char fff330[7] = {'$','f','f','f','3','3','0'};//Result word table (LJSRR)
	char fff332[7] = {'$','f','f','f','3','3','2'};//Result word table (LJSRR)
	char fff334[7] = {'$','f','f','f','3','3','4'};//Result word table (LJSRR)
	char fff336[7] = {'$','f','f','f','3','3','6'};//Result word table (LJSRR)
	char fff338[7] = {'$','f','f','f','3','3','8'};//Result word table (LJSRR)
	char fff33a[7] = {'$','f','f','f','3','3','a'};//Result word table (LJSRR)
	char fff33c[7] = {'$','f','f','f','3','3','c'};//Result word table (LJSRR)
	char fff33e[7] = {'$','f','f','f','3','3','e'};//Result word table (LJSRR)
	char fff340[7] = {'$','f','f','f','3','4','0'};//Result word table (LJSRR)
	char fff342[7] = {'$','f','f','f','3','4','2'};//Result word table (LJSRR)
	char fff344[7] = {'$','f','f','f','3','4','4'};//Result word table (LJSRR)
	char fff346[7] = {'$','f','f','f','3','4','6'};//Result word table (LJSRR)
	char fff348[7] = {'$','f','f','f','3','4','8'};//Result word table (LJSRR)
	char fff34a[7] = {'$','f','f','f','3','4','a'};//Result word table (LJSRR)
	char fff34c[7] = {'$','f','f','f','3','4','c'};//Result word table (LJSRR)
	char fff34e[7] = {'$','f','f','f','3','4','e'};//Result word table (LJSRR)
	char fff350[7] = {'$','f','f','f','3','5','0'};//Result word table (LJSRR)
	char fff352[7] = {'$','f','f','f','3','5','2'};//Result word table (LJSRR)
	char fff354[7] = {'$','f','f','f','3','5','4'};//Result word table (LJSRR)
	char fff356[7] = {'$','f','f','f','3','5','6'};//Result word table (LJSRR)
	char fff358[7] = {'$','f','f','f','3','5','8'};//Result word table (LJSRR)
	char fff35a[7] = {'$','f','f','f','3','5','a'};//Result word table (LJSRR)
	char fff35c[7] = {'$','f','f','f','3','5','c'};//Result word table (LJSRR)
	char fff35e[7] = {'$','f','f','f','3','5','e'};//Result word table (LJSRR)
	char fff360[7] = {'$','f','f','f','3','6','0'};//Result word table (LJSRR)
	char fff362[7] = {'$','f','f','f','3','6','2'};//Result word table (LJSRR)
	char fff364[7] = {'$','f','f','f','3','6','4'};//Result word table (LJSRR)
	char fff366[7] = {'$','f','f','f','3','6','6'};//Result word table (LJSRR)
	char fff368[7] = {'$','f','f','f','3','6','8'};//Result word table (LJSRR)
	char fff36a[7] = {'$','f','f','f','3','6','a'};//Result word table (LJSRR)
	char fff36c[7] = {'$','f','f','f','3','6','c'};//Result word table (LJSRR)
	char fff36e[7] = {'$','f','f','f','3','6','e'};//Result word table (LJSRR)
	char fff370[7] = {'$','f','f','f','3','7','0'};//Result word table (LJSRR)
	char fff372[7] = {'$','f','f','f','3','7','2'};//Result word table (LJSRR)
	char fff374[7] = {'$','f','f','f','3','7','4'};//Result word table (LJSRR)
	char fff376[7] = {'$','f','f','f','3','7','6'};//Result word table (LJSRR)
	char fff378[7] = {'$','f','f','f','3','7','8'};//Result word table (LJSRR)
	char fff37a[7] = {'$','f','f','f','3','7','a'};//Result word table (LJSRR)
	char fff37c[7] = {'$','f','f','f','3','7','c'};//Result word table (LJSRR)
	char fff37e[7] = {'$','f','f','f','3','7','e'};//Result word table (LJSRR)
	//fff380-fff3ae are reserved
	char fff3b0[7] = {'$','f','f','f','3','b','0'};//Result word table (LJURR)
	char fff3b2[7] = {'$','f','f','f','3','b','2'};//Result word table (LJURR)
	char fff3b4[7] = {'$','f','f','f','3','b','4'};//Result word table (LJURR)
	char fff3b6[7] = {'$','f','f','f','3','b','6'};//Result word table (LJURR)
	char fff3b8[7] = {'$','f','f','f','3','b','8'};//Result word table (LJURR)
	char fff3ba[7] = {'$','f','f','f','3','b','a'};//Result word table (LJURR)
	char fff3bc[7] = {'$','f','f','f','3','b','c'};//Result word table (LJURR)
	char fff3be[7] = {'$','f','f','f','3','b','e'};//Result word table (LJURR)
	char fff3c0[7] = {'$','f','f','f','3','c','0'};//Result word table (LJURR)
	char fff3c2[7] = {'$','f','f','f','3','c','2'};//Result word table (LJURR)
	char fff3c4[7] = {'$','f','f','f','3','c','4'};//Result word table (LJURR)
	char fff3c6[7] = {'$','f','f','f','3','c','6'};//Result word table (LJURR)
	char fff3c8[7] = {'$','f','f','f','3','c','8'};//Result word table (LJURR)
	char fff3ca[7] = {'$','f','f','f','3','c','a'};//Result word table (LJURR)
	char fff3cc[7] = {'$','f','f','f','3','c','c'};//Result word table (LJURR)
	char fff3ce[7] = {'$','f','f','f','3','c','e'};//Result word table (LJURR)
	char fff3d0[7] = {'$','f','f','f','3','d','0'};//Result word table (LJURR)
	char fff3d2[7] = {'$','f','f','f','3','d','2'};//Result word table (LJURR)
	char fff3d4[7] = {'$','f','f','f','3','d','4'};//Result word table (LJURR)
	char fff3d6[7] = {'$','f','f','f','3','d','6'};//Result word table (LJURR)
	char fff3d8[7] = {'$','f','f','f','3','d','8'};//Result word table (LJURR)
	char fff3da[7] = {'$','f','f','f','3','d','a'};//Result word table (LJURR)
	char fff3dc[7] = {'$','f','f','f','3','d','c'};//Result word table (LJURR)
	char fff3de[7] = {'$','f','f','f','3','d','e'};//Result word table (LJURR)
	char fff3e0[7] = {'$','f','f','f','3','e','0'};//Result word table (LJURR)
	char fff3e2[7] = {'$','f','f','f','3','e','2'};//Result word table (LJURR)
	char fff3e4[7] = {'$','f','f','f','3','e','4'};//Result word table (LJURR)
	char fff3e6[7] = {'$','f','f','f','3','e','6'};//Result word table (LJURR)
	char fff3e8[7] = {'$','f','f','f','3','e','8'};//Result word table (LJURR)
	char fff3ea[7] = {'$','f','f','f','3','e','a'};//Result word table (LJURR)
	char fff3ec[7] = {'$','f','f','f','3','e','c'};//Result word table (LJURR)
	char fff3ee[7] = {'$','f','f','f','3','e','e'};//Result word table (LJURR)
	char fff3f0[7] = {'$','f','f','f','3','f','0'};//Result word table (LJURR)
	char fff3f2[7] = {'$','f','f','f','3','f','2'};//Result word table (LJURR)
	char fff3f4[7] = {'$','f','f','f','3','f','4'};//Result word table (LJURR)
	char fff3f6[7] = {'$','f','f','f','3','f','6'};//Result word table (LJURR)
	char fff3f8[7] = {'$','f','f','f','3','f','8'};//Result word table (LJURR)
	char fff3fa[7] = {'$','f','f','f','3','f','a'};//Result word table (LJURR)
	char fff3fc[7] = {'$','f','f','f','3','f','c'};//Result word table (LJURR)
	char fff3fe[7] = {'$','f','f','f','3','f','e'};//Result word table (LJURR)
	//QSM address map
	char fffc00[7] = {'$','f','f','f','c','0','0'};//QSM module configuration register (QSMCR)
	char fffc02[7] = {'$','f','f','f','c','0','2'};//QSM test register (QSMTEST)
	char fffc04[7] = {'$','f','f','f','c','0','4'};//QSM interrupt level register (QILR) | QSM interrupt vector registration (QIVR)
	char fffc05[7] = {'$','f','f','f','c','0','5'};// | QSM interrupt vector registration (QIVR)
	//string fffc06 = "fffc06";
	char fffc08[7] = {'$','f','f','f','c','0','8'};//SCI control 0 register (SCCR0)
	char fffc0a[7] = {'$','f','f','f','c','0','a'};//SCI control 1 register (SCCR1)
	char fffc0c[7] = {'$','f','f','f','c','0','c'};//SCI status register (SCSR)
	char fffc0e[7] = {'$','f','f','f','c','0','e'};//SCI data register (SCDR)
	char fffc15[7] = {'$','f','f','f','c','1','5'};// | PQS data register (PORTQS)
	char fffc16[7] = {'$','f','f','f','c','1','6'};//PQS pin assignment register (PQSPAR) | PQS data direction register (DDRQS)
	char fffc17[7] = {'$','f','f','f','c','1','7'};// | PQS data direction register (DDRQS)
	char fffc18[7] = {'$','f','f','f','c','1','8'};//SPI control register 0 (SPCR0)
	char fffc1a[7] = {'$','f','f','f','c','1','a'};//SPI control register 1 (SPCR1)
	char fffc1c[7] = {'$','f','f','f','c','1','c'};//SPI control register 2 (SPCR2)
	char fffc1e[7] = {'$','f','f','f','c','1','e'};//SPI control register 3 (SPCR3) | SPI status register (SPSR)
	char fffc1f[7] = {'$','f','f','f','c','1','f'};// | SPI status register (SPSR)
	//string fffc20-fffcff not used
	char fffd00[7] = {'$','f','f','f','d','0','0'};//Receive RAM (RR[0:F])
	char fffd02[7] = {'$','f','f','f','d','0','2'};//Receive RAM (RR[0:F])
	char fffd04[7] = {'$','f','f','f','d','0','4'};//Receive RAM (RR[0:F])
	char fffd06[7] = {'$','f','f','f','d','0','6'};//Receive RAM (RR[0:F])
	char fffd08[7] = {'$','f','f','f','d','0','8'};//Receive RAM (RR[0:F])
	char fffd0a[7] = {'$','f','f','f','d','0','a'};//Receive RAM (RR[0:F])
	char fffd0c[7] = {'$','f','f','f','d','0','c'};//Receive RAM (RR[0:F])
	char fffd0e[7] = {'$','f','f','f','d','0','e'};//Receive RAM (RR[0:F])
	char fffd10[7] = {'$','f','f','f','d','1','0'};//Receive RAM (RR[0:F])
	char fffd12[7] = {'$','f','f','f','d','1','2'};//Receive RAM (RR[0:F])
	char fffd14[7] = {'$','f','f','f','d','1','4'};//Receive RAM (RR[0:F])
	char fffd16[7] = {'$','f','f','f','d','1','6'};//Receive RAM (RR[0:F])
	char fffd18[7] = {'$','f','f','f','d','1','8'};//Receive RAM (RR[0:F])
	char fffd1a[7] = {'$','f','f','f','d','1','a'};//Receive RAM (RR[0:F])
	char fffd1c[7] = {'$','f','f','f','d','1','c'};//Receive RAM (RR[0:F])
	char fffd1e[7] = {'$','f','f','f','d','1','e'};//Receive RAM (RR[0:F])
	char fffd20[7] = {'$','f','f','f','d','2','0'};//Transmit RAM (TR[0:F])
	char fffd22[7] = {'$','f','f','f','d','2','2'};//Transmit RAM (TR[0:F])
	char fffd24[7] = {'$','f','f','f','d','2','4'};//Transmit RAM (TR[0:F])
	char fffd26[7] = {'$','f','f','f','d','2','6'};//Transmit RAM (TR[0:F])
	char fffd28[7] = {'$','f','f','f','d','2','8'};//Transmit RAM (TR[0:F])
	char fffd2a[7] = {'$','f','f','f','d','2','a'};//Transmit RAM (TR[0:F])
	char fffd2c[7] = {'$','f','f','f','d','2','c'};//Transmit RAM (TR[0:F])
	char fffd2e[7] = {'$','f','f','f','d','2','e'};//Transmit RAM (TR[0:F])
	char fffd30[7] = {'$','f','f','f','d','3','0'};//Transmit RAM (TR[0:F])
	char fffd32[7] = {'$','f','f','f','d','3','2'};//Transmit RAM (TR[0:F])
	char fffd34[7] = {'$','f','f','f','d','3','4'};//Transmit RAM (TR[0:F])
	char fffd36[7] = {'$','f','f','f','d','3','6'};//Transmit RAM (TR[0:F])
	char fffd38[7] = {'$','f','f','f','d','3','8'};//Transmit RAM (TR[0:F])
	char fffd3a[7] = {'$','f','f','f','d','3','a'};//Transmit RAM (TR[0:F])
	char fffd3c[7] = {'$','f','f','f','d','3','c'};//Transmit RAM (TR[0:F])
	char fffd3e[7] = {'$','f','f','f','d','3','e'};//Transmit RAM (TR[0:F])
	char fffd40[7] = {'$','f','f','f','d','4','0'};//Command RAM (CR[0:F])
	char fffd42[7] = {'$','f','f','f','d','4','2'};//Command RAM (CR[0:F])
	char fffd44[7] = {'$','f','f','f','d','4','4'};//Command RAM (CR[0:F])
	char fffd46[7] = {'$','f','f','f','d','4','6'};//Command RAM (CR[0:F])
	char fffd48[7] = {'$','f','f','f','d','4','8'};//Command RAM (CR[0:F])
	char fffd4a[7] = {'$','f','f','f','d','4','a'};//Command RAM (CR[0:F])
	char fffd4c[7] = {'$','f','f','f','d','4','c'};//Command RAM (CR[0:F])
	char fffd4e[7] = {'$','f','f','f','d','4','e'};//Command RAM (CR[0:F])
	char fffd50[7] = {'$','f','f','f','d','5','0'};//Command RAM (CR[0:F])
	char fffd52[7] = {'$','f','f','f','d','5','2'};//Command RAM (CR[0:F])
	char fffd54[7] = {'$','f','f','f','d','5','4'};//Command RAM (CR[0:F])
	char fffd56[7] = {'$','f','f','f','d','5','6'};//Command RAM (CR[0:F])
	char fffd58[7] = {'$','f','f','f','d','5','8'};//Command RAM (CR[0:F])
	char fffd5a[7] = {'$','f','f','f','d','5','a'};//Command RAM (CR[0:F])
	char fffd5c[7] = {'$','f','f','f','d','5','c'};//Command RAM (CR[0:F])
	char fffd5e[7] = {'$','f','f','f','d','5','e'};//Command RAM (CR[0:F])
	//CTM4 - configurable timer module 4 address map
	char fff400[7] = {'$','f','f','f','4','0','0'};//BIUSM module configuration register (BIUMCR)
	char fff402[7] = {'$','f','f','f','4','0','2'};//BIUSM test register (BIUTEST)
	char fff404[7] = {'$','f','f','f','4','0','4'};//BIUSM time base register (BIUTBR)
	char fff408[7] = {'$','f','f','f','4','0','8'};//CPSM control register (CPCR)
	char fff40a[7] = {'$','f','f','f','4','0','a'};//CPSM test register (CPTR)
	char fff410[7] = {'$','f','f','f','4','1','0'};//MCSM2 status/interrupt/control register (MCSM2SIC)
	char fff412[7] = {'$','f','f','f','4','1','2'};//MCSM2 counter (MCSM2CNT)
	char fff414[7] = {'$','f','f','f','4','1','4'};//MCSM2 modulus latch (MCSM2ML)
	//string fff416 = "fff416";
	char fff418[7] = {'$','f','f','f','4','1','8'};//DASM3 status/interrupt/control register (DASM3SIC)
	char fff41a[7] = {'$','f','f','f','4','1','a'};//DASM3 register a (DASM3A)
	char fff41c[7] = {'$','f','f','f','4','1','c'};//DASM3 register b (DASM3B)
	char fff420[7] = {'$','f','f','f','4','2','0'};//DASM4 status/interrupt/control register (DASM4SIC)
	char fff422[7] = {'$','f','f','f','4','2','2'};//DASM4 register a (DASM4A)
	char fff424[7] = {'$','f','f','f','4','2','4'};//DASM4 register b (DASM4B)
	char fff428[7] = {'$','f','f','f','4','2','8'};//PWMSM5 status/interrupt/control register (PWM5SIC)
	char fff42a[7] = {'$','f','f','f','4','2','a'};//PWMSM5 period (PWM5A)
	char fff42c[7] = {'$','f','f','f','4','2','c'};//PWMSM5 pulse width (PWM5B)
	char fff42e[7] = {'$','f','f','f','4','2','e'};//PWMSM5 counter (PWM5C)
	char fff430[7] = {'$','f','f','f','4','3','0'};//PWMSM6 status/interrupt/control register (PWM6SIC)
	char fff432[7] = {'$','f','f','f','4','3','2'};//PWMSM6 period (PWM6A)
	char fff434[7] = {'$','f','f','f','4','3','4'};//PWMSM6 pulse width (PWM6B)
	char fff436[7] = {'$','f','f','f','4','3','6'};//PWMSM6 counter (PWM6C)
	char fff438[7] = {'$','f','f','f','4','3','8'};//PWMSM7 status/interrupt/control register (PWM7SIC)
	char fff43a[7] = {'$','f','f','f','4','3','a'};//PWMSM7 period (PWM7A)
	char fff43c[7] = {'$','f','f','f','4','3','c'};//PWMSM7 pulse width (PWM7B)
	char fff43e[7] = {'$','f','f','f','4','3','e'};//PWMSM7 counter (PWM7C)
	char fff440[7] = {'$','f','f','f','4','4','0'};//PWMSM8 status/interrupt/control register (PWM8SIC)
	char fff442[7] = {'$','f','f','f','4','4','2'};//PWMSM8 period (PWM8A)
	char fff444[7] = {'$','f','f','f','4','4','4'};//PWMSM8 pulse width (PWM8B)
	char fff446[7] = {'$','f','f','f','4','4','6'};//PWMSM8 counter (PWM8C)
	char fff448[7] = {'$','f','f','f','4','4','8'};//DASM9 status/interrupt/control register (DASM9SIC)
	char fff44a[7] = {'$','f','f','f','4','4','a'};//DASM9 register a (DASM9A)
	char fff44c[7] = {'$','f','f','f','4','4','c'};//DASM9 register b (DASM9B)
	char fff450[7] = {'$','f','f','f','4','5','0'};//DASM10 status/interrupt/control register (DASM10SIC)
	char fff452[7] = {'$','f','f','f','4','5','2'};//DASM10 register a (DASM10A)
	char fff454[7] = {'$','f','f','f','4','5','4'};//DASM10 register b (DASM10B)
	char fff458[7] = {'$','f','f','f','4','5','8'};//MCSM11 status/interrupt/control register (MCSM11SIC)
	char fff45a[7] = {'$','f','f','f','4','5','a'};//MCSM11 counter (MCSM11CNT)
	char fff45c[7] = {'$','f','f','f','4','5','c'};//MCSM11 modulus latch (MCSM11ML)
	char fff460[7] = {'$','f','f','f','4','1','0'};//FCSM12 status/interrupt/control register (FCSM12SIC)
	char fff462[7] = {'$','f','f','f','4','1','2'};//FCSM12 counter (FCSM12CNT)
	//time processor unit TPU register map
	char fffe00[7] = {'$','f','f','f','e','0','0'};//Module configuration register (TPUMCR)
	char fffe02[7] = {'$','f','f','f','e','0','2'};//Test configuration register (TCR)
	char fffe04[7] = {'$','f','f','f','e','0','4'};//Development support control register (DSCR)
	char fffe06[7] = {'$','f','f','f','e','0','6'};//Development support status register (DSSR)
	char fffe08[7] = {'$','f','f','f','e','0','8'};//TPU interrupt configuratoin register (TICR)
	char fffe0a[7] = {'$','f','f','f','e','0','a'};//Channel interrupt enable register (CIER)
	char fffe0c[7] = {'$','f','f','f','e','0','c'};//Channel function selection register 0 (CFSR0)
	char fffe0e[7] = {'$','f','f','f','e','0','e'};//Channel function selection register 1 (CFSR1)
	char fffe10[7] = {'$','f','f','f','e','1','0'};//Channel function selection register 2 (CFSR2)
	char fffe12[7] = {'$','f','f','f','e','1','2'};//Channel function selection register 3 (CFSR3)
	char fffe14[7] = {'$','f','f','f','e','1','4'};//Host sequence register 0 (HSQR0)
	char fffe16[7] = {'$','f','f','f','e','1','6'};//Host sequence register 1 (HSQR1)
	char fffe18[7] = {'$','f','f','f','e','1','8'};//Host service request register 0 (HSSR0)
	char fffe1a[7] = {'$','f','f','f','e','1','a'};//Host service request register 1 (HSSR1)
	char fffe1c[7] = {'$','f','f','f','e','1','c'};//Channel priority register 0 (CPR0)
	char fffe1e[7] = {'$','f','f','f','e','1','e'};//Channel priority register 1 (CPR1)
	char fffe20[7] = {'$','f','f','f','e','2','0'};//Channel interrupt status register (CISR)
	char fffe22[7] = {'$','f','f','f','e','2','2'};//Link register (LR)
	char fffe24[7] = {'$','f','f','f','e','2','4'};//Service grant latch register (SGLR)
	char fffe26[7] = {'$','f','f','f','e','2','6'};//Decoded channel number register (DCNR)
	//TPURAM address map
	char fffb00[7] = {'$','f','f','f','b','0','0'};//TPURAM module configruation register (TRAMMCR)
	char fffb02[7] = {'$','f','f','f','b','0','2'};//TPURAM test register (TRAMTST)
	char fffb04[7] = {'$','f','f','f','b','0','4'};//TPURAM base address and status register (TRAMBAR)
	//TouCAN address map
	char fff080[7] = {'$','f','f','f','0','8','0'};//TouCAN module configuration register (CANMCR)
	char fff082[7] = {'$','f','f','f','0','8','2'};//TouCaN test configuration register (CANTCR)
	char fff084[7] = {'$','f','f','f','0','8','4'};//TouCAN interrupt register (CANICR)
	char fff086[7] = {'$','f','f','f','0','8','6'};//Control register 0 (CANCTRL0) | Control register 1 (CANCTRL1)
	char fff087[7] = {'$','f','f','f','0','8','7'};// | Control register 1 (CANCTRL1)
	char fff088[7] = {'$','f','f','f','0','8','8'};//Prescaler divider register (PRESDIV) | Control register 2 (CANCTRL2)
	char fff089[7] = {'$','f','f','f','0','8','9'};// | Control register 2 (CANCTRL2)
	char fff08a[7] = {'$','f','f','f','0','8','a'};//Free-running timer register (TIMER)
	char fff090[7] = {'$','f','f','f','0','9','0'};//Receive global mask high (RXMSKHI)
	char fff092[7] = {'$','f','f','f','0','9','2'};//Receive global mask low (RXGMSKLO)
	char fff094[7] = {'$','f','f','f','0','9','4'};//Receive buffer 14 mask high (RX14MSKHI)
	char fff096[7] = {'$','f','f','f','0','9','6'};//Receive buffer 14 mask low (RX14MSKLO)
	char fff098[7] = {'$','f','f','f','0','9','8'};//Receive buffer 15 mask high (RX15MSKHI)
	char fff09a[7] = {'$','f','f','f','0','9','a'};//Receive buffer 15 mask low (RX15MSKLO)
	char fff0a0[7] = {'$','f','f','f','0','a','0'};//Error and status register (ESTAT)
	char fff0a2[7] = {'$','f','f','f','0','a','2'};//Interrupt masks (IMASK)
	char fff0a4[7] = {'$','f','f','f','0','a','4'};//Interrupt flags (IFLAG)
	char fff0a6[7] = {'$','f','f','f','0','a','6'};//Receive error counter (RXECTR) | Transmit error counter(TXECTR)
	char fff0a7[7] = {'$','f','f','f','0','a','7'};// | Transmit error counter(TXECTR)
	//CAN message buffers from FFF100-FFF10E, FFF110-11E, FFF120-FFF12E, etc up to FFF1FF
	//where control/status bytes are the first two bytes (100-101)
	//ID high are next 2 bytes (102-103)
	//ID low are next 2 bytes (104-105)
	//next 8 bytes? are data (106-10b)
	//next 4 are reserved (checksum?) (10c-10f)
	char fff100[7] = {'$','f','f','f','1','0','0'};
	char fff102[7] = {'$','f','f','f','1','0','2'};
	char fff104[7] = {'$','f','f','f','1','0','4'};
	char fff106[7] = {'$','f','f','f','1','0','6'};
	char fff107[7] = {'$','f','f','f','1','0','7'};
	char fff108[7] = {'$','f','f','f','1','0','8'};
	char fff109[7] = {'$','f','f','f','1','0','9'};
	char fff10a[7] = {'$','f','f','f','1','0','a'};
	char fff10b[7] = {'$','f','f','f','1','0','b'};
	char fff10c[7] = {'$','f','f','f','1','0','c'};
	char fff10d[7] = {'$','f','f','f','1','0','d'};
	char fff10e[7] = {'$','f','f','f','1','0','e'};

	char fff110[7] = {'$','f','f','f','1','1','0'};
	char fff112[7] = {'$','f','f','f','1','1','2'};
	char fff114[7] = {'$','f','f','f','1','1','4'};
	char fff116[7] = {'$','f','f','f','1','1','6'};
	char fff117[7] = {'$','f','f','f','1','1','7'};
	char fff118[7] = {'$','f','f','f','1','1','8'};
	char fff119[7] = {'$','f','f','f','1','1','9'};
	char fff11a[7] = {'$','f','f','f','1','1','a'};
	char fff11b[7] = {'$','f','f','f','1','1','b'};
	char fff11c[7] = {'$','f','f','f','1','1','c'};
	char fff11d[7] = {'$','f','f','f','1','1','d'};
	char fff11e[7] = {'$','f','f','f','1','1','e'};

	char fff120[7] = {'$','f','f','f','1','2','0'};
	char fff122[7] = {'$','f','f','f','1','2','2'};
	char fff124[7] = {'$','f','f','f','1','2','4'};
	char fff126[7] = {'$','f','f','f','1','2','6'};
	char fff127[7] = {'$','f','f','f','1','2','7'};
	char fff128[7] = {'$','f','f','f','1','2','8'};
	char fff129[7] = {'$','f','f','f','1','2','9'};
	char fff12a[7] = {'$','f','f','f','1','2','a'};
	char fff12b[7] = {'$','f','f','f','1','2','b'};
	char fff12c[7] = {'$','f','f','f','1','2','c'};
	char fff12d[7] = {'$','f','f','f','1','2','d'};
	char fff12e[7] = {'$','f','f','f','1','2','e'};

	char fff130[7] = {'$','f','f','f','1','3','0'};
	char fff132[7] = {'$','f','f','f','1','3','2'};
	char fff134[7] = {'$','f','f','f','1','3','4'};
	char fff136[7] = {'$','f','f','f','1','3','6'};
	char fff137[7] = {'$','f','f','f','1','3','7'};
	char fff138[7] = {'$','f','f','f','1','3','8'};
	char fff139[7] = {'$','f','f','f','1','3','9'};
	char fff13a[7] = {'$','f','f','f','1','3','a'};
	char fff13b[7] = {'$','f','f','f','1','3','b'};
	char fff13c[7] = {'$','f','f','f','1','3','c'};
	char fff13d[7] = {'$','f','f','f','1','3','d'};
	char fff13e[7] = {'$','f','f','f','1','3','e'};

	char fff140[7] = {'$','f','f','f','1','4','0'};
	char fff142[7] = {'$','f','f','f','1','4','2'};
	char fff144[7] = {'$','f','f','f','1','4','4'};
	char fff146[7] = {'$','f','f','f','1','4','6'};
	char fff147[7] = {'$','f','f','f','1','4','7'};
	char fff148[7] = {'$','f','f','f','1','4','8'};
	char fff149[7] = {'$','f','f','f','1','4','9'};
	char fff14a[7] = {'$','f','f','f','1','4','a'};
	char fff14b[7] = {'$','f','f','f','1','4','b'};
	char fff14c[7] = {'$','f','f','f','1','4','c'};
	char fff14d[7] = {'$','f','f','f','1','4','d'};
	char fff14e[7] = {'$','f','f','f','1','4','e'};

	char fff150[7] = {'$','f','f','f','1','5','0'};
	char fff152[7] = {'$','f','f','f','1','5','2'};
	char fff154[7] = {'$','f','f','f','1','5','4'};
	char fff156[7] = {'$','f','f','f','1','5','6'};
	char fff157[7] = {'$','f','f','f','1','5','7'};
	char fff158[7] = {'$','f','f','f','1','5','8'};
	char fff159[7] = {'$','f','f','f','1','5','9'};
	char fff15a[7] = {'$','f','f','f','1','5','a'};
	char fff15b[7] = {'$','f','f','f','1','5','b'};
	char fff15c[7] = {'$','f','f','f','1','5','c'};
	char fff15d[7] = {'$','f','f','f','1','5','d'};
	char fff15e[7] = {'$','f','f','f','1','5','e'};

	char fff160[7] = {'$','f','f','f','1','6','0'};
	char fff162[7] = {'$','f','f','f','1','6','2'};
	char fff164[7] = {'$','f','f','f','1','6','4'};
	char fff166[7] = {'$','f','f','f','1','6','6'};
	char fff167[7] = {'$','f','f','f','1','6','7'};
	char fff168[7] = {'$','f','f','f','1','6','8'};
	char fff169[7] = {'$','f','f','f','1','6','9'};
	char fff16a[7] = {'$','f','f','f','1','6','a'};
	char fff16b[7] = {'$','f','f','f','1','6','b'};
	char fff16c[7] = {'$','f','f','f','1','6','c'};
	char fff16d[7] = {'$','f','f','f','1','6','d'};
	char fff16e[7] = {'$','f','f','f','1','6','e'};

	char fff170[7] = {'$','f','f','f','1','7','0'};
	char fff172[7] = {'$','f','f','f','1','7','2'};
	char fff174[7] = {'$','f','f','f','1','7','4'};
	char fff176[7] = {'$','f','f','f','1','7','6'};
	char fff177[7] = {'$','f','f','f','1','7','7'};
	char fff178[7] = {'$','f','f','f','1','7','8'};
	char fff179[7] = {'$','f','f','f','1','7','9'};
	char fff17a[7] = {'$','f','f','f','1','7','a'};
	char fff17b[7] = {'$','f','f','f','1','7','b'};
	char fff17c[7] = {'$','f','f','f','1','7','c'};
	char fff17d[7] = {'$','f','f','f','1','7','d'};
	char fff17e[7] = {'$','f','f','f','1','7','e'};

	char fff180[7] = {'$','f','f','f','1','8','0'};
	char fff182[7] = {'$','f','f','f','1','8','2'};
	char fff184[7] = {'$','f','f','f','1','8','4'};
	char fff186[7] = {'$','f','f','f','1','8','6'};
	char fff187[7] = {'$','f','f','f','1','8','7'};
	char fff188[7] = {'$','f','f','f','1','8','8'};
	char fff189[7] = {'$','f','f','f','1','8','9'};
	char fff18a[7] = {'$','f','f','f','1','8','a'};
	char fff18b[7] = {'$','f','f','f','1','8','b'};
	char fff18c[7] = {'$','f','f','f','1','8','c'};
	char fff18d[7] = {'$','f','f','f','1','8','d'};
	char fff18e[7] = {'$','f','f','f','1','8','e'};

	char fff190[7] = {'$','f','f','f','1','9','0'};
	char fff192[7] = {'$','f','f','f','1','9','2'};
	char fff194[7] = {'$','f','f','f','1','9','4'};
	char fff196[7] = {'$','f','f','f','1','9','6'};
	char fff197[7] = {'$','f','f','f','1','9','7'};
	char fff198[7] = {'$','f','f','f','1','9','8'};
	char fff199[7] = {'$','f','f','f','1','9','9'};
	char fff19a[7] = {'$','f','f','f','1','9','a'};
	char fff19b[7] = {'$','f','f','f','1','9','b'};
	char fff19c[7] = {'$','f','f','f','1','9','c'};
	char fff19d[7] = {'$','f','f','f','1','9','d'};
	char fff19e[7] = {'$','f','f','f','1','9','e'};

	char fff1a0[7] = {'$','f','f','f','1','a','0'};
	char fff1a2[7] = {'$','f','f','f','1','a','2'};
	char fff1a4[7] = {'$','f','f','f','1','a','4'};
	char fff1a6[7] = {'$','f','f','f','1','a','6'};
	char fff1a7[7] = {'$','f','f','f','1','a','7'};
	char fff1a8[7] = {'$','f','f','f','1','a','8'};
	char fff1a9[7] = {'$','f','f','f','1','a','9'};
	char fff1aa[7] = {'$','f','f','f','1','a','a'};
	char fff1ab[7] = {'$','f','f','f','1','a','b'};
	char fff1ac[7] = {'$','f','f','f','1','a','c'};
	char fff1ad[7] = {'$','f','f','f','1','a','d'};
	char fff1ae[7] = {'$','f','f','f','1','a','e'};

	char fff1b0[7] = {'$','f','f','f','1','b','0'};
	char fff1b2[7] = {'$','f','f','f','1','b','2'};
	char fff1b4[7] = {'$','f','f','f','1','b','4'};
	char fff1b6[7] = {'$','f','f','f','1','b','6'};
	char fff1b7[7] = {'$','f','f','f','1','b','7'};
	char fff1b8[7] = {'$','f','f','f','1','b','8'};
	char fff1b9[7] = {'$','f','f','f','1','b','9'};
	char fff1ba[7] = {'$','f','f','f','1','b','a'};
	char fff1bb[7] = {'$','f','f','f','1','b','b'};
	char fff1bc[7] = {'$','f','f','f','1','b','c'};
	char fff1bd[7] = {'$','f','f','f','1','b','d'};
	char fff1be[7] = {'$','f','f','f','1','b','e'};

	char fff1c0[7] = {'$','f','f','f','1','c','0'};
	char fff1c2[7] = {'$','f','f','f','1','c','2'};
	char fff1c4[7] = {'$','f','f','f','1','c','4'};
	char fff1c6[7] = {'$','f','f','f','1','c','6'};
	char fff1c7[7] = {'$','f','f','f','1','c','7'};
	char fff1c8[7] = {'$','f','f','f','1','c','8'};
	char fff1c9[7] = {'$','f','f','f','1','c','9'};
	char fff1ca[7] = {'$','f','f','f','1','c','a'};
	char fff1cb[7] = {'$','f','f','f','1','c','b'};
	char fff1cc[7] = {'$','f','f','f','1','c','c'};
	char fff1cd[7] = {'$','f','f','f','1','c','d'};
	char fff1ce[7] = {'$','f','f','f','1','c','e'};

	char fff1d0[7] = {'$','f','f','f','1','d','0'};
	char fff1d2[7] = {'$','f','f','f','1','d','2'};
	char fff1d4[7] = {'$','f','f','f','1','d','4'};
	char fff1d6[7] = {'$','f','f','f','1','d','6'};
	char fff1d7[7] = {'$','f','f','f','1','d','7'};
	char fff1d8[7] = {'$','f','f','f','1','d','8'};
	char fff1d9[7] = {'$','f','f','f','1','d','9'};
	char fff1da[7] = {'$','f','f','f','1','d','a'};
	char fff1db[7] = {'$','f','f','f','1','d','b'};
	char fff1dc[7] = {'$','f','f','f','1','d','c'};
	char fff1dd[7] = {'$','f','f','f','1','d','d'};
	char fff1de[7] = {'$','f','f','f','1','d','e'};

	char fff1e0[7] = {'$','f','f','f','1','e','0'};
	char fff1e2[7] = {'$','f','f','f','1','e','2'};
	char fff1e4[7] = {'$','f','f','f','1','e','4'};
	char fff1e6[7] = {'$','f','f','f','1','e','6'};
	char fff1e7[7] = {'$','f','f','f','1','e','7'};
	char fff1e8[7] = {'$','f','f','f','1','e','8'};
	char fff1e9[7] = {'$','f','f','f','1','e','9'};
	char fff1ea[7] = {'$','f','f','f','1','e','a'};
	char fff1eb[7] = {'$','f','f','f','1','e','b'};
	char fff1ec[7] = {'$','f','f','f','1','e','c'};
	char fff1ed[7] = {'$','f','f','f','1','e','d'};
	char fff1ee[7] = {'$','f','f','f','1','e','e'};

	char fff1f0[7] = {'$','f','f','f','1','f','0'};
	char fff1f2[7] = {'$','f','f','f','1','f','2'};
	char fff1f4[7] = {'$','f','f','f','1','f','4'};
	char fff1f6[7] = {'$','f','f','f','1','f','6'};
	char fff1f7[7] = {'$','f','f','f','1','f','7'};
	char fff1f8[7] = {'$','f','f','f','1','f','8'};
	char fff1f9[7] = {'$','f','f','f','1','f','9'};
	char fff1fa[7] = {'$','f','f','f','1','f','a'};
	char fff1fb[7] = {'$','f','f','f','1','f','b'};
	char fff1fc[7] = {'$','f','f','f','1','f','c'};
	char fff1fd[7] = {'$','f','f','f','1','f','d'};
	char fff1fe[7] = {'$','f','f','f','1','f','e'};

	//first, create the list of addresses which are interesting
	char* fffAddresses[] = {fffa00,fffa02,fffa04,fffa07,fffa08,fffa11,fffa13,fffa15,fffa17,fffa19,fffa1b,fffa1d,fffa1f,fffa21,fffa22,fffa24,fffa27,fffa30,fffa32,fffa34,fffa36,fffa38,fffa3a,fffa41,fffa44,fffa46,fffa48,fffa4a,fffa4c,fffa4e,fffa50,fffa52,fffa54,fffa56,fffa58,fffa5a,fffa5c,fffa5e,fffa60,fffa62,fffa64,fffa66,fffa68,fffa6a,fffa6c,fffa6e,fffa70,fffa72,fffa74,fffa76,fffb40,fffb42,fffb44,fffb46,fff820,fff824,fff826,fff828,fff82a,fff830,fff832,fff834,fff836,fff200,fff202,fff204,fff206,fff207,fff208,fff20a,fff20c,fff20e,fff210,fff230,fff232,fff234,fff236,fff238,fff23a,fff23c,fff23e,fff240,fff242,fff244,fff246,fff248,fff24a,fff24c,fff24e,fff250,fff252,fff254,fff256,fff258,fff25a,fff25c,fff25e,fff260,fff262,fff264,fff266,fff268,fff26a,fff26c,fff26e,fff270,fff272,fff274,fff276,fff278,fff27a,fff27c,fff27e,fff2b0,fff2b2,fff2b4,fff2b6,fff2b8,fff2ba,fff2bc,fff2be,fff2c0,fff2c2,fff2c4,fff2c6,fff2c8,fff2ca,fff2cc,fff2ce,fff2d0,fff2d2,fff2d4,fff2d6,fff2d8,fff2da,fff2dc,fff2de,fff2e0,fff2e2,fff2e4,fff2e6,fff2e8,fff2ea,fff2ec,fff2ee,fff2f0,fff2f2,fff2f4,fff2f6,fff2f8,fff2fa,fff2fc,fff2fe,fff330,fff332,fff334,fff336,fff338,fff33a,fff33c,fff33e,fff340,fff342,fff344,fff346,fff348,fff34a,fff34c,fff34e,fff350,fff352,fff354,fff356,fff358,fff35a,fff35c,fff35e,fff360,fff362,fff364,fff366,fff368,fff36a,fff36c,fff36e,fff370,fff372,fff374,fff376,fff378,fff37a,fff37c,fff37e,fff3b0,fff3b2,fff3b4,fff3b6,fff3b8,fff3ba,fff3bc,fff3be,fff3c0,fff3c2,fff3c4,fff3c6,fff3c8,fff3ca,fff3cc,fff3ce,fff3d0,fff3d2,fff3d4,fff3d6,fff3d8,fff3da,fff3dc,fff3de,fff3e0,fff3e2,fff3e4,fff3e6,fff3e8,fff3ea,fff3ec,fff3ee,fff3f0,fff3f2,fff3f4,fff3f6,fff3f8,fff3fa,fff3fc,fff3fe,fffc00,fffc02,fffc04,fffc05,fffc08,fffc0a,fffc0c,fffc0e,fffc15,fffc16,fffc17,fffc18,fffc1a,fffc1c,fffc1e,fffc1f,fffd00,fffd02,fffd04,fffd06,fffd08,fffd0a,fffd0c,fffd0e,fffd10,fffd12,fffd14,fffd16,fffd18,fffd1a,fffd1c,fffd1e,fffd20,fffd22,fffd24,fffd26,fffd28,fffd2a,fffd2c,fffd2e,fffd30,fffd32,fffd34,fffd36,fffd38,fffd3a,fffd3c,fffd3e,fffd40,fffd42,fffd44,fffd46,fffd48,fffd4a,fffd4c,fffd4e,fffd50,fffd52,fffd54,fffd56,fffd58,fffd5a,fffd5c,fffd5e,fff400,fff402,fff404,fff408,fff40a,fff410,fff412,fff414,fff418,fff41a,fff41c,fff420,fff422,fff424,fff428,fff42a,fff42c,fff42e,fff430,fff432,fff434,fff436,fff438,fff43a,fff43c,fff43e,fff440,fff442,fff444,fff446,fff448,fff44a,fff44c,fff450,fff452,fff454,fff458,fff45a,fff45c,fff460,fff462,fffe00,fffe02,fffe04,fffe06,fffe08,fffe0a,fffe0c,fffe0e,fffe10,fffe12,fffe14,fffe16,fffe18,fffe1a,fffe1c,fffe1e,fffe20,fffe22,fffe24,fffe26,fffb00,fffb02,fffb04,fff080,fff082,fff084,fff086,fff087,fff088,fff089,fff08a,fff090,fff092,fff094,fff096,fff098,fff09a,fff0a0,fff0a2,fff0a4,fff0a6,fff0a7,fff100,fff102,fff104,fff106,fff107,fff108,fff109,fff10a,fff10b,fff10c,fff10d,fff10e,fff110,fff112,fff114,fff116,fff117,fff118,fff119,fff11a,fff11b,fff11c,fff11d,fff11e,fff120,fff122,fff124,fff126,fff127,fff128,fff129,fff12a,fff12b,fff12c,fff12d,fff12e,fff130,fff132,fff134,fff136,fff137,fff138,fff139,fff13a,fff13b,fff13c,fff13d,fff13e,fff140,fff142,fff144,fff146,fff147,fff148,fff149,fff14a,fff14b,fff14c,fff14d,fff14e,fff150,fff152,fff154,fff156,fff157,fff158,fff159,fff15a,fff15b,fff15c,fff15d,fff15e,fff160,fff162,fff164,fff166,fff167,fff168,fff169,fff16a,fff16b,fff16c,fff16d,fff16e,fff170,fff172,fff174,fff176,fff177,fff178,fff179,fff17a,fff17b,fff17c,fff17d,fff17e,fff180,fff182,fff184,fff186,fff187,fff188,fff189,fff18a,fff18b,fff18c,fff18d,fff18e,fff190,fff192,fff194,fff196,fff197,fff198,fff199,fff19a,fff19b,fff19c,fff19d,fff19e,fff1a0,fff1a2,fff1a4,fff1a6,fff1a7,fff1a8,fff1a9,fff1aa,fff1ab,fff1ac,fff1ad,fff1ae,fff1b0,fff1b2,fff1b4,fff1b6,fff1b7,fff1b8,fff1b9,fff1ba,fff1bb,fff1bc,fff1bd,fff1be,fff1c0,fff1c2,fff1c4,fff1c6,fff1c7,fff1c8,fff1c9,fff1ca,fff1cb,fff1cc,fff1cd,fff1ce,fff1d0,fff1d2,fff1d4,fff1d6,fff1d7,fff1d8,fff1d9,fff1da,fff1db,fff1dc,fff1dd,fff1de,fff1e0,fff1e2,fff1e4,fff1e6,fff1e7,fff1e8,fff1e9,fff1ea,fff1eb,fff1ec,fff1ed,fff1ee,fff1f0,fff1f2,fff1f4,fff1f6,fff1f7,fff1f8,fff1f9,fff1fa,fff1fb,fff1fc,fff1fd,fff1fe};


	//second, create the list of identifiers which correspond to the previously ennumerated addresses
	string fffAddressesIdentifiers[] = {"SIM module configuration register (SIMCR)","SIM test register (SIMTR)","Clock synthesizer control register (SYNCR)","Reset status register (RSR)","SIM test register E (SIMTRE)","Port E data (PORTE0)","Port E data (PORTE1)","Port E data direction (DDRE)","Port E pin assignment (PEPAR)","Port F data (PORTF0)","Port F data (PORTF1)","Port F data direction (DDRF)","Port F pin assignment (PFPAR)","System protection control (SYPCR)","Periodic interrupt control register (PICR)","Periodic interrupt timing register (PITR)","Software service (SWSR)","Test module master shift A (TSTMSRA)","Test module master shift B (TSTMSRB)","Test module shift count (TSTSC)","Test module repetition counter (TSTRC)","Test module control (CREG)","Test module distributed (DREG)","Port C data (PORTC)","Chip select pin assignment (CSPAR0)","Chip select pin assignment (CSPAR1)","Chip select base boot (CSBARBT)","Chip select option boot (CSORBT)","Chip select base 0 (CSBAR0)","Chip select option 0 (CSOR0)","Chip select base 1 (CSBAR1)","Chip select option 1 (CSOR1)","Chip select base 2 (CSBAR2)","Chip select option 2 (CSOR2)","Chip select base 3 (CSBAR3)","Chip select option 3 (CSOR3)","Chip select base 4 (CSBAR4)","Chip select option 4 (CSOR4)","Chip select base 5 (CSBAR5)","Chip select option 5 (CSOR5)","Chip select base 6 (CSBAR6)","Chip select option 6 (CSOR6)","Chip select base 7 (CSBAR7)","Chip select option 7 (CSOR7)","Chip select base 8 (CSBAR8)","Chip select option 8 (CSOR8)","Chip select base 9 (CSBAR9)","Chip select option 9 (CSOR9)","Chip select base 10 (CSBAR10)","Chip select option 10 (CSOR10)","RAM module configuration register (RAMMCR)","RAM test register (RAMTST)","RAM array base address register high (RAMBAH)","RAM array base address register low (RAMBAL)","Masked ROM module configuration register (MRMCR)","ROM array base address high register (ROMBAH)","ROM array base address low register (ROMBAL)","Signature high register (SIGHI)","Signiture low register (SIGLO)","ROM boostrap word 0 (ROMBS0)","ROM boostrap word 1 (ROMBS1)","ROM boostrap word 2 (ROMBS2)","ROM boostrap word 3 (ROMBS3)","Module configuration register (QADCMCR)","Test register (QADCTEST)","Interrupt register (QADCINT)","Port A data (PORTQA)","PORT B data (PORTQB)","Port data direction register (DDRQA)","Control register 0 (QACR0)","Control register 1 (QACR1)","Control register 2 (QACR2)","Status register (QASR)","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Conversion command word (CCW) table","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (RRJURR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJSRR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","Result word table (LJURR)","QSM module configuration register (QSMCR)","QSM test register (QSMTEST)","QSM interrupt level register (QILR)","QSM interrupt vector registration (QIVR)","SCI control 0 register (SCCR0)","SCI control 1 register (SCCR1)","SCI status register (SCSR)","SCI data register (SCDR)","PQS data register (PORTQS)","PQS pin assignment register (PQSPAR)","PQS data direction register (DDRQS)","SPI control register 0 (SPCR0)","SPI control register 1 (SPCR1)","SPI control register 2 (SPCR2)","SPI control register 3 (SPCR3)","SPI status register (SPSR)","Receive RAM (RR[0:F])","Receive RAM (RR[0:F])","Receive RAM (RR[0:F])","Receive RAM (RR[0:F])","Receive RAM (RR[0:F])","Receive RAM (RR[0:F])","Receive RAM (RR[0:F])","Receive RAM (RR[0:F])","Receive RAM (RR[0:F])","Receive RAM (RR[0:F])","Receive RAM (RR[0:F])","Receive RAM (RR[0:F])","Receive RAM (RR[0:F])","Receive RAM (RR[0:F])","Receive RAM (RR[0:F])","Receive RAM (RR[0:F])","Transmit RAM (TR[0:F])","Transmit RAM (TR[0:F])","Transmit RAM (TR[0:F])","Transmit RAM (TR[0:F])","Transmit RAM (TR[0:F])","Transmit RAM (TR[0:F])","Transmit RAM (TR[0:F])","Transmit RAM (TR[0:F])","Transmit RAM (TR[0:F])","Transmit RAM (TR[0:F])","Transmit RAM (TR[0:F])","Transmit RAM (TR[0:F])","Transmit RAM (TR[0:F])","Transmit RAM (TR[0:F])","Transmit RAM (TR[0:F])","Transmit RAM (TR[0:F])","Command RAM (CR[0:F])","Command RAM (CR[0:F])","Command RAM (CR[0:F])","Command RAM (CR[0:F])","Command RAM (CR[0:F])","Command RAM (CR[0:F])","Command RAM (CR[0:F])","Command RAM (CR[0:F])","Command RAM (CR[0:F])","Command RAM (CR[0:F])","Command RAM (CR[0:F])","Command RAM (CR[0:F])","Command RAM (CR[0:F])","Command RAM (CR[0:F])","Command RAM (CR[0:F])","Command RAM (CR[0:F])","BIUSM module configuration register (BIUMCR)","BIUSM test register (BIUTEST)","BIUSM time base register (BIUTBR)","CPSM control register (CPCR)","CPSM test register (CPTR)","MCSM2 status/interrupt/control register (MCSM2SIC)","MCSM2 counter (MCSM2CNT)","MCSM2 modulus latch (MCSM2ML)","DASM3 status/interrupt/control register (DASM3SIC)","DASM3 register a (DASM3A)","DASM3 register b (DASM3B)","DASM4 status/interrupt/control register (DASM4SIC)","DASM4 register a (DASM4A)","DASM4 register b (DASM4B)","PWMSM5 status/interrupt/control register (PWM5SIC)","PWMSM5 period (PWM5A)","PWMSM5 pulse width (PWM5B)","PWMSM5 counter (PWM5C)","PWMSM6 status/interrupt/control register (PWM6SIC)","PWMSM6 period (PWM6A)","PWMSM6 pulse width (PWM6B)","PWMSM6 counter (PWM6C)","PWMSM7 status/interrupt/control register (PWM7SIC)","PWMSM7 period (PWM7A)","PWMSM7 pulse width (PWM7B)","PWMSM7 counter (PWM7C)","PWMSM8 status/interrupt/control register (PWM8SIC)","PWMSM8 period (PWM8A)","PWMSM8 pulse width (PWM8B)","PWMSM8 counter (PWM8C)","DASM9 status/interrupt/control register (DASM9SIC)","DASM9 register a (DASM9A)","DASM9 register b (DASM9B)","DASM10 status/interrupt/control register (DASM10SIC)","DASM10 register a (DASM10A)","DASM10 register b (DASM10B)","MCSM11 status/interrupt/control register (MCSM11SIC)","MCSM11 counter (MCSM11CNT)","MCSM11 modulus latch (MCSM11ML)","FCSM12 status/interrupt/control register (FCSM12SIC)","FCSM12 counter (FCSM12CNT)","Module configuration register (TPUMCR)","Test configuration register (TCR)","Development support control register (DSCR)","Development support status register (DSSR)","TPU interrupt configuratoin register (TICR)","Channel interrupt enable register (CIER)","Channel function selection register 0 (CFSR0)","Channel function selection register 1 (CFSR1)","Channel function selection register 2 (CFSR2)","Channel function selection register 3 (CFSR3)","Host sequence register 0 (HSQR0)","Host sequence register 1 (HSQR1)","Host service request register 0 (HSSR0)","Host service request register 1 (HSSR1)","Channel priority register 0 (CPR0)","Channel priority register 1 (CPR1)","Channel interrupt status register (CISR)","Link register (LR)","Service grant latch register (SGLR)","Decoded channel number register (DCNR)","TPURAM module configruation register (TRAMMCR)","TPURAM test register (TRAMTST)","TPURAM base address and status register (TRAMBAR)","TouCAN module configuration register (CANMCR)","TouCaN test configuration register (CANTCR)","TouCAN interrupt register (CANICR)","Control register 0 (CANCTRL0)","Control register 1 (CANCTRL1)","Prescaler divider register (PRESDIV)","Control register 2 (CANCTRL2)","Free-running timer register (TIMER)","Receive global mask high (RXMSKHI)","Receive global mask low (RXGMSKLO)","Receive buffer 14 mask high (RX14MSKHI)","Receive buffer 14 mask low (RX14MSKLO)","Receive buffer 15 mask high (RX15MSKHI)","Receive buffer 15 mask low (RX15MSKLO)","Error and status register (ESTAT)","Interrupt masks (IMASK)","Interrupt flags (IFLAG)","Receive error counter (RXECTR)","Transmit error counter(TXECTR)","CTRL_Status CAN_mes_buf_0","ID high CAN_mes_buf_0","ID low CAN_mes_buf_0","Byte0 CAN_mes_buf_0","Byte1 CAN_mes_buf_0","Byte2 CAN_mes_buf_0","Byte3 CAN_mes_buf_0","Byte4 CAN_mes_buf_0","Byte5 CAN_mes_buf_0","Byte6 CAN_mes_buf_0","byte7 CAN_mes_buf_0","CheckSum CAN_mes_buf_0","CTRL_Status CAN_mes_buf_1","ID high CAN_mes_buf_1","ID low CAN_mes_buf_1","Byte0 CAN_mes_buf_1","Byte1 CAN_mes_buf_1","Byte2 CAN_mes_buf_1","Byte3 CAN_mes_buf_1","Byte4 CAN_mes_buf_1","Byte5 CAN_mes_buf_1","Byte6 CAN_mes_buf_1","byte7 CAN_mes_buf_1","CheckSum CAN_mes_buf_1","CTRL_Status CAN_mes_buf_2","ID high CAN_mes_buf_2","ID low CAN_mes_buf_2","Byte0 CAN_mes_buf_2","Byte1 CAN_mes_buf_2","Byte2 CAN_mes_buf_2","Byte3 CAN_mes_buf_2","Byte4 CAN_mes_buf_2","Byte5 CAN_mes_buf_2","Byte6 CAN_mes_buf_2","byte7 CAN_mes_buf_2","CheckSum CAN_mes_buf_2","CTRL_Status CAN_mes_buf_3","ID high CAN_mes_buf_3","ID low CAN_mes_buf_3","Byte0 CAN_mes_buf_3","Byte1 CAN_mes_buf_3","Byte2 CAN_mes_buf_3","Byte3 CAN_mes_buf_3","Byte4 CAN_mes_buf_3","Byte5 CAN_mes_buf_3","Byte6 CAN_mes_buf_3","byte7 CAN_mes_buf_3","CheckSum CAN_mes_buf_3","CTRL_Status CAN_mes_buf_4","ID high CAN_mes_buf_4","ID low CAN_mes_buf_4","Byte0 CAN_mes_buf_4","Byte1 CAN_mes_buf_4","Byte2 CAN_mes_buf_4","Byte3 CAN_mes_buf_4","Byte4 CAN_mes_buf_4","Byte5 CAN_mes_buf_4","Byte6 CAN_mes_buf_4","byte7 CAN_mes_buf_4","CheckSum CAN_mes_buf_4","CTRL_Status CAN_mes_buf_5","ID high CAN_mes_buf_5","ID low CAN_mes_buf_5","Byte0 CAN_mes_buf_5","Byte1 CAN_mes_buf_5","Byte2 CAN_mes_buf_5","Byte3 CAN_mes_buf_5","Byte4 CAN_mes_buf_5","Byte5 CAN_mes_buf_5","Byte6 CAN_mes_buf_5","byte7 CAN_mes_buf_5","CheckSum CAN_mes_buf_5","CTRL_Status CAN_mes_buf_6","ID high CAN_mes_buf_6","ID low CAN_mes_buf_6","Byte0 CAN_mes_buf_6","Byte1 CAN_mes_buf_6","Byte2 CAN_mes_buf_6","Byte3 CAN_mes_buf_6","Byte4 CAN_mes_buf_6","Byte5 CAN_mes_buf_6","Byte6 CAN_mes_buf_6","byte7 CAN_mes_buf_6","CheckSum CAN_mes_buf_6","CTRL_Status CAN_mes_buf_7","ID high CAN_mes_buf_7","ID low CAN_mes_buf_7","Byte0 CAN_mes_buf_7","Byte1 CAN_mes_buf_7","Byte2 CAN_mes_buf_7","Byte3 CAN_mes_buf_7","Byte4 CAN_mes_buf_7","Byte5 CAN_mes_buf_7","Byte6 CAN_mes_buf_7","byte7 CAN_mes_buf_7","CheckSum CAN_mes_buf_7","CTRL_Status CAN_mes_buf_8","ID high CAN_mes_buf_8","ID low CAN_mes_buf_8","Byte0 CAN_mes_buf_8","Byte1 CAN_mes_buf_8","Byte2 CAN_mes_buf_8","Byte3 CAN_mes_buf_8","Byte4 CAN_mes_buf_8","Byte5 CAN_mes_buf_8","Byte6 CAN_mes_buf_8","byte7 CAN_mes_buf_8","CheckSum CAN_mes_buf_8","CTRL_Status CAN_mes_buf_9","ID high CAN_mes_buf_9","ID low CAN_mes_buf_9","Byte0 CAN_mes_buf_9","Byte1 CAN_mes_buf_9","Byte2 CAN_mes_buf_9","Byte3 CAN_mes_buf_9","Byte4 CAN_mes_buf_9","Byte5 CAN_mes_buf_9","Byte6 CAN_mes_buf_9","byte7 CAN_mes_buf_9","CheckSum CAN_mes_buf_9","CTRL_Status CAN_mes_buf_10","ID high CAN_mes_buf_10","ID low CAN_mes_buf_10","Byte0 CAN_mes_buf_10","Byte1 CAN_mes_buf_10","Byte2 CAN_mes_buf_10","Byte3 CAN_mes_buf_10","Byte4 CAN_mes_buf_10","Byte5 CAN_mes_buf_10","Byte6 CAN_mes_buf_10","byte7 CAN_mes_buf_10","CheckSum CAN_mes_buf_10","CTRL_Status CAN_mes_buf_11","ID high CAN_mes_buf_11","ID low CAN_mes_buf_11","Byte0 CAN_mes_buf_11","Byte1 CAN_mes_buf_11","Byte2 CAN_mes_buf_11","Byte3 CAN_mes_buf_11","Byte4 CAN_mes_buf_11","Byte5 CAN_mes_buf_11","Byte6 CAN_mes_buf_11","byte7 CAN_mes_buf_11","CheckSum CAN_mes_buf_11","CTRL_Status CAN_mes_buf_12","ID high CAN_mes_buf_12","ID low CAN_mes_buf_12","Byte0 CAN_mes_buf_12","Byte1 CAN_mes_buf_12","Byte2 CAN_mes_buf_12","Byte3 CAN_mes_buf_12","Byte4 CAN_mes_buf_12","Byte5 CAN_mes_buf_12","Byte6 CAN_mes_buf_12","byte7 CAN_mes_buf_12","CheckSum CAN_mes_buf_12","CTRL_Status CAN_mes_buf_13","ID high CAN_mes_buf_13","ID low CAN_mes_buf_13","Byte0 CAN_mes_buf_13","Byte1 CAN_mes_buf_13","Byte2 CAN_mes_buf_13","Byte3 CAN_mes_buf_13","Byte4 CAN_mes_buf_13","Byte5 CAN_mes_buf_13","Byte6 CAN_mes_buf_13","byte7 CAN_mes_buf_13","CheckSum CAN_mes_buf_13","CTRL_Status CAN_mes_buf_14","ID high CAN_mes_buf_14","ID low CAN_mes_buf_14","Byte0 CAN_mes_buf_14","Byte1 CAN_mes_buf_14","Byte2 CAN_mes_buf_14","Byte3 CAN_mes_buf_14","Byte4 CAN_mes_buf_14","Byte5 CAN_mes_buf_14","Byte6 CAN_mes_buf_14","byte7 CAN_mes_buf_14","CheckSum CAN_mes_buf_14","CTRL_Status CAN_mes_buf_15","ID high CAN_mes_buf_15","ID low CAN_mes_buf_15","Byte0 CAN_mes_buf_15","Byte1 CAN_mes_buf_15","Byte2 CAN_mes_buf_15","Byte3 CAN_mes_buf_15","Byte4 CAN_mes_buf_15","Byte5 CAN_mes_buf_15","Byte6 CAN_mes_buf_15","byte7 CAN_mes_buf_15","CheckSum CAN_mes_buf_15"};

	//next, open the file and read in a line, preparing to loop through the entire file while doing this
	//also open a file which can have the output pushed into

	string line = "";
	if (myfile.is_open())
	{
		while (getline (myfile,line))
		{
			//search the line for an address
			string addressInLine = "";
			addressInLine = extractAddress(line);
			int sizeOfAddress = addressInLine.length();

			//if an address is found, loop through the list of interesting addresses to search for a match
			string replacementString = "";
			bool matchFound = false;
			//reWrite << addressInLine << '\n';
			if(sizeOfAddress > 0)
			{
				for(int a = 0; a < 572; a++)
				{
					string stringCompare = "";
					for(int r = 0; r < 7; r++)
					{
						stringCompare.push_back(fffAddresses[a][r]);
					}
					if(addressInLine == stringCompare)
					{
						//create the replacement string
						replacementString = addressInLine.substr(0,7);
						replacementString.push_back('_');
						replacementString.append(fffAddressesIdentifiers[a]);
						//reWrite << replaceString << '\n';
						matchFound = true;
						break;
					}
				}

			}

			//if a match is found, replace the original address in the string with the identifier
			if(matchFound == true)
			{
				//cout <<addressInLine << '\t' << replaceString << '\n';
				line = replaceString(line, addressInLine, replacementString);
			}
			//output the updated string into the output file
			reWrite << line << '\n';
		}
	}


	myfile.close();
	reWrite.close();
	return 0;
}

string replaceString(string original, string searchTerm, string replaceTerm)
{
	//searches original for searchTerm, if found will replace with replaceTerm
	//otherwise, returns original
	string editedString = "";

	int stringLength = original.length();
	int searchTermLength = searchTerm.length();

	//cout << "looking for: " << searchTerm << ", size: " << searchTermLength << '\n';

	//loop through the string, searching for "searchTerm"
	int searchIndex = 0;
	int matchingChars = 0;
	int matchPosition = 0;
	bool matchFound = false;
	for(int i = 0; i < stringLength; i++)
	{
		if(original[i] == searchTerm[searchIndex])
		{
			//one character of the string matches our search
			matchingChars += 1;
			searchIndex += 1;//look to see if the next character matches

			//cout << "matched chars: " << matchingChars << '\n';
		}
		else
		{
			searchIndex = 0;
			matchingChars = 0;
		}

		//if we make our first match, then mark the position
		if(matchingChars == 1)
		{
			matchPosition = i;
			//cout << "match found at position: " << matchPosition << '\n';
			//cout << "match begins with: " << original[i] << '\n';
		}

		if(matchingChars == searchTermLength)
		{
			matchFound = true;
			break;//end the for loop
		}
	}

	//if we have a match, then break up the original string and add in the replacement string instead of the original
	if(matchFound == true)
	{
		editedString = original.substr(0, matchPosition);

		//cout << "beginning of orig.: " << editedString << '\n';

		editedString.append(replaceTerm);

		//cout << "add in replaceTerm: " << editedString << '\n';


		//make sure to search the remaining part of the string for the searchTerm
		int positionEndOfSearchTermInOriginalString = matchPosition + searchTermLength;
		string restOfString = original.substr(positionEndOfSearchTermInOriginalString, stringLength - positionEndOfSearchTermInOriginalString);
		string searchRestOfString = replaceString(restOfString, searchTerm, replaceTerm);
		editedString.append(searchRestOfString);
	}
	else
	{
		//if we have no match, output the original string
		editedString = original;
	}
	return editedString;

	/*
	string phrase = "0x00045c:	MOVE.W ($fffa04).L,D0	; source -> destination	1116 + 6	001100000011100100000000111111111111101000000100";
	string phraseSearch = replaceString(phrase, "$fffa04", "$fffa04_clock_synthesizer_control_register_SYNCR");

	cout << phrase << '\n';
	cout << phraseSearch << '\n';
	 */
}

string extractAddress(string original)
{
	//extracts an address as indicated by a hex preceded by '($' and terminated by ')'
	string address = "";
	int originalStringLength = original.length();
	int addrStartPosition = -1;
	//loop through the string, search for "($" and ended by ')'
	for(int i = 0; i < originalStringLength; i++)
	{
		if(original[i] == '(' && original[i + 1] == '$')
		{
			addrStartPosition = i + 1;
		}

		if(addrStartPosition >= 0 && original[i] != ')' && original[i] != '(')
		{
			address.push_back(original[i]);
		}
		else if(original[i] == ')')
		{
			break;//end the for loop, we've collected the address
		}
	}
	return address;

	/*
	string phrase = "0x00045c:	MOVE.W ($fffa04).L,D0	; source -> destination	1116 + 6	001100000011100100000000111111111111101000000100";
	string addr = extractAddress(phrase);

	cout << addr << '\n';
	 */
}
