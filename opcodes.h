// http://devernay.free.fr/hacks/chip8/C8TECH10.HTM

#define CLS 0x00E0
#define RET 0x00EE
#define JP 0x1000
#define CALL 0x2000
#define SE_Vx_byte 0x3000
#define SNE_Vx_byte 0x4000
#define SE_Vx_Vy 0x5000
#define LD_Vx_byte 0x6000
#define ADD_Vx 0x7000

// 0x8000
#define LD_Vx_Vy 0x0000
#define OR_Vx_Vy 0x0001
#define AND_Vx_Vy 0x0002
#define XOR_Vx_Vy 0x0003
#define ADD_Vx_Vy 0x0004
#define SUB_Vx_Vy 0x0005
#define SHR_Vx_Vy 0x0006
#define SUBN_Vx_Vy 0x0007
#define SHL_Vx_Vy 0x000E

#define SNE_Vx_Vy 0x9000
#define LD_I 0xA000
#define JP_V0_addr 0xB000
#define RND_Vx_byte 0xC000
#define DRW 0xD000

// 0xE000
#define SKP_Vx 0x009E
#define SKNP_Vx 0x00A1

// 0xF000
#define LD_Vx_DT 0x0007
#define LD_Vx_K 0x000A
#define LD_DT_Vx 0x0015
#define LD_ST_Vx 0x0018
#define ADD_I_Vx 0x001E
#define LD_F_Vx 0x0029
#define LD_B_Vx 0x0033
#define LD_I_Vx 0x0055
#define LD_Vx_I 0x0065