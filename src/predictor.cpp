//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>

#include <stdlib.h>
#include <stdint.h>
#include <string.h>


#include <math.h>
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "Hengzhou Li";
const char *studentID = "A69033010";
const char *email = "hel053@ucsd.edu";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = {"Static", "Gshare",
                         "Tournament", "Custom"};

// define number of bits required for indexing the BHT here.
int ghistoryBits = 17; // Number of bits used for Global History
int bpType;            // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
// TODO: Add your own Branch Predictor data structures here
//
// gshare
uint8_t *bht_gshare;
uint64_t ghistory;





// Tournament
int pclocal_PHT_bits = 15;
int pclocal_table_bits = 10;
int global_table_bits = 16;
int choser_table_bits = 15;

uint64_t tournament_ghistory;
uint64_t tournament_chistory;


uint32_t *pclocal_PHT_table;
uint8_t *pclocal_table;
uint8_t *global_table;
uint8_t *choser_table;


// Custom TAGE
uint32_t num_tables = 1;
uint32_t table_size = 2184;
uint32_t tag_bits = 10;
int32_t pred_bits = 3; // TAKEN 0 1 2 NOTTAKEN -3 -2 -1
uint32_t useful_bits = 2;
uint32_t history_bits = 32;
uint8_t pc_base_predictor;
uint64_t clear_counter;


uint16_t **tage_tags;  // Tags
int8_t **tage_pred;    // prediction
uint8_t **tage_useful; // u
uint32_t tage_ghr = 0;

uint32_t history_lengths[1] = {16};




//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//

// gshare functions
void init_gshare()
{
  int bht_entries = 1 << ghistoryBits;
  bht_gshare = (uint8_t *)malloc(bht_entries * sizeof(uint8_t));
  int i = 0;
  for (i = 0; i < bht_entries; i++)
  {
    bht_gshare[i] = WN;
  }
  ghistory = 0;
}


void init_tournament()
{
  int pcloacal_tbale_pht_entries = 1 << pclocal_PHT_bits;
  int pclocal_table_bht_entries = 1 << pclocal_table_bits;
  int global_table_bht_entries = 1 << global_table_bits;
  int choser_table_bht_entries = 1 << choser_table_bits;


  pclocal_PHT_table = (uint32_t *)malloc(pcloacal_tbale_pht_entries * sizeof(uint32_t));
  pclocal_table = (uint8_t *)malloc(pclocal_table_bht_entries * sizeof(uint8_t));
  global_table = (uint8_t *)malloc(global_table_bht_entries * sizeof(uint8_t));
  choser_table = (uint8_t *)malloc(choser_table_bht_entries * sizeof(uint8_t));


  // BHT
  int i = 0;
  for (i = 0; i < pclocal_table_bht_entries; i++)
  {
    pclocal_table[i] = WN;
  }
  i = 0;

  for (i = 0; i < global_table_bht_entries; i++)
  {
    global_table[i] = WN;
  }
  i = 0;


  // Chooser and PHT

  for (i = 0; i < pcloacal_tbale_pht_entries; i++)
  {
    pclocal_PHT_table[i] = 0; 
  }
  i = 0;


  for (i = 0; i < choser_table_bht_entries; i++)
  {
    choser_table[i] = 1;
  }
  i = 0;

  ghistory = 0;
  tournament_ghistory = 0;
  tournament_chistory = 0;
}














uint8_t gshare_predict(uint32_t pc)
{
  // get lower ghistoryBits of pc
  uint32_t bht_entries = 1 << ghistoryBits;
  uint32_t pc_lower_bits = pc & (bht_entries - 1);
  uint32_t ghistory_lower_bits = ghistory & (bht_entries - 1);
  uint32_t index = pc_lower_bits ^ ghistory_lower_bits;
  switch (bht_gshare[index])
  {
  case WN:
    return NOTTAKEN;
  case SN:
    return NOTTAKEN;
  case WT:
    return TAKEN;
  case ST:
    return TAKEN;
  default:
    printf("Warning: Undefined state of entry in GSHARE BHT!\n");
    return NOTTAKEN;
  }
}


