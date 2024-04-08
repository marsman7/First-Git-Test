// Funktionen zum berechnen des Ergebnis

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "calc.h"

const char *termstart;
int64_t  result;
//uint16_t FehlerPos;     //: integer;
uint16_t FehlerNum = OK;
char FehlerText[MAX_ERROR_LEN] = {0};     //: string;
//void *pVari = 0;     //: PVarArray;

// Zeichen bzw. Symbole für Rechenoperation
//    << Bits links schieben, >> Bits rechts schieben
//    & AND, | OR, ~ XOR
const char *OpStr[OP_NUMBER] = {"+","-","*","/",">>","<<","&","|","~"};      //: array[0..8] of string = ('+','-','*','/','>>','<<','&','|','~');

bool find_operation(const char *pStart, const char *pEnd, const char *Opp, const char **pOpp);
uint16_t term_disassemble(const char *term, const char *term_end, int64_t *subresult);

/** ***************************************************************************
 * Name
 *
 * Discription
 *
 * @param term  Term-String, der berechnet werden soll
 * @return Fehlernummer
 *************************************************************************** */
uint16_t calc_term(const char *term)
{
    result = 0;
    FehlerNum = OK;
    if (!term) {
      FehlerNum = ERR_TERM_EMPTY;
      return FehlerNum;
    }

    if (!strlen(term)) {
      FehlerNum = ERR_TERM_EMPTY;
      return FehlerNum;
    }

    termstart = term;

    printf("Start Term [%s] %p %p \n", term, term, term + strlen(term));

    return term_disassemble(term, term + strlen(term) - 1, &result);
}

/** ***************************************************************************
 * term_disassemble : 
 *
 * Teilt den Term in seine Bestandteile auf
 *
 * @param term      String des Term der entschlüsselt werden soll
 * @param term_end  Zeige auf letztes Zeichen des Terms
 * @return Fehlernummer
 *************************************************************************** */
uint16_t term_disassemble(const char *term, const char *term_end, int64_t *subresult)
{
    const char *pPos = term;         // Zeiger auf Position die verarbeitet wird
    const char *pEnd = term_end;   // Zeiger auf Ende des Terms
    const char *pOpp = NULL;
    int32_t i = 0;      //: integer;
    int64_t left_opp = 0;        //left operant : integer;
    int64_t right_opp = 0;        //right operant : integer;
    int64_t tempresult = 0;

    printf("Sub Term 1 [%ld] [%ld] \n", pPos-termstart, pEnd-termstart);

    if (FehlerNum != OK) {
        return FehlerNum;
    }

    // Trim left (find begin)
    while (isspace(*pPos)) {
        pPos++;
    }

    printf("Sub Term 2 [%ld] [%ld] \n", pPos-termstart, pEnd-termstart);

    // Trim right (find end)
    while(pEnd > pPos) {
        if (!isspace(*pEnd)) {
            break;
        }
        pEnd--;
    }

    printf("Sub Term 3 [%ld] [%ld] \n", pPos-termstart, pEnd-termstart);

    printf("Subterm start[%d] end[%d] schar[%c] echar[%c]\n"
        , (uint16_t)(pPos-termstart), (uint16_t)(pEnd-termstart), (char)*pPos, (char)*pEnd);

    i = 0;
    while (i < OP_NUMBER) {
      if (find_operation(pPos, pEnd, OpStr[i], &pOpp)) {
        printf("Operation erkannt [%s] Pos[%d]\n", OpStr[i], (uint16_t)(pOpp-termstart));
        if (term_disassemble(pPos, pOpp-1, &left_opp)) {
          return FehlerNum;
        }
        if (term_disassemble(pOpp+1, pEnd, &right_opp)) {
          return FehlerNum;
        }

        printf("Rechen Operanten %ld %s %ld\n", left_opp, OpStr[i], right_opp);

        switch (i) {
          case 0: tempresult = left_opp + right_opp; break;
          case 1: tempresult = left_opp - right_opp; break;
          case 2: tempresult = left_opp * right_opp; break;
          case 3: tempresult = left_opp / right_opp; break;
          case 4: tempresult = left_opp >> right_opp; break;
          case 5: tempresult = left_opp << right_opp; break;
          case 6: tempresult = left_opp & right_opp; break;
          case 7: tempresult = left_opp | right_opp; break;
          case 8: tempresult = left_opp ^ right_opp; break;
          default:
              ;
        }
        printf("Zwischenergebnis %ld\n", tempresult);
        *subresult = tempresult;
        return OK;
      }
      i++;
    }

    // Ist Term in Klammern
    if (*pPos == '(') {
        if (*pEnd == ')') {
          printf("Klammerauflösung\n");
          if (term_disassemble(pPos+1, pEnd-1, &tempresult)) {
            return FehlerNum;
          }
          *subresult = tempresult;
          return OK;
        } else {
          FehlerNum = ERR_BRACKET_CLOSE;
          return FehlerNum;
        }
    }

    // Auf Variable oder Funktion prüfen
    if (isalpha(*pPos) || *pPos == '_') {
      printf("Variablenauflösung\n");
      uint16_t i = 0;
      char *pVarEnd;
      pVarEnd = (char*)pPos;
#warning Fehler zu langer Variablenname abfangen              
      while ((pVarEnd <= pEnd) && (isalnum(*pVarEnd) || *pVarEnd == '_')) {
        pVarEnd++;
        i++;
      }
      char varName[64] = {0};
      strncpy(varName, pPos, i-1);
      *pVarEnd = '\0';
      printf("Variable %s erkannt\n", varName);

      snprintf(FehlerText, MAX_ERROR_LEN, "Syntax Fehler, Variable unbekannt.");
      FehlerNum = ERR_UNKNOWN_VARIABLE;
      return FehlerNum;
    }

    // Wenn mit Komma beginnt = 0,xx
    if (*pPos == ',') {

    }

    // Wenn Term eine Zahl ist
    if (isdigit(*pPos)) {
      while(isdigit(*pPos) && pPos <= pEnd) {
        tempresult *= 10;
        tempresult += *pPos & 0x0f;
        pPos++;
      }
      printf("Zahl erkannt %ld\n", tempresult);
      *subresult = tempresult;
    } else {
      FehlerNum = ERR_UNKNOWN;
      return FehlerNum;
    }

    return OK;
}
// ^^ TermZerlegung ^^^^^^^^^^^^^^^^^^^^^^^^^^^

