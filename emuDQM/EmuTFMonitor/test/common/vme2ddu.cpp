#include<stdio.h>
#include<stdlib.h>
// gcc -o vme2ddu vme2ddu.cpp

int main(int argc, char *argv[]){
	if(argc!=3&&argc!=4){ printf("Arguments: 1) input VME ascii dump 2) output DDU-Spy like binary file [3) print specified event]\n"); exit(0); }
	FILE *in=0, *out=0;
	if( (in =fopen(argv[1],"rw"))==NULL ){ printf("Cannot open input file\n");  exit(0); }
	if( (out=fopen(argv[2],"wt"))==NULL ){ printf("Cannot open output file\n"); exit(0); }
	int print_event=(argc==4?atoi(argv[3]):-2);
	unsigned long val1=0, val2=0, val3=0, val4=0, nevent=0, newline=0;
	bool trailer = false;
	//while( !feof(in) && fscanf(in,"%x",&val1)==1 && fscanf(in,"%x",&val2)==1 && fscanf(in,"%x",&val3)==1 && fscanf(in,"%x",&val4)==1 ){
	while( !feof(in) && fscanf(in,"%x",&val4)==1 /*&& nevent<10*/ ){
		newline++;
		if( (val1&0xF000)==0x9000 && (val2&0xF000)==0x9000 && (val3&0xF000)==0x9000 && (val4&0xF000)==0x9000 ){
			unsigned long long dduHeader1  = 0x500000017512f850LL;
			unsigned long long dduHeader2  = 0x8000000180000000LL;
			unsigned long long dduHeader3  = 0x01ff121000000080LL;
			if(argc==3){
				fwrite(&dduHeader1,8,1,out);
				fwrite(&dduHeader2,8,1,out);
				fwrite(&dduHeader3,8,1,out);
			}
			nevent++;
			newline=0;
			trailer = false;
		}
		if(print_event==nevent-1 && newline%4==0) printf("%x %x %x %x\n",val1,val2,val3,val4);

		if( argc==3 && newline%4==0 ){
			fwrite(&val1,2,1,out);
			fwrite(&val2,2,1,out);
			fwrite(&val3,2,1,out);
			fwrite(&val4,2,1,out);
		}

		if( (val1&0xF000)==0xE000 && (val2&0xF000)==0xE000 && (val3&0xF000)==0xE000 && (val4&0xF000)==0xE000 && !trailer ){
			unsigned long long dduTrailer1 = 0x8000ffff80008000LL;
			unsigned long long dduTrailer2 = 0x2000000000000000LL;
			unsigned long long dduTrailer3 = 0xa00000068e9f0080LL;
			if(argc==3){
				fwrite(&dduTrailer1,8,1,out);
				fwrite(&dduTrailer2,8,1,out);
				fwrite(&dduTrailer3,8,1,out);
			}
			trailer = true;
		}
		val1=val2;
		val2=val3;
		val3=val4;
	}
	fclose(in);
	fclose(out);
	return 0;
}