uint8_t torunament_predict(uint32_t pc){
  uint32_t pcloacal_tbale_pht_entries = (1 << pclocal_PHT_bits)-1;
  uint32_t pclocal_table_bht_entries = (1 << pclocal_table_bits)-1;
  uint32_t global_table_bht_entries = (1 << global_table_bits)-1;
  uint32_t choser_table_bht_entries = (1 << choser_table_bits)-1;

  pcloacal_tbale_pht_entries = pcloacal_tbale_pht_entries & pc;

  pclocal_table_bht_entries = pclocal_PHT_table[pcloacal_tbale_pht_entries];
  
  uint8_t PHT_local_pred = pclocal_table[pclocal_table_bht_entries];
  uint8_t gloabl_pred = global_table[tournament_ghistory];
  uint8_t choser_pred = choser_table[tournament_chistory];

  uint8_t PHT_RESULT = TAKEN;
  uint8_t GLOBAL_RESULT = TAKEN;
  switch (PHT_local_pred)
  {
  case WN:
    PHT_RESULT = NOTTAKEN;
    break;
  case SN:
    PHT_RESULT = NOTTAKEN;
    break;
  case WT:
    PHT_RESULT = TAKEN;
    break;
  case ST:
    PHT_RESULT = TAKEN;
    break;
  }
  switch (gloabl_pred)
  {
  case WN:
    GLOBAL_RESULT = NOTTAKEN;
    break;
  case SN:
    GLOBAL_RESULT = NOTTAKEN;
    break;
  case WT:
    GLOBAL_RESULT = TAKEN;
    break;
  case ST:
    GLOBAL_RESULT = TAKEN;
    break;
  }
  if (choser_pred <= 1){
    return PHT_RESULT;
  }else{
    return GLOBAL_RESULT;
  }
}



void train_gshare(uint32_t pc, uint8_t outcome)
{
  // get lower ghistoryBits of pc
  uint32_t bht_entries = 1 << ghistoryBits;
  uint32_t pc_lower_bits = pc & (bht_entries - 1);
  uint32_t ghistory_lower_bits = ghistory & (bht_entries - 1);
  uint32_t index = pc_lower_bits ^ ghistory_lower_bits;

  // Update state of entry in bht based on outcome
  switch (bht_gshare[index])
  {
  case WN:
    bht_gshare[index] = (outcome == TAKEN) ? WT : SN;
    break;
  case SN:
    bht_gshare[index] = (outcome == TAKEN) ? WN : SN;
    break;
  case WT:
    bht_gshare[index] = (outcome == TAKEN) ? ST : WN;
    break;
  case ST:
    bht_gshare[index] = (outcome == TAKEN) ? ST : WT;
    break;
  default:
    printf("Warning: Undefined state of entry in GSHARE BHT!\n");
    break;
  }

  // Update history register
  ghistory = ((ghistory << 1) | outcome);
}


