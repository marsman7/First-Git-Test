// Funktionen zum berechnen des Ergebnis

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include "calc.h"

char *termstart;
int64_t  result;
//uint16_t FehlerPos;     //: integer;
uint16_t FehlerNum = OK;
char FehlerText[MAX_ERROR_LEN] = {0};     //: string;
//void *pVari = 0;     //: PVarArray;

// Zeichen bzw. Symbole für Rechenoperation
//    << Bits links schieben, >> Bits rechts schieben
//    & AND, | OR, ~ XOR
const char *OpStr[OP_NUMBER] = {"+","-","*","/",">>","<<","&","|","~"};      //: array[0..8] of string = ('+','-','*','/','>>','<<','&','|','~');

/** ***************************************************************************
 * Name
 *
 * Discription
 *
 * @param term  Term-String, der berechnet werden soll
 * @return Fehlernummer
 *************************************************************************** */
uint16_t calc_term(char *term)
{
    result = 0;
    if (!term) return ERR_TERM_EMPTY;

    if (!strlen(term)) return ERR_TERM_EMPTY;

    printf("Term [%s]\n", term);

    termstart = term;
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
uint16_t term_disassemble(char *term, char *term_end, int64_t *subresult)
{
    char *pPos = term;         // Zeiger auf Position die verarbeitet wird
    char *pEnd = term_end;   // Zeiger auf Ende des Terms
    char *pOpp = NULL;
    int32_t i = 0;      //: integer;
    uint32_t n;         //: integer;
    uint64_t left_opp = 0;        //left operant : integer;
    uint64_t right_opp = 0;        //right operant : integer;

    uint64_t tempresult = 0;

    if (FehlerNum != OK) {
        return FehlerNum;
    }

    // Trim left (find begin)
    while ((*pPos) && isspace(*pPos)) {
        pPos++;
    }

    // Trim right (find end)
    while(pEnd > pPos) {
        pEnd--;
        if (!isspace(*pEnd)) {
            break;
        }
    }

    if (pEnd == pPos) {
      // Sollte eigentlich nie auftreten
      result = 0;
      return OK;
    }

    printf("Subterm start[%d] end[%d] schar[%c] echar[%c]\n"
        , (uint32_t)(pPos-termstart), (uint32_t)(pEnd-termstart), pPos, pEnd);

    i = 0;
    while (i < OP_NUMBER) {
      if (find_operation(pPos, OpStr[i], &pOpp)) {
        if (term_disassemble(pPos, pOpp-1, &left_opp)) {
          return FehlerNum;
        }
        if (term_disassemble(pOpp+1, pEnd, &right_opp)) {
          return FehlerNum;
        }

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
        *subresult = tempresult;
        return OK;
      }
      i++;
    }

    // Ist Term in Klammern
    if (*pPos == '(') {
        if (pEnd == ')') {
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
      char *pVarEnd = pPos;
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

    // Wenn Term eine Zahl ist
//    if (term[1] in ['0'..'9']) {
    if (isdigit(*pPos)) {
      while(isdigit(*pPos)) {
        tempresult *= 10;
        tempresult += *pPos & 0x0f;
      }
      printf("Zahl erkannt %d\n", tempresult);
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
 * @return Fehler bool
 *************************************************************************** */
bool find_operation(char *subterm, char *Opp, char **pPos)
{
  uint16_t i = 0;   // Zeichenzähler
  uint16_t l = 0;   // Merker für subterm Länge
  uint16_t k = 0;   // Klammerzähler, '(' plus 1 ; ')' minus 1

//  for (n = 1; n <= strlen(subterm); n++) {
  l = strlen(subterm);
  while (i < l) {
    if (*subterm == '(') {
      k++;
    }

    if (*subterm == ')') {
      k--;
      if (k < 0) {
        FehlerNum = ERR_BRACKET_OPEN;
        snprintf(FehlerText, MAX_ERROR_LEN, "Klammer nicht geöffnet.");
        return false;
      }
    }

    if ((k = 0) && strncmp(subterm, Opp, strlen(Opp))) {
      *pPos = subterm;
      return true;
    }
    i++;
    subterm++;
  }

  *pPos = 0;
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
int64_t GetCalcValue() {
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
