#ifndef _OP_CODES_H_
#define _OP_CODES_H_

#define DESTINATION(x)	(x >> 3 & 7)
#define SOURCE(x)		(x & 7)
#define CONDITION(x)	(x >> 3 & 7)
#define VECTOR(x)		(x >> 3 & 7)
#define RP(x)			(x >> 4 & 3)

#define REGISTER_A		7
#define REGISTER_B		0
#define REGISTER_C		1
#define REGISTER_D		2
#define REGISTER_E		3
#define REGISTER_H		4
#define REGISTER_L		5
#define MEMORY_ACCESS	6

#define PAIR_BC			0
#define PAIR_DE			1
#define PAIR_HL			2
#define PAIR_SP			3

#define CONDITION_NZ	0
#define CONDITION_Z		1
#define CONDITION_NC	2
#define CONDITION_C		3
#define CONDITION_PO	4
#define CONDITION_PE	5
#define CONDITION_P		6
#define CONDITION_M		7

#define MASK_JMP		0xff
#define OP_JMP			0xc3
#define CYCLES_JMP		10

#define MASK_NOP		0xff
#define OP_NOP			0x00
#define CYCLES_NOP		4

#define MASK_MOV		0xC0
#define OP_MOV			0x40
#define CYCLES_MOV_REG	5
#define CYCLES_MOV_MEM	7

#define MASK_MVI		0xC7
#define OP_MVI			0x06
#define CYCLES_MVI_REG	7
#define CYCLES_MVI_MEM	10

#define MASK_LXI		0xCF
#define OP_LXI			1
#define CYCLES_LXI		10

#define MASK_LDA		0xFF
#define OP_LDA			0x3A
#define CYCLES_LDA		13

#define MASK_STA		0xFF
#define OP_STA			0x32
#define CYCLES_STA		13

#define MASK_LHLD		0xFF
#define OP_LHLD			0x2A
#define CYCLES_LHLD		16

#define MASK_SHLD		0xFF
#define OP_SHLD			0x22
#define CYCLES_SHLD		16

#define MASK_LDAX		0xCF
#define OP_LDAX			0x0A
#define CYCLES_LDAX		7

#define MASK_STAX		0xCF
#define OP_STAX			0x02
#define CYCLES_STAX		7

#define MASK_XCHG		0xFF
#define OP_XCHG			0xEB
#define CYCLES_XCHG		5

#define MASK_ADD		0xF8
#define OP_ADD			0x80
#define CYCLES_ADD		4

#define MASK_ADI		0xFF
#define OP_ADI			0xC6
#define CYCLES_ADI		7

#define MASK_ADC		0xF8
#define OP_ADC			0x88
#define CYCLES_ADC		4

#define MASK_ACI		0xFF
#define OP_ACI			0xCE
#define CYCLES_ACI		7

#define MASK_SUB		0xF8
#define OP_SUB			0x90
#define CYCLES_SUB		4

#define MASK_SUI		0xFF
#define OP_SUI			0xD6
#define CYCLES_SUI		7

#define MASK_SBB		0xF8
#define OP_SBB			0x98
#define CYCLES_SBB		4

#define MASK_SBI		0xFF
#define OP_SBI			0xDE
#define CYCLES_SBI		7

#define MASK_INR		0xC7
#define OP_INR			0x04
#define CYCLES_INR		5

#define MASK_DCR		0xC7
#define OP_DCR			0x05
#define CYCLES_DCR		5

#define MASK_INX		0xCF
#define OP_INX			0x03
#define CYCLES_INX		5

#define MASK_DCX		0xCF
#define OP_DCX			0x0B
#define CYCLES_DCX		5

#define MASK_DAD		0xCF
#define OP_DAD			0x09
#define CYCLES_DAD		10

#define MASK_ANA		0xF8
#define OP_ANA			0xA0
#define CYCLES_ANA		4

#define MASK_ANI		0xFF
#define OP_ANI			0xE6
#define CYCLES_ANI		7

#define MASK_ORA		0xF8
#define OP_ORA			0xB0
#define CYCLES_ORA		4

#define MASK_ORI		0xFF
#define OP_ORI			0xF6
#define CYCLES_ORI		7

#define MASK_XRA		0xF8
#define OP_XRA			0xA8
#define CYCLES_XRA		4

#define MASK_XRI		0xFF
#define OP_XRI			0xEE
#define CYCLES_XRI		7

#define MASK_EI			0xFF
#define OP_EI			0xFB
#define CYCLES_EI		4

#define MASK_DI			0xFF
#define OP_DI			0xF3
#define CYCLES_DI		4

#define MASK_XTHL		0xFF
#define OP_XTHL			0xE3
#define CYCLES_XTHL		18

#define MASK_SPHL		0xFF
#define OP_SPHL			0xF9
#define CYCLES_SPHL		5

#define MASK_IN			0xFF
#define OP_IN			0xDB
#define CYCLES_IN		10

#define MASK_OUT		0xFF
#define OP_OUT			0xD3
#define CYCLES_OUT		10

#define MASK_PUSH		0xCF
#define OP_PUSH			0xC5
#define CYCLES_PUSH		11

#define MASK_POP		0xCF
#define OP_POP			0xC1
#define CYCLES_POP		10


#define MASK_RLC		0xFF
#define OP_RLC			0x07
#define CYCLES_RLC		4

#define MASK_RRC		0xFF
#define OP_RRC			0x0F
#define CYCLES_RRC		4

#define MASK_RAL		0xFF
#define OP_RAL			0x17
#define CYCLES_RAL		4

#define MASK_RAR		0xFF
#define OP_RAR			0x1F
#define CYCLES_RAR		4

#define MASK_JCCC		0xC7
#define OP_JCCC			0xC2

#define MASK_RET		0xFF
#define OP_RET			0xC9
#define CYCLES_RET		5

#define MASK_RCCC		0xC7
#define OP_RCCC			0xC0

#define MASK_CALL		0xFF
#define OP_CALL			0xCD
#define CYCLES_CALL		17

#define MASK_RST		0xC7
#define OP_RST			0xC7
#define CYCLES_RST		11

#define MASK_CMP		0xF8
#define OP_CMP			0xB8
#define CYCLES_CMP		4

#define MASK_CPI		0xFF
#define OP_CPI			0xFE
#define CYCLES_CPI		7

#define MASK_CCCC		0xC7
#define OP_CCCC			0xC4

#define MASK_STC		0xFF
#define OP_STC			0x37
#define CYCLES_STC		1

#define MASK_CMC		0xFF
#define OP_CMC			0x3f
#define CYCLES_CMC		2

#define MASK_CMA		0xFF
#define OP_CMA			0x2F
#define CYCLES_CMA		2

#define MASK_PCHL		0xFF
#define OP_PCHL			0xe9
#define CYCLES_PCHL		5

#define MASK_DAA		0xFF
#define OP_DAA			0x27
#define CYCLES_DAA		5

#define ISOP(OP, x)		((x & MASK_ ## OP) == OP_ ## OP)

#endif