void train_tournament(uint32_t pc, uint8_t outcome){
  uint32_t pcloacal_tbale_pht_entries = (1 << pclocal_PHT_bits)-1;
  uint32_t pclocal_table_bht_entries = (1 << pclocal_table_bits)-1;
  uint32_t global_table_bht_entries = (1 << global_table_bits)-1;
  uint32_t choser_table_bht_entries = (1 << choser_table_bits)-1;

  pcloacal_tbale_pht_entries = pcloacal_tbale_pht_entries & pc;

  pclocal_table_bht_entries = pclocal_PHT_table[pcloacal_tbale_pht_entries];
  
  uint8_t PHT_local_pred = pclocal_table[pclocal_table_bht_entries];
  uint8_t gloabl_pred = global_table[tournament_ghistory];
  uint8_t choser_pred = choser_table[tournament_chistory];

  // Update the choser table entry
  uint8_t PHT_RESULT = TAKEN;
  uint8_t GLOBAL_RESULT = TAKEN;
  switch (PHT_local_pred)
  {
  case WN:
    PHT_RESULT = NOTTAKEN;
    break;
  case SN:
    PHT_RESULT = NOTTAKEN;
    break;
  case WT:
    PHT_RESULT = TAKEN;
    break;
  case ST:
    PHT_RESULT = TAKEN;
    break;
  }
  switch (gloabl_pred)
  {
  case WN:
    GLOBAL_RESULT = NOTTAKEN;
    break;
  case SN:
    GLOBAL_RESULT = NOTTAKEN;
    break;
  case WT:
    GLOBAL_RESULT = TAKEN;
    break;
  case ST:
    GLOBAL_RESULT = TAKEN;
    break;
  }

  if (PHT_RESULT!=GLOBAL_RESULT){
    if (PHT_RESULT == outcome && choser_pred > 0){
      choser_table[tournament_chistory] --;
    }else if(GLOBAL_RESULT == outcome && choser_pred < 3){
      choser_table[tournament_chistory] ++;
    }
  }



  // Update the PHT BHT entry
  if (outcome == TAKEN && PHT_local_pred < ST){
    pclocal_table[pclocal_table_bht_entries] ++;
  }else if(outcome==NOTTAKEN && PHT_local_pred > SN){
    pclocal_table[pclocal_table_bht_entries] --;
  }

  // Update the Global table entry
  if (outcome == TAKEN && gloabl_pred < ST){
    global_table[tournament_ghistory] ++;
  }else if(outcome == NOTTAKEN && gloabl_pred > SN){
    global_table[tournament_ghistory] --;
  }



  // Update the PHT entry
  pclocal_PHT_table[pcloacal_tbale_pht_entries] = (pclocal_table_bht_entries << 1) + outcome;
  tournament_ghistory = (tournament_ghistory << 1) + outcome;
  tournament_chistory = (tournament_ghistory << 1) + outcome;

  
  pclocal_PHT_table[pcloacal_tbale_pht_entries] = ((1 << pclocal_table_bits)-1) & pclocal_PHT_table[pcloacal_tbale_pht_entries];
  tournament_ghistory = tournament_ghistory & global_table_bht_entries;
  tournament_chistory = tournament_chistory & choser_table_bht_entries;
}




void cleanup_gshare()
{
  free(bht_gshare);
}

void cleanup_torunament(){
  free(pclocal_PHT_table);
  free(pclocal_table);
  free(global_table);
  free(choser_table);
}


uint32_t hash_to_index(uint32_t pc, uint32_t history, int history_len) {
    return (pc ^ (history * 31)) % table_size;
}

uint16_t hash_to_tag(uint32_t pc) {
    return (pc ^ (pc >> 5)) & ((1 << tag_bits) - 1);
}

void init_custom() {
    tage_tags = (uint16_t **)malloc(num_tables * sizeof(uint16_t*));
    tage_pred = (int8_t **)malloc(num_tables * sizeof(int8_t*));
    tage_useful = (uint8_t **)malloc(num_tables * sizeof(uint8_t*));
    for (int i = 0; i < num_tables; i++) {
        tage_tags[i] = (uint16_t*)malloc(table_size *sizeof(uint16_t));
        tage_pred[i] = (int8_t *)malloc(table_size * sizeof(int8_t));
        tage_useful[i] = (uint8_t *)malloc(table_size * sizeof(uint8_t));
        for (int j = 0; j < table_size; j++) {
            tage_tags[i][j] = 0;
            tage_pred[i][j] = 0;
            tage_useful[i][j] = 0;
        }
    }
    tage_ghr = 0;
    pc_base_predictor = WT;
    clear_counter = 1<<18;
    pclocal_PHT_bits = 16;
    pclocal_table_bits = 10;
    global_table_bits = 17;
    choser_table_bits = 16;
    init_tournament();

}

// Predict outcome
uint8_t custom_predict(uint32_t pc) {
    uint8_t prediction = torunament_predict(pc);
    // switch (pc_base_predictor)
    // {
    // case WN:
    //   prediction =  NOTTAKEN;
    // case SN:
    //   prediction =  NOTTAKEN;
    // case WT:
    //   prediction =  TAKEN;
    // case ST:
    //   prediction =  TAKEN;
    // }
    int most_significant_table = -1;
    for (int i = 0; i < num_tables; i++){
      uint32_t idx = hash_to_index(pc, tage_ghr, history_lengths[i]);
      if (tage_tags[i][idx] == hash_to_tag(pc)) {
          most_significant_table = i;
        }
    }
    if (most_significant_table == -1){
      return prediction;  // PC base prediction T0
    }else{
      uint32_t table_entry_index = hash_to_index(pc, tage_ghr, history_lengths[most_significant_table]);
      int8_t prediction_counter = tage_pred[most_significant_table][table_entry_index];
      if (prediction_counter >= 0){
        return TAKEN;
      }else{
        return NOTTAKEN;
      }
    }
}