/** ***************************************************************************
 * find_operation
 *
 * Sucht die Rechenopertion, und gibt die Position zurück. In Klammern wird
 * nicht gesucht.
 *
 * @param Term  Term, der nach einer Operation/Verknüpfung durchsucht werden soll
 * @param Opp   Operation/Verknüpfung nach der in Term gesucht werden soll
 * @param pPos  Rückgabe der Position an der die Operation gefunden wurde
 * @return true wenn Rechenoperation gefunden
 *************************************************************************** */
//bool find_operation(const char *subterm, const char *Opp, const char **pPos)
bool find_operation(const char *pPos, const char *pEnd, const char *Opp, const char **pOpp)
{
  uint16_t i = 0;   // Zeichenzähler
  uint16_t k = 0;   // Klammerzähler, '(' plus 1 ; ')' minus 1

  while (pPos <= pEnd) {
    if (*pPos == '(') {
      k++;
    }

    if (*pPos == ')') {
      if (k == 0) {
        FehlerNum = ERR_BRACKET_OPEN;
        snprintf(FehlerText, MAX_ERROR_LEN, "Klammer nicht geöffnet.");
        return false;
      }
      k--;
    }

    if ((k == 0) && !strncmp(pPos, Opp, strlen(Opp))) {
      printf("Find Opp start[%d] end[%d] Opp[%s]\n", (uint16_t)(pPos-termstart), (uint16_t)(pEnd-termstart), Opp);
      *pOpp = pPos;
      return true;
    }
    i++;
    pPos++;
  }

  if (k > 0) {
    FehlerNum = ERR_BRACKET_CLOSE;
    snprintf(FehlerText, MAX_ERROR_LEN, "Geöffnete Klammer gefunden, die nicht geschlossen wird.");
    return false;
  }

  return false;
}
// ^^ find_operation ^^^^^^^^^^^^^^^^^^^^^^^^^^

/** ***************************************************************************
 * Name
 *
 * Discription
 *
 * @param a  Discription
 * @return no
 *************************************************************************** */
int64_t get_calc_result() {
  return result;
}

/** ***************************************************************************
 * Name
 *
 * Discription
 *
 * @param a  Discription
 * @return no
 *************************************************************************** */
int16_t GetFehler() {
  return FehlerNum;
}

/** ***************************************************************************
 * Name
 *
 * Discription
 *
 * @param a  Discription
 * @return no
 *************************************************************************** */
const char *GetFehlerText() {
  return NULL;
}
