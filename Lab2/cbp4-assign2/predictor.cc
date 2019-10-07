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
        two_bit_pt[id]++;
    else if(resolveDir==true && check<3)
        two_bit_pt[id]--;
        
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

void InitPredictor_openend() {

}

bool GetPrediction_openend(UINT32 PC) {

  return TAKEN;
}

void UpdatePredictor_openend(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget) {

}

