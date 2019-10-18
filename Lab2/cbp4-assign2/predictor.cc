#include "predictor.h"

/////////////////////////////////////////////////////////////
// 2bitsat
/////////////////////////////////////////////////////////////

int two_bit_pt[4096];

void InitPredictor_2bitsat() {
//initialize pht with weak not taken
    for(int i=0;i<4096;i++){
        two_bit_pt[i]=1;
    }
}

bool GetPrediction_2bitsat(UINT32 PC) {
    UINT32 id= PC % 4096;
    int check=two_bit_pt[id];
    if(check==0||check==1)
        return NOT_TAKEN;
    else if(check==2||check==3)
        return TAKEN;
}

void UpdatePredictor_2bitsat(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget) {
    UINT32 id= PC % 4096;
    int check=two_bit_pt[id];
    if(resolveDir==false && check>0)
        two_bit_pt[id]--;
    else if(resolveDir==true && check<3)
        two_bit_pt[id]++;
        
}

/////////////////////////////////////////////////////////////
// 2level
/////////////////////////////////////////////////////////////

uint8_t two_level_pht[8][64]; //8 pattern history tables with 6 bits
uint8_t bht[512]; // 512 bits index for PC[11:3]
void InitPredictor_2level() {
    //init pht
    for(int i=0;i<8;i++){
        for(int j=0;j<64;j++){
            two_level_pht[i][j]=1;
        }
    }
    
    //init bht
    for(int i=0; i<512;i++)
        bht[i] =0;
}

bool GetPrediction_2level(UINT32 PC) {
//PC[2:0] would be pht id PC[11:3] would be BHT id
    UINT32 id_pht=PC & 0b111;
    UINT32 id_bht=PC & 0b111111111000;
    id_bht=id_bht>>3;
    
    uint8_t check=two_level_pht[id_pht][bht[id_bht]];
    
    if(check==0 || check ==1)
        return NOT_TAKEN;
    else if(check==2||check==3)
        return TAKEN;
    
  return TAKEN;
}

void UpdatePredictor_2level(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget) {
    UINT32 id_pht=PC & 0b111;
    UINT32 id_bht=PC & 0b111111111000;
    id_bht=id_bht>>3;
    
    uint8_t check=two_level_pht[id_pht][bht[id_bht]];
    
    //update pht
    if(resolveDir==true && check<3)
        two_level_pht[id_pht][bht[id_bht]]++;
    else if(resolveDir==false && check>0)
        two_level_pht[id_pht][bht[id_bht]]--;
    
    
    //update bht
    //each bht entry should contain 6 bits for pht
    bht[id_bht]=((bht[id_bht]<<1)|resolveDir) & 0b111111;
    
}

/////////////////////////////////////////////////////////////
// openend
/////////////////////////////////////////////////////////////

//maximum 131072 bits 
int const y=88;
int const z=88;
short weight[y][z];//88*88*16 =123904
short bias[y]; //smaller than 65536 88*16=1408
short x[z]; //global brach history register works in circular buffer 88*16=1408
int position;
////y=wx+b

void InitPredictor_openend() {
    for (int i = 0; i < y; i++) {
        for (int j = 0; j < z; j++) {
            weight[i][j] = 0;
        }
    }

    for (int i = 0; i < y; i++) {
        bias[i] = 1;
    }

    for (int i = 0; i < z; i++) {
        x[i] = -1;

    }


    position = 0; //start at the first position
    
    

}

bool GetPrediction_openend(UINT32 PC) {
    int id = PC % y;
    int check = bias[id];
    for (int i = 0; i < z; i++) {
        check = check + weight[id][i] * x[(position + i) % z];
    }
    if (check >= 0)
        return TAKEN;
    else
        return NOT_TAKEN;
}

void UpdatePredictor_openend(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget) {
    uint8_t id = PC % y;
    int check = bias[id];
    for (int i = 0; i < z; i++) {
        check = check + weight[id][i] * x[(position + i) % z];
    }
    int t = 0;
    if (resolveDir)
        t = 1;
    else
        t = -1;

    if ((abs(check) < 200) || predDir != resolveDir) {
            //  if(h<100){
    //    printf("h: %d %d \n",h,check);
          //    printf("w: %d \n",weight[id][0]);
          //    printf("x: %d \n", x[position]);
          //    }
        if (abs(bias[id]) < 65535 - t)
            bias[id] += t;
        for (int i = 0; i < z; i++) {
            if (abs(weight[id][i]+t * x[(position + i) % 88]) < 65535 )
                weight[id][i] = weight[id][i] + t * x[(position + i) % z];
        }
    }

    x[position] = t;
   // if(h<100)
      //  printf("check: %d\n",x[position]);
    position = (1 + position) % z;

}
