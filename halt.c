
/********************** HALT.C ***********************/
/*
 * Rory Barnes, Wed May  7 16:19:48 PDT 2014
 *
 * This file contains all the checks for dynamical states
 * that result in the termination of the program.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "vplanet.h"

/* Minimum obliquity? */
int HaltMinObl(CONTROL *control,BODY *body,UPDATE *update,int iBody) {
  if (body[iBody].dObliquity < control->Halt.dMinObl[iBody]) {
    if (control->iVerbose >= VERBPROG) {
      printf("HALT: Body %s's Obliquity = ",body[iBody].cName);
      fprintd(stdout,body[iBody].dObliquity,control->iSciNot,control->iDigits);
      printf(", < minimum obliquity = ");
      fprintd(stdout,control->Halt.dMinObl[iBody],control->iSciNot,control->iDigits);
      printf(" at %.2e years.\n",control->Integr.dTime/YEARSEC);
    }
    return 1;
  }

  return 0;
}

/* Maximum Eccentricity? */
int HaltMaxEcc(CONTROL *control,BODY *body,UPDATE *update) {  
  if (body[1].dEcc > control->Halt.dMaxEcc) {
    if (control->iVerbose >= VERBPROG) {
      printf("HALT: e = ");
      fprintd(stdout,body[1].dEcc,control->iSciNot,control->iDigits);
      printf(", > max e = ");
      fprintd(stdout,control->Halt.dMaxEcc,control->iSciNot,control->iDigits);
      printf(" at %.2e years\n",control->Integr.dTime/YEARSEC);
    }
    return 1;
  }

  return 0;
}

/* Minimum Eccentricity? */
int HaltMinEcc(CONTROL *control,BODY *body,UPDATE *update) {
  if (body[1].dEcc < control->Halt.dMinEcc) {
    if (control->iVerbose >= VERBPROG) {
      printf("HALT: e = ");
      fprintd(stdout,body[1].dEcc,control->iSciNot,control->iDigits);
      printf(", < min e = ");
      fprintd(stdout,control->Halt.dMinEcc,control->iSciNot,control->iDigits);
      printf(" at %.2e years\n",control->Integr.dTime/YEARSEC);
    }
    return 1;
  }

  return 0;
}

/* Positive de/dt? */
int HaltPosDeccDt(CONTROL *control,BODY *body,UPDATE *update) {
  if (*(update[1].pdDeccDt) > 0 && control->Halt.bPosDeDt) {
    if (control->iVerbose >= VERBPROG) {
      printf("HALT: de/dt = ");
      fprintd(stdout,*(update[1].pdDeccDt),control->iSciNot,control->iDigits);
      printf(" at %.2e years\n",control->Integr.dTime/YEARSEC);
    }
    return 1;
  }

  return 0;
}

/* Merge? */
int HaltMerge(CONTROL *control,BODY *body,UPDATE *update) {
  if (body[1].dSemi*(1-body[1].dEcc) <= (body[0].dRadius + body[1].dRadius) && control->Halt.bMerge) { /* Merge! */
    if (control->iVerbose > VERBPROG) 
      printf("HALT: Merge at %.2e years!\n",control->Integr.dTime/YEARSEC);

    return 1;
  }

  return 0;
}

int bCheckHalt(CONTROL *control,VPLANET *vplanet,BODY *body,UPDATE *update,double dTime) {
  int iBody,iHalt;
  double dMeanMotion;

  dMeanMotion = fdSemiToMeanMotion(body[1].dSemi,(body[0].dMass+body[1].dMass));

  /* Global Halts */
  for (iHalt=0;iHalt<MODULEHALTSYSEND;iHalt++) {
    if (vplanet->fnHaltSys[iHalt](control,body,update))
      return 1;
  }

  /* Now check for body halts */
  for (iBody=0;iBody<2;iBody++) {
    for (iHalt=0;iHalt<MODULEHALTBODYEND;iHalt++) {
      if (vplanet->fnHaltBody[iHalt](control,body,update,iBody))
	return 1;
    }
  }

  return 0;
}

void InitializeHalt(HALT *halt,VPLANET *vplanet,int iNumInputs) {
  int iHaltBody=0,iHaltSys=0;

  halt->dMinObl=malloc(iNumInputs*sizeof(double));
  
  vplanet->fnHaltSys=malloc(MODULEHALTSYSEND*sizeof(fnHaltSystem));
  vplanet->fnHaltBody=malloc(MODULEHALTBODYEND*sizeof(fnHaltBod));


  vplanet->fnHaltSys[iHaltSys++] = &HaltMerge;
  vplanet->fnHaltSys[iHaltSys++] = &HaltMinEcc;
  vplanet->fnHaltSys[iHaltSys++] = &HaltMaxEcc;
  vplanet->fnHaltSys[iHaltSys++] = &HaltPosDeccDt;

  vplanet->fnHaltBody[iHaltBody++] = &HaltMinObl;

  /* Needs to be vectorized VPL */
  InitializeHaltEqtide(halt,vplanet,iNumInputs,&iHaltSys,&iHaltBody);
}