void train_custom(uint32_t pc, uint8_t outcome) {

    int most_significant_table = -1;
    for (int i = 0; i < num_tables; i++){
      uint32_t idx = hash_to_index(pc, tage_ghr, history_lengths[i]);
      if (tage_tags[i][idx] == hash_to_tag(pc)) {
          most_significant_table = i;
        }
    }

    uint8_t prediction_outcome = custom_predict(pc);

    if (most_significant_table != -1) {
        uint32_t target_idx = hash_to_index(pc, tage_ghr, history_lengths[most_significant_table]);
        if (outcome == TAKEN && tage_pred[most_significant_table][target_idx] < 3){
          tage_pred[most_significant_table][target_idx] ++;
        }else if (outcome == NOTTAKEN && tage_pred[most_significant_table][target_idx] > -4){
          tage_pred[most_significant_table][target_idx] --;
        }

        if (outcome == prediction_outcome && tage_useful[most_significant_table][target_idx] < 3){
          tage_useful[most_significant_table][target_idx] ++;
        }else if (outcome != prediction_outcome && tage_useful[most_significant_table][target_idx] >0){
          tage_useful[most_significant_table][target_idx] --;
        }
    } else {
        // Allocate new entry
        for (int i = 0; i < num_tables; i++) {
            uint32_t idx = hash_to_index(pc, tage_ghr, history_lengths[i]);
            if (tage_useful[i][idx] == 0) {
                tage_tags[i][idx] = hash_to_tag(pc);
                if (outcome == 1){
                  tage_pred[i][idx] = 0;
                }else{
                  tage_pred[i][idx] = -1;
                }
                tage_useful[i][idx] = 1;
                break;
            }else{
              tage_useful[i][idx] --;
            }
        }
    }

    // if (outcome == TAKEN && pc_base_predictor < 3){
    //       pc_base_predictor ++;
    //     }else if (outcome == NOTTAKEN && pc_base_predictor > 0){
    //       pc_base_predictor --;
    //     }

    train_tournament(pc, outcome);


    
    clear_counter --;
    if (clear_counter == 0){
      clear_counter == 1<<18;
      for (int i = 0; i < num_tables; i++) {
         for (int j =0; j < table_size; j++){
            tage_useful[i][j] = 0;
         }
      }
    }
    tage_ghr = ((tage_ghr << 1) + outcome) & ((1<<(history_bits-1))-1 + (1<<(history_bits-1)));
}

// Clean up memory
void custom_clean() {
    for (int i = 0; i < num_tables; i++) {
        free(tage_tags[i]);
        free(tage_pred[i]);
        free(tage_useful[i]);
    }
    free(tage_tags);
    free(tage_pred);
    free(tage_useful);
    cleanup_torunament();
}


void init_predictor()
{
  switch (bpType)
  {
  case STATIC:
    break;
  case GSHARE:
    init_gshare();
    break;
  case TOURNAMENT:
    init_tournament();
    break;
  case CUSTOM:
    init_custom();
    break;
  default:
    break;
  }
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint32_t make_prediction(uint32_t pc, uint32_t target, uint32_t direct)
{

  // Make a prediction based on the bpType
  switch (bpType)
  {
  case STATIC:
    return TAKEN;
  case GSHARE:
    return gshare_predict(pc);
  case TOURNAMENT:
    return torunament_predict(pc);
  case CUSTOM:
    return custom_predict(pc);
  default:
    break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//

void train_predictor(uint32_t pc, uint32_t target, uint32_t outcome, uint32_t condition, uint32_t call, uint32_t ret, uint32_t direct)
{
  if (condition)
  {
    switch (bpType)
    {
    case STATIC:
      return;
    case GSHARE:
      return train_gshare(pc, outcome);
    case TOURNAMENT:
      return train_tournament(pc, outcome);
    case CUSTOM:
      return train_custom(pc, outcome);
    default:
      break;
    }
  }
}
