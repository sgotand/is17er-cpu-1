#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "include.h"
#include "other_function.h"
//浮動小数点のstoreかloadがおかしい
typedef union _word word;
union _word {
  float f;
  unsigned int x;
  int d;
  uint8_t u8;
};

typedef union {
  int i;
  float f;
  uint32_t u;
  uint8_t b[4];
} fi_union;
int input_index=0;
extern fi_union sld_words[];
extern uint8_t sld_bytes[];
extern unsigned sld_n_bytes;
word w;


int instr_exec(Machine mac,instruction instr){	
	int opcode,funct3,funct7;
	int rd,rs1,rs2,imm,shamt;
	unsigned int u1,u2;//unsignedに変換するときに用いる

	opcode = bintonm(instr,0,6);
	funct3 = bintonm(instr,12,14);
	funct7 = bintonm(instr,25,31);
	rd = bintonm(instr,7,11);
	rs1 = bintonm(instr,15,19);
	rs2 = bintonm(instr,20,24);
	shamt = bintonm(instr,20,24);
	imm = immtonm(instr,opcode);

	switch(opcode){
		case OP_LUI:
			mac->x[rd]=imm;//"上位"に注意！
			mac->pc=nextpc(mac->pc);
		break;
		case OP_AUIPC://なぜかmac->pcはプログラムカウンタの1/4であることに注意しなくて良さそう
			mac->x[rd]=mac->pc + imm;//"上位"に注意
			mac->pc=nextpc(mac->pc);
		break;
		case OP_JAL://なぜかmac->pcはプログラムカウンタの1/4であることに注意しなくて良さそう
			mac->x[rd]=nextpc(mac->pc);
			mac->pc=mac->pc+imm;//mac->pc足す！
		break;
		case OP_JALR://なぜかmac->pcはプログラムカウンタの1/4であることに注意しなくて良さそう
			mac->x[rd]=nextpc(mac->pc);
			mac->pc=(mac->x[rs1]+imm);
		break;
		case OP_BRANCH://なぜかmac->pcはプログラムカウンタの1/4であることに注意しなくて良さそう
			switch(funct3){
				case B_EQ:
					if(mac->x[rs1]==mac->x[rs2]){
						mac->pc=mac->pc+imm;
					}else{
						mac->pc=nextpc(mac->pc);
					}
					break;
				case B_NE:
					if(mac->x[rs1]!=mac->x[rs2]){
						mac->pc=mac->pc+imm;
					}else{
						mac->pc=nextpc(mac->pc);
					}
				break;
				case B_LT:
					if(mac->x[rs1]<mac->x[rs2]){
						mac->pc=mac->pc+imm;
					}else{
						mac->pc=nextpc(mac->pc);
					}				
				break;
				case B_GE:
					if(mac->x[rs1]>=mac->x[rs2]){
						mac->pc=mac->pc+imm;		
					}else{
						mac->pc=nextpc(mac->pc);
					}
				break;
				case B_LTU:
					u1=mac->x[rs1];
					u2=mac->x[rs2];
					if(u1<u2){
						mac->pc=mac->pc+imm;		
					}else{
							mac->pc=nextpc(mac->pc);
					}
					break;
					case B_GEU:
						u1=mac->x[rs1];
						u2=mac->x[rs2];
					if(u1>=u2){	
						mac->pc=mac->pc+imm;
					}else{
						mac->pc=nextpc(mac->pc);
					}
					break;
					default:
				          perror("error:funct3branch\n");
			}
		break;
		case OP_LOAD:
			switch(funct3){				
				case LOAD_BYTE_S:
					mac->x[rd]=mac->mem[mac->x[rs1]+imm];
				break;
				case LOAD_HALF_S:
					mac->x[rd]=mac->mem[mac->x[rs1]+imm];
				break;
				case LOAD_WORD:
					mac->x[rd]=mac->mem[mac->x[rs1]+imm];
				break;
				case LOAD_BYTE_Z:
					mac->x[rd]=mac->mem[mac->x[rs1]+imm];
				break;
				case LOAD_HALF_Z:
					mac->x[rd]=mac->mem[mac->x[rs1]+imm];
				break;
				default:
			          perror("error:funct3load\n");				
			}
			mac->pc=nextpc(mac->pc);
		break;
		case OP_STORE://rs2に注意！
			switch(funct3){
				case STORE_BYTE:
					mac->mem[mac->x[rs1]+imm]=mac->x[rs2];
				break;
				case STORE_HALF:	
					mac->mem[mac->x[rs1]+imm]=mac->x[rs2];
				break;
				case STORE_WORD:
					mac->mem[mac->x[rs1]+imm]=mac->x[rs2];
				break;
				default:
			          perror("error:funct3store\n");

			}
			mac->pc=nextpc(mac->pc);
		break;
		case OP_ALU:
			switch(funct3){
				case ALU_ADD:
					switch(funct7){
						case F7_ADD:
							mac->x[rd]=mac->x[rs1]+mac->x[rs2];
						break;
						case F7_SUB:
							mac->x[rd]=mac->x[rs1]-mac->x[rs2];
						break;
						default:
				      			perror("error:funct7\n");
					}
				break;
				case ALU_SLL:
					mac->x[rd]=mac->x[rs1] << mac->x[rs2];
				break;
				case ALU_SLT:
					mac->x[rd]=(mac->x[rs1] < mac->x[rs2])?1:0;
				break;
				case ALU_SLTU:
					u1=mac->x[rs1];
					u2=mac->x[rs2];
					mac->x[rd]=(u1 < u2)?1:0;
				break;
				case ALU_XOR:
					mac->x[rd]=mac->x[rs1]^mac->x[rs2];
				break;
				case ALU_SRX:
					switch(funct7){
						case F7_SRL:
							u1=mac->x[rs1];
							mac->x[rd]=u1 >> mac->x[rs2];
						break;
						case F7_SRA:
							mac->x[rd]=mac->x[rs1] >> mac->x[rs2];
						break;
						default:
				      			perror("error:funct7\n");
					}
				break;
				case ALU_OR:
					mac->x[rd]=mac->x[rs1] | mac->x[rs2];
				break;
				case ALU_AND:
					mac->x[rd]=mac->x[rs1] & mac->x[rs2];
				break;
				default:
			          perror("error:funct3alu\n");				

			}
			mac->pc=nextpc(mac->pc);
		break;
		case OP_ALUI://OP_ALUをコピペ＋α（ただし、SUBはない）
			switch(funct3){
				case ALU_ADD:
					mac->x[rd]=mac->x[rs1]+imm;
				break;
				case ALU_SLL:
					mac->x[rd]=mac->x[rs1] << shamt;//immでないことに注意
				break;
				case ALU_SLT:
					mac->x[rd]=(mac->x[rs1] < imm)?1:0;
				break;
				case ALU_SLTU:
					u1=mac->x[rs1];
					u2=imm;
					mac->x[rd]=(u1 < u2)?1:0;
				break;
				case ALU_XOR:
					mac->x[rd]=mac->x[rs1]^imm;
				break;
				case ALU_SRX:
					switch(funct7){
						case F7_SRL:
							u1=mac->x[rs1];
							mac->x[rd]=u1 >> shamt;//immでないことに注意
						break;
						case F7_SRA:
							mac->x[rd]=mac->x[rs1] >> shamt;//immでないことに注意
						break;
						default:
				      			perror("error:funct7\n");
					}
				break;
				case ALU_OR:
					mac->x[rd]=mac->x[rs1] | imm;//||ではない
				break;
				case ALU_AND:
					mac->x[rd]=mac->x[rs1] & imm;//&&ではない
				break;
				default:
					perror("error:funct3alui\n");				
			}
			mac->pc=nextpc(mac->pc);
		break;
		case OP_LOAD_FP:
			mac->f[rd]=mac->fmem[mac->x[rs1]+imm];
			mac->pc=nextpc(mac->pc);
		break;
		case OP_STORE_FP:
			mac->fmem[mac->x[rs1]+imm]=mac->f[rs2];
			mac->pc=nextpc(mac->pc);	
		break;
		case OP_FP:
			switch(funct7){
				case F7_FADD:
					mac->f[rd]=mac->f[rs1]+mac->f[rs2];
				break;
				case F7_FSUB:
					mac->f[rd]=mac->f[rs1]-mac->f[rs2];
				break;
				case F7_FMUL:
					mac->f[rd]=mac->f[rs1]*mac->f[rs2];
				break;
				case F7_FDIV:
					mac->f[rd]=mac->f[rs1]/mac->f[rs2];
				break;
				case F7_FSQRT:
					mac->f[rd]=sqrtf(fabsf(mac->f[rs1]));
				break;
				case F7_FSGNJ:
					switch(funct3){
						case F3_FSGNJ:
							mac->f[rd]=mac->f[rs1];
						break;
						case F3_FSGNJN:
							mac->f[rd]=-(mac->f[rs1]);
						break;
						case F3_FSGNJX:
							mac->f[rd]=fabsf(mac->f[rs1]);
							//float型fabsfに注意
						break;
						default:
				      			perror("error:funct3fsgnj\n");
					}
				break;
				case F7_FTOI:
					switch(funct3){
						case F3_RDN://floorに注意した
							mac->x[rd]=(int) floorf(mac->f[rs1]);
						break;
						case F3_RNE://ftoiに注意した
							mac->x[rd]=(int) mac->f[rs1];
						break;
						default:
				      			perror("error:funct3ftoi\n");
					}	
				break;
				case F7_FTOX:
					((float *)mac->x)[rd]=mac->f[rs1];
				break;
				case F7_FCMP:
					switch(funct3){
						case F3_FEQ:
						 mac->x[rd]=(mac->f[rs1]==mac->f[rs2])?1:0;
						break;
						case F3_FLT:
						 mac->x[rd]=(mac->f[rs1]<mac->f[rs2])?1:0;
						break;
						case F3_FLE:
						 mac->x[rd]=(mac->f[rs1]<=mac->f[rs2])?1:0;
						break;
						default:
						 printf("error:funct3fcmp%d\n",funct3);
					}	
				break;
				case F7_ITOF:
					mac->f[rd]=(float) mac->x[rs1];
				break;
				case F7_XTOF:
					((int *)mac->f)[rd]=mac->x[rs1];
				break;
				default:
			          perror("error:funct7\n");				
			}
			mac->pc=nextpc(mac->pc);
		break;
//とりあえずコピペ↓
		case OP_LOAD_IO:
//		if(input_index <sld_n_bytes){
						w.x = sld_words[input_index/4].i;
						uint32_t data =  (uint32_t)sld_bytes[input_index++];
						fprintf(log_fp,"\tinput[%d]: %02x\t", input_index-1,data);
						printf("(%12d or %10.5f)\n",w.x,w.f);
						mac->x[rd] = data;
//			}else{
//					opcode=OP_HLT;
//
//			}

			mac->pc=nextpc(mac->pc);			
		break;
		case OP_STORE_IO:
			{
//			if(funct3 == STORE_BYTE){
				if(!out_fp){
					fprintf(log_fp,"[LOG]@exec_instr:\toutput file is not declared\n");
					printf("please input output file name\n>> ");
					char filename[BUF_SIZE];
					if(scanf("%s",filename) == EOF ){
						printf("scanf reach EOF\n");
					}
						out_fp = fopen(filename,"w");
					}
					fwrite(&(mac->x[rs2]),1,1,out_fp);
//					printf("output %d\n",mac->x[rs2]);
				}/*else{
					error = 1;
					fprintf(log_fp,"[ERROR]@exec_instr\t:funct3 of ob should be STORE_BYTE\n");
				}

			}*/
			mac->pc=nextpc(mac->pc);				
		break;
//とりあえずコピペ↑		
		case OP_HLT:break;//while()内で役割を果たしている
		default:
			perror("error:opcode\n");
			exit(0);			
	}
	mac->x[0]=0;//0レジスタは常に0
	mac->f[0]=0;//0レジスタは常に0
	if(mac->x[2] > mac->stack_max) mac->stack_max = mac->x[2];
	if(mac->x[3] > mac->heap_max) mac->heap_max = mac->x[3];

	return opcode;
}

void program_exec(FILE *fp,Machine mac,int breakpoint_nm,int breakpoint_op,int begin_fprintf,int end_fprintf){//breakpointなどは設定しないとき、-1とする。
	BinaryCode bin_code;
	bin_code=file_load(fp);
	int instr_count=0;

	FILE *lfp;
	lfp=fopen("simulator.log","w+");

	int opcode;

	opcode = OP_HLT+1;//とりあえずOP_HLTと異なるようにする。
	while(bin_code[mac->pc]!=NULL&&opcode!=OP_HLT){
		if(instr_count>=begin_fprintf&&instr_count<=end_fprintf)
			print_asm(bin_code[mac->pc],mac,instr_count,lfp);
		if(mac->pc==breakpoint_nm||opcode==OP_HLT){
			break;
		}
		opcode=instr_exec(mac,bin_code[mac->pc]);
		instr_count++;
	}
}
